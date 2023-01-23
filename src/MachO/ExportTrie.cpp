/*
 * MachO/ExportTrie.cpp
 * © suhas pai
 */

#include <algorithm>

#include "MachO/ExportTrie.h"
#include "Utils/Leb128.h"
#include "Utils/Overflow.h"

namespace  MachO {
    ExportTrieIterator::ExportTrieIterator(const uint8_t *Begin,
                                           const uint8_t *End,
                                           const ParseOptions &Options) noexcept
    : Begin(Begin), End(End)
    {
        Info = std::make_unique<ExportTrieIterateInfo>();
        Info->setMaxDepth(Options.MaxDepth);

        Info->rangeListRef().reserve(Options.RangeListReserveSize);
        Info->stackListRef().reserve(Options.StackListReserveSize);
        Info->stringRef().reserve(Options.StringReserveSize);

        auto Node = NodeInfo();
        this->ParseError = ParseNode(Begin, &Node);

        if (ParseError != Error::None) {
            return;
        }

        NextStack = std::make_unique<StackInfo>(Node);
        this->ParseError = Advance();
    }

    auto ExportTrieIterator::SetupInfoForNewStack() noexcept -> void {
        Info->stringRef().append(NextStack->node().prefix());
        Info->stackListRef().emplace_back(std::move(*NextStack));
    }

    auto ExportTrieIterator::MoveUptoParentNode() noexcept -> bool {
        auto &StackList = Info->stackListRef();
        if (StackList.size() == 1) {
            StackList.clear();
            return false;
        }

        auto &String = Info->stringRef();
        auto &Top = StackList.back();

        const auto EraseSuffixLength =
            String.length() - Top.node().prefix().length();

        String.erase(EraseSuffixLength);
        if (Top.rangeListSize() != 0) {
            auto &RangeList = Info->rangeListRef();
            RangeList.erase(RangeList.cbegin() +
                                static_cast<long>(Top.rangeListSize()),
                            RangeList.cend());
        }

        StackList.pop_back();
        StackList.back().childOrdinalRef() += 1;

        return true;
    }

    auto
    ExportTrieIterator::ParseNode(const uint8_t *Ptr,
                                  NodeInfo *InfoOut) noexcept
        -> ExportTrieIterator::Error
    {
        auto NodeSize = uint32_t();
        if ((Ptr = Utils::ReadUleb128(Ptr, End, &NodeSize)) == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        const auto Offset = static_cast<uint32_t>(Ptr - Begin);
        const auto OffsetEndOpt = Utils::AddAndCheckOverflow(Offset, NodeSize);

        if (!OffsetEndOpt.has_value()) {
            return Error::InvalidFormat;
        }

        const auto ExpectedEnd = Ptr + NodeSize;
        if (ExpectedEnd >= End) {
            return Error::InvalidFormat;
        }

        auto &RangeList = Info->rangeListRef();

        const auto Range = ADT::Range::FromEnd(Offset, OffsetEndOpt.value());
        const auto Predicate = [&Range](const ADT::Range &RhsRange) noexcept {
            return Range.overlaps(RhsRange);
        };

        const auto RangeListEnd = RangeList.cend();
        if (std::find_if(RangeList.cbegin(), RangeListEnd, Predicate) !=
            RangeListEnd)
        {
            return Error::OverlappingRanges;
        }

        const auto ChildCount = *ExpectedEnd;
        Info->rangeListRef().emplace_back(Range);

        InfoOut->setOffset(static_cast<uint64_t>(Ptr - Begin));
        InfoOut->setSize(NodeSize);
        InfoOut->setChildCount(ChildCount);

        return Error::None;
    }

    auto
    ExportTrieIterator::ParseNextNode(const uint8_t *& Ptr,
                                      NodeInfo *const InfoOut) noexcept
        -> ExportTrieIterator::Error
    {
        const auto StringPtr = reinterpret_cast<const char *>(Ptr);
        const auto StringLength =
            strnlen(StringPtr, static_cast<uint64_t>(End - Ptr));

        Ptr += (StringLength + 1);

        auto Next = uint64_t();
        if ((Ptr = Utils::ReadUleb128(Ptr, End, &Next)) == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        if (Next >= static_cast<uint64_t>(End - Begin)) {
            return Error::InvalidFormat;
        }

        InfoOut->setPrefix(std::string_view(StringPtr, StringLength));
        return ParseNode(Begin + Next, InfoOut);
    }

    auto ExportTrieIterator::Advance() noexcept -> ExportTrieIterator::Error {
        auto &StackList = Info->stackListRef();
        const auto MaxDepth = Info->getMaxDepth();

        if (!StackList.empty()) {
            if (StackList.size() == MaxDepth) {
                return Error::TooDeep;
            }

            auto &PrevStack = StackList.back();
            const auto &PrevNode = PrevStack.node();

            if (PrevNode.isExport()) {
                Info->exportInfoRef().clearExclusiveInfo();
                Info->setKind(ExportTrieExportKind::None);

                if (PrevNode.childCount() == 0) {
                    if (!MoveUptoParentNode()) {
                        return Error::None;
                    }
                }

                PrevStack.setChildOrdinal(1);
            } else {
                PrevStack.childOrdinalRef() += 1;
                SetupInfoForNewStack();
            }
        } else {
            SetupInfoForNewStack();
        }

        do {
            auto &Stack = StackList.back();
            auto &Node = Stack.node();
            auto &Export = Info->exportInfoRef();
            auto Ptr = this->Begin + Node.offset();

            const auto ExpectedEnd = Ptr + Node.size();
            const auto UpdateOffset = [&]() noexcept {
                Node.setOffset(static_cast<uint64_t>(Ptr - this->Begin));
            };

            if (Stack.childOrdinal() == 0) {
                const auto IsExportInfo = Node.isExport();
                if (IsExportInfo) {
                    if (Info->string().empty()) {
                        return Error::EmptyExport;
                    }

                    auto Flags = uint64_t();
                    Ptr = Utils::ReadUleb128(Ptr, End, &Flags);

                    if (Ptr == nullptr) {
                        return Error::InvalidUleb128;
                    }

                    if (Ptr == End) {
                        return Error::InvalidFormat;
                    }

                    Export.setFlags(Flags);
                    UpdateOffset();

                    if (Export.reexport()) {
                        auto DylibOrdinal = uint32_t();
                        Ptr = Utils::ReadUleb128(Ptr, End, &DylibOrdinal);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Ptr == End) {
                            return Error::InvalidFormat;
                        }

                        Export.setReexportDylibOrdinal(DylibOrdinal);
                        UpdateOffset();

                        if (*Ptr != '\0') {
                            const auto String =
                                reinterpret_cast<const char *>(Ptr);
                            const auto MaxLength =
                                static_cast<uint64_t>(End - Ptr);

                            const auto Length = strnlen(String, MaxLength);
                            if (Length == MaxLength) {
                                return Error::InvalidFormat;
                            }

                            auto ImportName = std::string(String, Length);
                            Export.setReexportImportName(std::move(ImportName));

                            const auto StringSize = Length + 1;

                            Ptr += StringSize;
                            Node.offsetRef() += StringSize;
                        } else {
                            Ptr += 1;
                            Node.offsetRef() += 1;
                        }
                    } else {
                        auto ImageOffset = uint64_t();
                        Ptr = Utils::ReadUleb128(Ptr, End, &ImageOffset);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        Export.setImageOffset(ImageOffset);
                        if (Export.stubAndResolver()) {
                            if (Ptr == End) {
                                return Error::InvalidFormat;
                            }

                            auto ResolverStubAddress = uint64_t();
                            Ptr =
                                Utils::ReadUleb128(Ptr,
                                                   End,
                                                   &ResolverStubAddress);

                            if (Ptr == nullptr) {
                                return Error::InvalidUleb128;
                            }

                            Export.setResolverStubAddress(ResolverStubAddress);
                        }

                        UpdateOffset();
                    }

                    if (Ptr != ExpectedEnd) {
                        return Error::InvalidUleb128;
                    }

                    switch (Export.kind()) {
                        case ExportTrieFlags::Kind::Regular:
                            if (Export.stubAndResolver()) {
                                Info->setIsStubAndResolver();
                            } else if (Export.reexport()) {
                                Info->setIsReexport();
                            } else {
                                Info->setIsRegular();
                            }

                            break;
                        case ExportTrieFlags::Kind::ThreadLocal:
                            Info->setIsThreadLocal();
                            break;
                        case ExportTrieFlags::Kind::Absolute:
                            Info->setIsAbsolute();
                            break;
                    }

                    // Return later, after checking the child-count, so we can
                    // update the stack-list if necessary.
                }

                // We're done with this entire stack if this node has no
                // children. To finish, we move to the parent-stack. If we don't
                // have a parent-stack, we've reached the end of the
                // export-trie.

                if (Node.childCount() == 0) {
                    if (IsExportInfo) {
                        break;
                    }

                    if (MoveUptoParentNode()) {
                        continue;
                    }

                    break;
                }

                // Skip the byte storing the child-count.
                Ptr++;
                Node.offsetRef() += 1;

                if (IsExportInfo) {
                    break;
                }
            } else if (Stack.childOrdinal() == (Node.childCount() + 1)) {
                // We've finished with this node if ChildOrdinal is past
                // ChildCount.

                if (MoveUptoParentNode()) {
                    continue;
                }

                break;
            }

            // Prepare the next-stack before returning.

            const auto Error = ParseNextNode(Ptr, &NextStack->node());
            UpdateOffset();

            if (Error != Error::None) {
                return Error;
            }

            NextStack->setRangeListSize(Info->rangeList().size());

            // We've already returned this node once if our Child-Ordinal is not
            // zero.

            if (Stack.childOrdinal() == 0) {
                break;
            }

            if (StackList.size() == MaxDepth) {
                return Error::TooDeep;
            }

            SetupInfoForNewStack();
        } while (true);

        return Error::None;
    }

    void ExportTrieExportIterator::Advance() noexcept {
        do {
            Iterator++;
            if (Iterator.hasError()) {
                break;
            }

            if (Iterator.isAtEnd()) {
                break;
            }

            if (Iterator.info().node().isExport()) {
                break;
            }
        } while (true);
    }
}