/*
 * ADT/Tree.cpp
 * © suhas pai
 */

#include <algorithm>

#include "ADT/Trie.h"
#include "Utils/Leb128.h"

namespace ADT {
    auto
    TrieParser::ParseNode(uint8_t *const Begin,
                          uint8_t *Ptr,
                          uint8_t *const End,
                          std::vector<ADT::Range> &RangeList,
                          TrieNodeInfo *const InfoOut) noexcept -> Error
    {
        auto NodeSizeOpt = Utils::ReadUleb128<uint32_t>(Ptr, End, &Ptr);
        if (!NodeSizeOpt.has_value()) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        const auto NodeSize = NodeSizeOpt.value();
        const auto Offset = static_cast<uint64_t>(Ptr - Begin);
        const auto OffsetEndOpt = Utils::AddAndCheckOverflow(Offset, NodeSize);

        if (!OffsetEndOpt.has_value()) {
            return Error::InvalidFormat;
        }

        const auto ExpectedEnd = Ptr + NodeSize;
        if (ExpectedEnd >= End) {
            return Error::InvalidFormat;
        }

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

        InfoOut->setOffset(static_cast<uint64_t>(Ptr - Begin));
        InfoOut->setSize(NodeSize);
        InfoOut->setChildCount(*ExpectedEnd);

        RangeList.emplace_back(Range);
        return TrieParseError::None;
    }

    auto
    TrieParser::ParseNextNode(uint8_t *const Begin,
                              uint8_t *& Ptr,
                              uint8_t *const End,
                              std::vector<ADT::Range> &RangeList,
                              TrieNodeInfo *const InfoOut) noexcept -> Error
    {
        const auto StringPtr = reinterpret_cast<const char *>(Ptr);
        const auto StringLength =
            strnlen(StringPtr, static_cast<uint64_t>(End - Ptr));

        Ptr += StringLength + 1;

        const auto NextOpt = Utils::ReadUleb128(Ptr, End, &Ptr);
        if (!NextOpt.has_value()) {
            return Error::InvalidUleb128;
        }

        if (Ptr == End) {
            return Error::InvalidFormat;
        }

        const auto Next = NextOpt.value();
        if (Next >= static_cast<uint64_t>(End - Begin)) {
            return Error::InvalidFormat;
        }

        InfoOut->setPrefix(std::string_view(StringPtr, StringLength));
        const auto NewBegin =
            Utils::AddPtrAndCheckOverflow(Begin, Next).value();

        return ParseNode(Begin, NewBegin, End, RangeList, InfoOut);
    }
}