//
//  ADT/Mach-O/SymbolTableUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <string.h>

#include "ADT/Mach-O/LoadCommands.h"
#include "ADT/Mach-O/SymbolTableUtil.h"

#include "Utils/PointerUtils.h"

namespace MachO {
    typedef
        std::unordered_map<uint64_t,
                           std::unique_ptr<SymbolTableEntryCollectionEntryInfo>>
        InfoMap;

    typedef
        std::unordered_map<uint64_t, std::unique_ptr<std::string>> StringMap;

    template <PointerKind Kind>
    using MachOSymbolTableEntryTypeCalculator =
        std::conditional_t<PointerKindIs64Bit(Kind),
                           const SymbolTableEntry64,
                           const SymbolTableEntry32>;

    template <PointerKind Kind>
    [[nodiscard]] static auto
    ParseSymbol(const MachOSymbolTableEntryTypeCalculator<Kind> &Entry,
                const uint64_t Index,
                const char *const StrTab,
                const char *const StrTabEnd,
                InfoMap &InfoMap,
                StringMap &StringMap,
                const enum SymbolTableEntryCollection::KeyKindEnum KeyKind,
                const SymbolTableEntryCollection::ParseOptions &Options,
                const bool IsBigEndian) noexcept
        -> SymbolTableEntryCollection::Error
    {
        if (Entry.Info.isExternal()) {
            if (Options.IgnoreExternal) {
                return SymbolTableParseError::None;
            }
        }

        switch (Entry.Info.getKind()) {
            case SymbolTableEntryInfo::SymbolKind::Undefined:
                if (Options.IgnoreUndefined) {
                    return SymbolTableParseError::None;
                }

                break;
            case SymbolTableEntryInfo::SymbolKind::Absolute:
                if (Options.IgnoreAbsolute) {
                    return SymbolTableParseError::None;
                }

                break;
            case SymbolTableEntryInfo::SymbolKind::Indirect:
                if (Options.IgnoreIndirect) {
                    return SymbolTableParseError::None;
                }

                break;
            case SymbolTableEntryInfo::SymbolKind::PreboundUndefined:
                if (Options.IgnorePreboundUndefined) {
                    return SymbolTableParseError::None;
                }

                break;
            case SymbolTableEntryInfo::SymbolKind::SectionDefined:
                if (Options.IgnoreSectionDefined) {
                    return SymbolTableParseError::None;
                }

                break;
        }

        const auto StrTabRange = Range::CreateWithSize(0, StrTabEnd - StrTab);
        const auto StringIndex = Entry.getIndex(IsBigEndian);

        if (!StrTabRange.hasLocation(StringIndex)) {
            return SymbolTableParseError::InvalidStringOffset;
        }

        const auto StringIter = StringMap.find(StringIndex);
        auto StringPtr = static_cast<std::string *>(nullptr);

        if (StringIter == StringMap.end()) {
            const auto String = StrTab + StringIndex;
            const auto MaxLength =
                static_cast<size_t>((StrTabEnd - StrTab) - StringIndex);

            const auto Length = strnlen(String, MaxLength);
            if (Length == MaxLength) {
                return SymbolTableParseError::NoNullTerminator;
            }

            const auto &Pair = StringMap.insert({
                StringIndex,
                std::make_unique<std::string>(String, Length)
            });

            StringPtr = Pair.first->second.get();
        } else {
            StringPtr = StringIter->second.get();
        }

        auto SymbolEntryInfo = SymbolTableEntryCollection::EntryInfo();
        SymbolEntryInfo.setSymbolInfo(Entry.Info)
            .setString(StringPtr)
            .setSectionOrdinal(Entry.getSectionOrdinal(IsBigEndian))
            .setDescription(
                static_cast<uint16_t>(Entry.getDescription(IsBigEndian)))
            .setIndex(Index)
            .setValue(Entry.getValue(IsBigEndian));

        auto Key = uint64_t();
        switch (KeyKind) {
            case SymbolTableEntryCollection::KeyKindEnum::Index:
                Key = SymbolEntryInfo.getIndex();
                break;
            case SymbolTableEntryCollection::KeyKindEnum::Value:
                Key = SymbolEntryInfo.getValue();
                break;
        }

        auto Ptr =
            std::make_unique<SymbolTableEntryCollection::EntryInfo>(
                std::move(SymbolEntryInfo));

        InfoMap.insert({ Key, std::move(Ptr) });
        return SymbolTableParseError::None;
    }

    template <PointerKind Kind>
    [[nodiscard]] static auto
    ParseList(const uint8_t *Begin,
              const uint64_t Count,
              uint64_t NlistStartIndex,
              const char *StrTab,
              const char *StrTabEnd,
              InfoMap &InfoMap,
              StringMap &StringMap,
              enum SymbolTableEntryCollection::KeyKindEnum KeyKind,
              const SymbolTableEntryCollection::ParseOptions &Options,
              bool IsBigEndian) noexcept
        -> SymbolTableEntryCollection::Error
    {
        using PointerType = MachOSymbolTableEntryTypeCalculator<Kind>;
        auto Index = NlistStartIndex;

        const auto List = BasicContiguousList<PointerType>(Begin, Count);
        const auto ListEnd = List.end();

        for (auto Iter = List.begin(); Iter != ListEnd; Iter++, Index++) {
            const auto Error =
                ParseSymbol<Kind>(*Iter,
                                  Index,
                                  StrTab,
                                  StrTabEnd,
                                  InfoMap,
                                  StringMap,
                                  KeyKind,
                                  Options,
                                  IsBigEndian);

            if (Error != SymbolTableParseError::None) {
                return Error;
            }
        }

        return SymbolTableParseError::None;
    }

    auto
    SymbolTableEntryCollection::Parse(const uint8_t *const NlistBegin,
                                      const uint64_t NlistCount,
                                      const char *const StrTab,
                                      const char *const StrEnd,
                                      const bool IsBigEndian,
                                      const bool Is64Bit,
                                      const enum KeyKindEnum KeyKind,
                                      const ParseOptions Options,
                                      Error *const ErrorOut) noexcept
        -> decltype(*this)
    {
        auto Error = SymbolTableEntryCollection::Error();
        if (Is64Bit) {
            Error =
                ParseList<PointerKind::s64Bit>(NlistBegin,
                                               NlistCount,
                                               0,
                                               StrTab,
                                               StrEnd,
                                               InfoMap,
                                               StringMap,
                                               KeyKind,
                                               Options,
                                               IsBigEndian);
        } else {
            Error =
                ParseList<PointerKind::s32Bit>(NlistBegin,
                                               NlistCount,
                                               0,
                                               StrTab,
                                               StrEnd,
                                               InfoMap,
                                               StringMap,
                                               KeyKind,
                                               Options,
                                               IsBigEndian);
        }

        if (Error != Error::None) {
            *ErrorOut = Error;
        }

        return *this;
    }

    auto
    SymbolTableEntryCollection::Parse(const uint8_t *const Map,
                                      const SymTabCommand &SymTab,
                                      const bool IsBigEndian,
                                      const bool Is64Bit,
                                      const KeyKindEnum KeyKind,
                                      const ParseOptions Options,
                                      Error *const ErrorOut) noexcept
        -> decltype(*this)
    {
        const auto NlistCount = SymTab.getSymbolCount(IsBigEndian);
        const auto NlistBegin = Map + SymTab.getSymbolTableOffset(IsBigEndian);

        const auto StrTab = Map + SymTab.getStringTableOffset(IsBigEndian);
        const auto StrEnd = StrTab + SymTab.getStringTableSize(IsBigEndian);

        Parse(NlistBegin,
              NlistCount,
              reinterpret_cast<const char *>(StrTab),
              reinterpret_cast<const char *>(StrEnd),
              IsBigEndian,
              Is64Bit,
              KeyKind,
              Options,
              ErrorOut);

        return *this;
    }

    auto SymbolTableEntryCollection::ParseIndirectSymbolIndexTable(
        const uint8_t *const NlistBegin,
        const uint64_t NlistCount,
        const uint32_t *const IndexBegin,
        const uint64_t IndexCount,
        const char *const StrTab,
        const char *const StrEnd,
        const bool IsBigEndian,
        const bool Is64Bit,
        const KeyKindEnum KeyKind,
        const ParseOptions Options,
        Error *const ErrorOut) noexcept
            -> decltype(*this)
    {
        const auto IndexEnd = IndexBegin + IndexCount;
        const auto IndexList = BasicContiguousList(IndexBegin, IndexEnd);

        if (Is64Bit) {
            using EntryType =
                 MachOSymbolTableEntryTypeCalculator<PointerKind::s64Bit>;

            const auto EntryList =
                BasicContiguousList<EntryType>(NlistBegin, NlistCount);

            for (const auto &Index : IndexList) {
                if (Index == IndirectSymbolAbsolute ||
                    Index == IndirectSymbolLocal)
                {
                    continue;
                }

                if (IndexOutOfBounds(Index, EntryList.count())) {
                    if (ErrorOut != nullptr) {
                        *ErrorOut =
                            SymbolTableParseError::OutOfBoundsIndirectIndex;
                    }

                    return *this;
                }

                const auto Error =
                    ParseSymbol<PointerKind::s64Bit>(EntryList.at(Index),
                                                     Index,
                                                     StrTab,
                                                     StrEnd,
                                                     InfoMap,
                                                     StringMap,
                                                     KeyKind,
                                                     Options,
                                                     IsBigEndian);

                if (Error != SymbolTableParseError::None) {
                    if (ErrorOut != nullptr) {
                        *ErrorOut = Error;
                    }

                    return *this;
                }
            }
        } else {
            using EntryType =
                 MachOSymbolTableEntryTypeCalculator<PointerKind::s32Bit>;

            const auto EntryList =
                BasicContiguousList<EntryType>(NlistBegin, NlistCount);

            for (const auto &Index : IndexList) {
                if (IndexOutOfBounds(Index, EntryList.count())) {
                    if (ErrorOut != nullptr) {
                        *ErrorOut =
                            SymbolTableParseError::OutOfBoundsIndirectIndex;
                    }

                    return *this;
                }

                const auto Error =
                    ParseSymbol<PointerKind::s32Bit>(EntryList.at(Index),
                                                     Index,
                                                     StrTab,
                                                     StrEnd,
                                                     InfoMap,
                                                     StringMap,
                                                     KeyKind,
                                                     Options,
                                                     IsBigEndian);

                if (Error != SymbolTableParseError::None) {
                    if (ErrorOut != nullptr) {
                        *ErrorOut = Error;
                    }

                    return *this;
                }

            }
        }

        return *this;
    }

    auto
    SymbolTableEntryCollection::ParseIndirectSymbolIndexTable(
        const uint8_t *const Map,
        const SymTabCommand &SymTab,
        const DynamicSymTabCommand &DySymTab,
        const bool IsBigEndian,
        const bool Is64Bit,
        const KeyKindEnum KeyKind,
        const ParseOptions Options,
        Error *const ErrorOut) noexcept
            -> decltype(*this)
    {
        const auto IndexListCount =
            DySymTab.getIndirectSymbolTableCount(IsBigEndian);
        const auto IndexListBegin =
            Map + DySymTab.getIndirectSymbolTableOffset(IsBigEndian);

        const auto NlistCount = SymTab.getSymbolCount(IsBigEndian);
        const auto NlistBegin = Map + SymTab.getSymbolTableOffset(IsBigEndian);

        const auto StrTab = Map + SymTab.getStringTableOffset(IsBigEndian);
        const auto StrEnd = StrTab + SymTab.getStringTableSize(IsBigEndian);

        ParseIndirectSymbolIndexTable(
            NlistBegin,
            NlistCount,
            reinterpret_cast<const uint32_t *>(IndexListBegin),
            IndexListCount,
            reinterpret_cast<const char *>(StrTab),
            reinterpret_cast<const char *>(StrEnd),
            IsBigEndian,
            Is64Bit,
            KeyKind,
            Options,
            ErrorOut);

        return *this;
    }

    auto
    SymbolTableEntryCollection::ParseIndirectSymbolsPtrSection(
        const uint8_t *const Map,
        const SymTabCommand &SymTab,
        const DynamicSymTabCommand &DySymTab,
        const SectionInfo &Sect,
        const bool IsBigEndian,
        const bool Is64Bit,
        const KeyKindEnum KeyKind,
        const ParseOptions Options,
        Error *const ErrorOut) noexcept
            -> decltype(*this)
    {
        const auto IndexListOffset =
            DySymTab.getIndirectSymbolTableOffset(IsBigEndian);
        const auto IndexListCount =
            DySymTab.getIndirectSymbolTableCount(IsBigEndian);

        const auto IndexListMap = Map + IndexListOffset;
        const auto IndexListRange = Range::CreateWithSize(0, IndexListCount);

        const auto SectionSize = Sect.getFileRange().size();
        const auto SectionIndexCount = SectionSize / PointerSize(Is64Bit);
        const auto SectionIndexRange =
            Range::CreateWithSize(Sect.getReserved1(),
                                          SectionIndexCount);

        if (!IndexListRange.contains(SectionIndexRange)) {
            if (ErrorOut != nullptr) {
                *ErrorOut = SymbolTableParseError::InvalidSection;
            }

            return *this;
        }

        const auto NlistBegin = Map + SymTab.getSymbolTableOffset(IsBigEndian);
        const auto NlistCount = SymTab.getSymbolCount(IsBigEndian);

        const auto StrTab = Map + SymTab.getStringTableOffset(IsBigEndian);
        const auto StrEnd = StrTab + SymTab.getStringTableSize(IsBigEndian);
        const auto Index = Sect.getReserved1();

        ParseIndirectSymbolIndexTable(
            NlistBegin,
            NlistCount,
            reinterpret_cast<const uint32_t *>(IndexListMap) + Index,
            SectionIndexCount,
            reinterpret_cast<const char *>(StrTab),
            reinterpret_cast<const char *>(StrEnd),
            IsBigEndian,
            Is64Bit,
            KeyKind,
            Options,
            ErrorOut);

        return *this;
    }

    auto
    SymbolTableEntryCollection::GetInfoWithKey(
        const KeyKindEnum KeyKind,
        const uint64_t Value) const noexcept
            -> SymbolTableEntryCollection::EntryInfo *
    {
        assert(this->KeyKind == KeyKind);

        const auto Iter = InfoMap.find(Value);
        if (Iter != InfoMap.end()) {
            return Iter->second.get();
        }

        return nullptr;
    }
}
