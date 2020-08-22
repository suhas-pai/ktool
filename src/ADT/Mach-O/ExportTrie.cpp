//
//  src/ADT/Mach-O/ExportTrie.cpp
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <algorithm>
#include <cstring>

#include "Utils/Leb128.h"
#include "ExportTrie.h"

namespace MachO {
    ExportTrieIterator::ExportTrieIterator(const uint8_t *Begin,
                                           const uint8_t *End,
                                           const ParseOptions &Options) noexcept
    : Begin(Begin), End(End), MaxDepth(Options.MaxDepth)
    {
        Info = std::make_unique<ExportTrieIterateInfo>();

        Info->getRangeList().reserve(Options.RangeListReserveSize);
        Info->getStackList().reserve(Options.StackListReserveSize);
        Info->getString().reserve(Options.StringReserveSize);

        auto Node = NodeInfo();
        this->ParseError = ParseNode(Begin, &Node);

        if (ParseError.hasValue()) {
            return;
        }

        NextStack = std::make_unique<StackInfo>(Node);
        this->ParseError = Advance();
    }

    ExportTrieExportIterator::ExportTrieExportIterator(
        const uint8_t *Begin,
        const uint8_t *End) noexcept : Iterator(Begin, End)
    {
        Advance();
    }

    void ExportTrieIterator::SetupInfoForNewStack() noexcept {
        Info->getString().append(NextStack->getNode().getPrefix());
        Info->getStackList().emplace_back(std::move(*NextStack));
    }

    bool ExportTrieIterator::MoveUptoParentNode() noexcept {
        auto &StackList = Info->getStackList();
        if (StackList.size() == 1) {
            StackList.clear();
            return false;
        }

        auto &Top = StackList.back();
        const auto EraseToLength =
            (Info->getString().length() - Top.getNode().getPrefix().length());

        Info->getString().erase(EraseToLength);
        if (Top.getRangeListSize() != 0) {
            auto &RangeList = Info->getRangeList();
            RangeList.erase(RangeList.cbegin() + Top.getRangeListSize(),
                            RangeList.cend());
        }

        StackList.pop_back();
        StackList.back().getChildOrdinal() += 1;

        return true;
    }

    ExportTrieIterator::Error
    ExportTrieIterator::ParseNode(const uint8_t *Ptr,
                                  NodeInfo *InfoOut) noexcept
    {
        auto NodeSize = uint32_t();
        if ((Ptr = ReadUleb128(Ptr, End, &NodeSize)) == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        const auto Offset = static_cast<uint32_t>(Ptr - Begin);
        auto OffsetEnd = uint32_t();

        if (DoesAddOverflow(Offset, NodeSize, &OffsetEnd)) {
            return Error::InvalidFormat;
        }

        const auto ExpectedEnd = Ptr + NodeSize;
        if (ExpectedEnd >= End) {
            return Error::InvalidFormat;
        }

        const auto Range = LocationRange::CreateWithEnd(Offset, OffsetEnd);
        auto &RangeList = Info->getRangeList();
        const auto RangeListEnd = RangeList.cend();

        const auto Predicate =
            [&Range](const LocationRange &RhsRange) noexcept
        {
            return Range.overlaps(RhsRange);
        };

        if (std::find_if(RangeList.cbegin(), RangeListEnd, Predicate) !=
            RangeListEnd)
        {
            return Error::OverlappingRanges;
        }

        const auto ChildCount = *ExpectedEnd;
        Info->getRangeList().emplace_back(Range);

        InfoOut->setOffset(Ptr - Begin);
        InfoOut->setSize(NodeSize);
        InfoOut->setChildCount(ChildCount);

        return Error::None;
    }

    ExportTrieIterator::Error
    ExportTrieIterator::ParseNextNode(const uint8_t *& Ptr,
                                      NodeInfo *InfoOut) noexcept
    {
        const auto StringPtr = reinterpret_cast<const char *>(Ptr);
        const auto StringLength = strnlen(StringPtr, End - Ptr);

        Ptr += (StringLength + 1);

        auto Next = uint64_t();
        if ((Ptr = ReadUleb128(Ptr, End, &Next)) == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        if (Next >= (End - Begin)) {
            return Error::InvalidFormat;
        }

        InfoOut->setPrefix(std::string_view(StringPtr, StringLength));
        return ParseNode(Begin + Next, InfoOut);
    }

    ExportTrieIterator::Error ExportTrieIterator::Advance() noexcept {
        using ExportInfoKind = ExportTrieExportInfo::Kind;

        auto &StackList = Info->getStackList();
        if (!StackList.empty()) {
            if (StackList.size() == MaxDepth) {
                return Error::TooDeep;
            }

            auto &PrevStack = StackList.back();
            const auto &PrevNode = PrevStack.getNode();

            if (PrevNode.IsExport()) {
                Info->getExportInfo().clearExclusiveInfo();
                Info->setKind(ExportTrieExportKind::None);

                if (PrevNode.getChildCount() == 0) {
                    if (!MoveUptoParentNode()) {
                        return Error::None;
                    }
                }

                PrevStack.setChildOrdinal(1);
            } else {
                PrevStack.getChildOrdinal() += 1;
                SetupInfoForNewStack();
            }
        } else {
            SetupInfoForNewStack();
        }

        do {
            auto &Stack = StackList.back();
            auto &Node = Stack.getNode();
            auto &Export = Info->getExportInfo();
            auto Ptr = this->Begin + Node.getOffset();

            const auto NodeSize = Node.getSize();
            const auto ExpectedEnd = Ptr + NodeSize;
            const auto UpdateOffset = [&]() noexcept {
                Node.setOffset(Ptr - this->Begin);
            };

            if (Stack.getChildOrdinal() == 0) {
                const auto IsExportInfo = (NodeSize != 0);
                if (IsExportInfo) {
                    if (Info->getString().empty()) {
                        return Error::EmptyExport;
                    }

                    auto Flags = uint64_t();
                    if ((Ptr = ReadUleb128(Ptr, End, &Flags)) == nullptr) {
                        return Error::InvalidUleb128;
                    }

                    if (Ptr == End) {
                        return Error::InvalidFormat;
                    }

                    Export.setFlags(Flags);
                    UpdateOffset();

                    if (Export.IsReexport()) {
                        auto ReexportDylibOrdinal = uint64_t();
                        Ptr = ReadUleb128(Ptr, End, &ReexportDylibOrdinal);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Ptr == End) {
                            return Error::InvalidFormat;
                        }

                        Export.setReexportDylibOrdinal(ReexportDylibOrdinal);
                        UpdateOffset();

                        if (*Ptr != '\0') {
                            const auto String =
                                reinterpret_cast<const char *>(Ptr);

                            const auto MaxLength = (End - Ptr);
                            const auto Length = strnlen(String, MaxLength);

                            if (Length == MaxLength) {
                                return Error::InvalidFormat;
                            }

                            auto ImportName = std::string(String, Length);
                            Export.setReexportImportName(std::move(ImportName));

                            Ptr += (Length + 1);
                        } else {
                            Ptr += 1;
                        }
                    } else {
                        auto ImageOffset = uint64_t();
                        Ptr = ReadUleb128(Ptr, End, &ImageOffset);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        Export.setImageOffset(ImageOffset);
                        if (Export.IsStubAndResolver()) {
                            if (Ptr == End) {
                                return Error::InvalidFormat;
                            }

                            auto ResolverStubAddress = uint64_t();
                            Ptr = ReadUleb128(Ptr, End, &ResolverStubAddress);

                            if (Ptr == nullptr) {
                                return Error::InvalidUleb128;
                            }

                            Export.setResolverStubAddress(ResolverStubAddress);
                        }
                    }

                    UpdateOffset();
                    if (Ptr != ExpectedEnd) {
                        return Error::InvalidUleb128;
                    }

                    switch (Export.getKind()) {
                        case ExportSymbolKind::Regular:
                            if (Export.IsStubAndResolver()) {
                                Info->setKind(ExportInfoKind::StubAndResolver);
                            } else if (Export.IsReexport()) {
                                Info->setKind(ExportInfoKind::Reexport);
                            } else {
                                Info->setKind(ExportInfoKind::Regular);
                            }

                            break;
                        case ExportSymbolKind::ThreadLocal:
                            Info->setKind(ExportInfoKind::ThreadLocal);
                            break;
                        case ExportSymbolKind::Absolute:
                            Info->setKind(ExportInfoKind::Absolute);
                            break;
                    }

                    // Return later, after checking the child-count, so we can
                    // update the stack-list if necessary.
                }

                // We're done with this entire stack if this node has no
                // children. To finish, we simply pop the stack and move to the
                // parent-stack. If we don't have a parent-stack, we've reached
                // the end of the export-trie.

                if (Node.getChildCount() == 0) {
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
                UpdateOffset();

                if (IsExportInfo) {
                    break;
                }
            }

            // We've finished with this node if ChildOrdinal is past ChildCount.
            if (Stack.getChildOrdinal() == (Node.getChildCount() + 1)) {
                if (MoveUptoParentNode()) {
                    continue;
                }

                break;
            }

            // Prepare the next-stack before returning.

            const auto Error = ParseNextNode(Ptr, &NextStack->getNode());
            UpdateOffset();

            if (Error != Error::None) {
                return Error;
            }

            NextStack->setRangeListSize(Info->getRangeList().size());

            // We've already returned this node once if our Child-Ordinal is not
            // zero.

            if (Stack.getChildOrdinal() == 0) {
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

            if (Iterator.IsAtEnd()) {
                break;
            }

            if (Iterator.getInfo().getNode().IsExport()) {
                break;
            }
        } while (true);
    }

    TypedAllocationOrError<ExportTrieList, SizeRangeError>
    GetExportTrieList(const MemoryMap &Map,
                      uint32_t ExportOff,
                      uint32_t ExportSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, ExportOff, ExportSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        return new ExportTrieList(MapBegin + ExportOff, MapBegin + End);
    }

    TypedAllocationOrError<ConstExportTrieList, SizeRangeError>
    GetConstExportTrieList(const ConstMemoryMap &Map,
                           uint32_t ExportOff,
                           uint32_t ExportSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, ExportOff, ExportSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        return new ConstExportTrieList(MapBegin + ExportOff, MapBegin + End);
    }

    TypedAllocationOrError<ExportTrieExportList, SizeRangeError>
    GetExportTrieExportList(const MemoryMap &Map,
                            uint32_t ExportOff,
                            uint32_t ExportSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, ExportOff, ExportSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        return new ExportTrieExportList(MapBegin + ExportOff, MapBegin + End);
    }

    TypedAllocationOrError<ConstExportTrieExportList, SizeRangeError>
    GetConstExportTrieExportList(const ConstMemoryMap &Map,
                                 uint32_t ExportOff,
                                 uint32_t ExportSize) noexcept
    {
        auto End = uint64_t();
        if (const auto Error = CheckSizeRange(Map, ExportOff, ExportSize, &End);
            Error != SizeRangeError::None)
        {
            return Error;
        }

        const auto MapBegin = Map.getBegin();
        const auto Result =
            new ConstExportTrieExportList(MapBegin + ExportOff, MapBegin + End);

        return Result;
    }

    SizeRangeError
    GetExportListFromExportTrie(
        const ConstMemoryMap &Map,
        uint32_t ExportOff,
        uint32_t ExportSize,
        std::vector<ExportTrieIterateInfo> &ExportListOut) noexcept
    {
        // TODO: Get faster impl?
        const auto TrieList =
            GetConstExportTrieExportList(Map, ExportOff, ExportSize);

        if (TrieList.hasError()) {
            return TrieList.getError();
        }

        // We have to create a copy because each export's string is only
        // temporary.

        for (auto Export : TrieList.getRef()) {
            ExportListOut.emplace_back(std::move(Export));
        }

        return SizeRangeError::None;
    }
}
