//
//  src/ADT/Mach-O/ExportTrie.cpp
//  stool
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
                                           const uint8_t *End) noexcept
    : Begin(Begin), End(End)
    {
        Info = std::make_unique<ExportTrieIterateInfo>();

        Info->RangeList.reserve(128);
        Info->StackList.reserve(16);
        Info->String.reserve(64);

        auto Node = NodeInfo();
        auto ParseError = ParseNode(Begin, &Node);

        if (ParseError != Error::None) {
            this->ParseError = ParseError;
            return;
        }

        NextStack = std::make_unique<StackInfo>(Node);
        ParseError = Advance();

        if (ParseError != Error::None) {
            this->ParseError = ParseError;
        }
    }

    ExportTrieExportIterator::ExportTrieExportIterator(
        const uint8_t *Begin,
        const uint8_t *End) noexcept : Iterator(Begin, End)
    {
        Advance();
    }

    void ExportTrieIterator::SetupInfoForNewStack() noexcept {
        Info->String.append(NextStack->Node.Prefix);
        Info->StackList.emplace_back(*NextStack);
    }

    void ExportTrieIterator::MoveUptoParentNode() noexcept {
        auto &Top = Info->getStack();
        Info->String.erase(Top.FullPrefixLength);

        if (Top.RangeListSize != 0) {
            Info->RangeList.erase(Info->RangeList.cbegin() + Top.RangeListSize,
                                  Info->RangeList.cend());
        }

        Info->StackList.pop_back();
        if (!Info->StackList.empty()) {
            Info->StackList.back().ChildOrdinal += 1;
        }
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
        const auto RangeListEnd = Info->RangeList.cend();

        const auto Predicate = [&Range](const LocationRange &RhsRange) {
            return Range.overlaps(RhsRange);
        };

        if (std::find_if(Info->RangeList.cbegin(), RangeListEnd, Predicate) !=
            RangeListEnd)
        {
            return Error::OverlappingRanges;
        }

        const auto ChildCount = *ExpectedEnd;
        Info->RangeList.emplace_back(Range);

        InfoOut->Offset = (Ptr - Begin);
        InfoOut->Size = NodeSize;
        InfoOut->ChildCount = ChildCount;

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

        InfoOut->Prefix = std::string_view(StringPtr, StringLength);
        return ParseNode(Begin + Next, InfoOut);
    }

    ExportTrieIterator::Error ExportTrieIterator::Advance() noexcept {
        using ExportInfoKind = ExportTrieExportInfo::Kind;

        auto &StackList = Info->StackList;
        if (!StackList.empty()) {
            auto &PrevStack = StackList.back();
            const auto &PrevNode = PrevStack.Node;

            if (PrevNode.IsExport()) {
                Info->Export.clearExclusiveInfo();
                if (PrevNode.ChildCount == 0) {
                    MoveUptoParentNode();
                }
            } else {
                SetupInfoForNewStack();
                PrevStack.ChildOrdinal += 1;
            }
        } else {
            SetupInfoForNewStack();
        }

        if (getInfo().getDepthLevel() > 128) {
            return Error::TooDeep;
        }

        do {
            if (StackList.empty()) {
                return Error::None;
            }

            auto &Stack = StackList.back();
            auto &Node = Stack.Node;
            auto &Export = Info->Export;
            auto Ptr = this->Begin + Node.Offset;

            const auto NodeSize = Node.Size;
            const auto ExpectedEnd = Ptr + NodeSize;
            const auto UpdateOffset = [&]() noexcept {
                Node.Offset = (Ptr - this->Begin);
            };

            const auto IsExportInfo = (NodeSize != 0);
            constexpr const auto MarkerValue =
                std::numeric_limits<uint16_t>::max();

            if (Stack.ChildOrdinal == 0) {
                if (IsExportInfo) {
                    if (Info->String.empty()) {
                        return Error::EmptyExport;
                    }

                    auto Flags = uint64_t();
                    if ((Ptr = ReadUleb128(Ptr, End, &Flags)) == nullptr) {
                        return Error::InvalidUleb128;
                    }

                    if (Ptr == End) {
                        return Error::InvalidFormat;
                    }

                    Export.Flags = ExportTrieFlags(Flags);
                    UpdateOffset();

                    if (Export.Flags.IsReexport()) {
                        auto DylibOrdinal = uint32_t();
                        Ptr = ReadUleb128(Ptr, End, &DylibOrdinal);

                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Ptr == End) {
                            return Error::InvalidFormat;
                        }

                        UpdateOffset();
                        Export.ReexportDylibOrdinal = DylibOrdinal;

                        if (*Ptr != '\0') {
                            const auto String =
                                reinterpret_cast<const char *>(Ptr);

                            const auto MaxLength = (End - Ptr);
                            const auto Length = strnlen(String, MaxLength);

                            if (Length == MaxLength) {
                                return Error::InvalidFormat;
                            }

                            Export.ReexportImportName.append(String, Length);
                            Ptr += (Length + 1);
                        } else {
                            Ptr += 1;
                        }
                    } else {
                        Ptr = ReadUleb128(Ptr, End, &Export.ImageOffset);
                        if (Ptr == nullptr) {
                            return Error::InvalidUleb128;
                        }

                        if (Export.Flags.IsStubAndResolver()) {
                            if (Ptr == End) {
                                return Error::InvalidFormat;
                            }

                            auto ResolverStubAddress = uint64_t();
                            Ptr = ReadUleb128(Ptr, End, &ResolverStubAddress);

                            if (Ptr == nullptr) {
                                return Error::InvalidUleb128;
                            }

                            Export.ResolverStubAddress = ResolverStubAddress;
                        }
                    }

                    UpdateOffset();
                    if (Ptr != ExpectedEnd) {
                        return Error::InvalidUleb128;
                    }

                    switch (Export.Flags.getKind()) {
                        case ExportSymbolKind::Regular:
                            if (Export.Flags.IsStubAndResolver()) {
                                Export.sKind = ExportInfoKind::StubAndResolver;
                            } else if (Export.Flags.IsReexport()) {
                                Export.sKind = ExportInfoKind::Reexport;
                            } else {
                                Export.sKind = ExportInfoKind::Regular;
                            }

                            break;
                        case ExportSymbolKind::ThreadLocal:
                            Export.sKind = ExportInfoKind::ThreadLocal;
                            break;
                        case ExportSymbolKind::Absolute:
                            Export.sKind = ExportInfoKind::Absolute;
                            break;
                    }

                    // Return later, after checking the child-count, so we can
                    // update the stack-list if necessary.
                }

                // We're done with this entire stack if this node has no
                // children. To finish, we simply pop the stack and move to the
                // parent-stack. If we don't have a parent-stack, we've reached
                // the end of the export-trie.

                if (Node.ChildCount == 0) {
                    if (IsExportInfo) {
                        break;
                    }

                    if (StackList.empty()) {
                        break;
                    }

                    MoveUptoParentNode();
                    continue;
                }

                // Skip the byte storing the child-count.
                Ptr++;
                UpdateOffset();

                if (IsExportInfo) {
                    Stack.ChildOrdinal = MarkerValue;
                    break;
                }
            } else if (Stack.ChildOrdinal == MarkerValue) {
                Stack.ChildOrdinal = 1;
            }

            // We've finished with this node if ChildOrdinal is past ChildCount.
            if (Stack.ChildOrdinal == (Node.ChildCount + 1)) {
                if (StackList.empty()) {
                    break;
                }

                MoveUptoParentNode();
                continue;
            }

            // Prepare the next-stack before returning.

            const auto Error = ParseNextNode(Ptr, &NextStack->Node);
            UpdateOffset();

            if (Error != Error::None) {
                return Error;
            }

            NextStack->RangeListSize = Info->RangeList.size();
            NextStack->FullPrefixLength = Info->String.length();

            // We've already returned this node once if our Child-Ordinal is not
            // zero.

            if (Stack.ChildOrdinal == 0) {
                break;
            }

            SetupInfoForNewStack();
            if (getInfo().getDepthLevel() > 128) {
                return Error::TooDeep;
            }
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
