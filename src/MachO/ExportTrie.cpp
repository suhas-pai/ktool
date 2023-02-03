/*
 * MachO/ExportTrie.cpp
 * © suhas pai
 */

#include <algorithm>
#include "MachO/ExportTrie.h"

#include "Utils/Leb128.h"
#include "Utils/Overflow.h"

namespace  MachO {
    ExportTrieIterator::ExportTrieIterator(const uint8_t *const Begin,
                                           const uint8_t *const End,
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
                                  NodeInfo *const InfoOut) noexcept
        -> ExportTrieIterator::Error
    {
        auto NodeSize = Utils::ReadUleb128<uint32_t>(Ptr, End, &Ptr);
        if (Ptr == nullptr) {
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

        const auto Next = Utils::ReadUleb128(Ptr, End, &Ptr);
        if (Ptr == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        if (Next >= static_cast<uint64_t>(End - Begin)) {
            return Error::InvalidFormat;
        }

        InfoOut->setPrefix(std::string_view(StringPtr, StringLength));

        const auto NewBegin =
            Utils::AddPtrAndCheckOverflow(Begin, Next).value();

        return ParseNode(NewBegin, InfoOut);
    }

    auto ExportTrieIterator::Advance() noexcept -> ExportTrieIterator::Error {
        auto &StackList = Info->stackListRef();
        const auto MaxDepth = Info->maxDepth();

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
                switch (Direction) {
                    case Direction::Normal:
                        PrevStack.childOrdinalRef() += 1;
                        SetupInfoForNewStack();

                        break;
                    case Direction::MoveUptoParentNode:
                        if (MoveUptoParentNode()) {}
                        break;
                }
            }

            switch (Direction) {
                case Direction::Normal:
                    break;
                case Direction::MoveUptoParentNode:
                    if (MoveUptoParentNode()) {}
                    return Error::None;
            }
        } else {
            SetupInfoForNewStack();
        }

        this->Direction = Direction::Normal;

        do {
            auto &Stack = StackList.back();
            auto &Node = Stack.node();
            auto &Export = Info->exportInfoRef();

            auto Ptr =
                Utils::AddPtrAndCheckOverflow(this->Begin, Node.offset())
                    .value();

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

                    const auto Flags = Utils::ReadUleb128(Ptr, End, &Ptr);
                    if (Ptr == nullptr) {
                        return Error::InvalidUleb128;
                    }

                    if (Ptr == End) {
                        return Error::InvalidFormat;
                    }

                    Export.setFlags(Flags);
                    UpdateOffset();

                    if (Export.isReexport()) {
                        const auto DylibOrdinal =
                            Utils::ReadUleb128<uint32_t>(Ptr, End, &Ptr);

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
                        const auto ImageOffset =
                            Utils::ReadUleb128(Ptr, End, &Ptr);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        Export.setImageOffset(ImageOffset);
                        if (Export.stubAndResolver()) {
                            if (Ptr == End) {
                                return Error::InvalidFormat;
                            }

                            const auto ResolverStubAddress =
                                Utils::ReadUleb128(Ptr, End, &Ptr);

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
                            } else if (Export.isReexport()) {
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

    const SegmentInfo *
    ExportTrieEntryCollection::LookupInfoForAddress(
        const SegmentList &SegList,
        [[maybe_unused]] const ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
    {
        const auto Segment = SegList.findSegmentWithVmAddr(Address);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(Address);
        }

        return Segment;
    }

    ExportTrieEntryCollection::ChildNode *
    ExportTrieEntryCollection::MakeNodeForEntryInfo(
        const ExportTrieIterateInfo &Info,
        const SegmentList *const SegList) noexcept
    {
        auto Node = static_cast<ChildNode *>(nullptr);
        if (Info.isExport()) {
            Node = new ExportChildNode();
        } else {
            Node = new ChildNode();
        }

        Node->setKind(Info.kind());
        Node->setString(std::string(Info.string()));

        if (Info.isExport()) {
            auto ExportNode = Node->getAsExportNode();
            const auto &ExportInfo = Info.exportInfo();

            if (!ExportInfo.isReexport()) {
                if (SegList != nullptr) {
                    auto Section = static_cast<const SectionInfo *>(nullptr);

                    const auto Addr = ExportInfo.imageOffset();
                    const auto Segment =
                        LookupInfoForAddress(*SegList,
                                             ExportInfo.kind(),
                                             Addr,
                                             &Section);

                    ExportNode->setSegment(Segment);
                    ExportNode->setSection(Section);
                }
            }

            ExportNode->setInfo(ExportInfo);
        }

        return Node;
    }

    void
    ExportTrieEntryCollection::ParseFromTrie(const ExportTrieMap &Trie,
                                             const SegmentList *const SegList,
                                             Error *const Error) noexcept
    {
        auto Iter = Trie.begin();
        const auto End = Trie.end();

        if (Iter == End) {
            return;
        }

        setRoot(MakeNodeForEntryInfo(*Iter, SegList));

        auto Parent = root();
        auto PrevDepthLevel = uint64_t(1);

        const auto MoveUpParentHierarchy = [&](const uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                Parent = Parent->parent();
            }
        };

        for (Iter++; Iter != End; Iter++) {
            if (Iter.hasError()) {
                if (Error != nullptr) {
                    *Error = Iter.getError();
                }

                return;
            }

            const auto DepthLevel = Iter->depthLevel();
            if (PrevDepthLevel > DepthLevel) {
                MoveUpParentHierarchy(
                    static_cast<uint64_t>(PrevDepthLevel) - DepthLevel);
            }

            const auto Current = MakeNodeForEntryInfo(*Iter, SegList);

            Parent->addChild(*Current);
            if (Iter->node().childCount() != 0) {
                Parent = Current;
            }

            PrevDepthLevel = DepthLevel;
        }
    }

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(const ExportTrieMap &Trie,
                                    const SegmentList *const SegList,
                                    Error *const ErrorOut)
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(Trie, SegList, ErrorOut);

        return Result;
    }

    auto
    ExportTrieExportCollection::LookupInfoForAddress(
        const SegmentList &SegList,
        [[maybe_unused]] const ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
        -> const SegmentInfo *
    {
        const auto Segment = SegList.findSegmentWithVmAddr(Address);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(Address);
        }

        return Segment;
    }

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(const ExportTrieMap::ExportMap &Trie,
                                     const SegmentList *const SegList,
                                     Error *const ErrorOut) noexcept
    {
        auto Result = ExportTrieExportCollection();
        Result.Parse(Trie, SegList, ErrorOut);

        return Result;
    }

    void
    ExportTrieExportCollection::Parse(const ExportTrieMap::ExportMap &Trie,
                                      const SegmentList *const SegList,
                                      Error *const ErrorOut) noexcept
    {
        for (auto Iter = Trie.begin(); Iter != Trie.end(); Iter++) {
            if (Iter.hasError()) {
                *ErrorOut = Iter.getError();
                return;
            }

            auto Entry = EntryInfo();
            const auto &ExportInfo = Iter->exportInfo();

            if (!ExportInfo.isReexport()) {
                if (SegList != nullptr) {
                    const auto Addr = ExportInfo.imageOffset();
                    auto Section = static_cast<const SectionInfo *>(nullptr);

                    Entry.setSegment(
                        LookupInfoForAddress(*SegList,
                                             ExportInfo.kind(),
                                             Addr,
                                             &Section));

                    Entry.setSection(Section);
                }
            }

            EntryList.emplace_back(std::make_unique<EntryInfo>(Entry));
        }
    }
}