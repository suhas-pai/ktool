//
//  src/ADT/Mach-O/SymbolTableUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <string.h>
#include "Utils/PointerUtils.h"

#include "LoadCommands.h"
#include "SymbolTableUtil.h"

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
    [[nodiscard]] static SymbolTableParseError
    ParseSymbol(
        const MachOSymbolTableEntryTypeCalculator<Kind> &Entry,
        uint64_t Index,
        const char *StrTab,
        const char *StrTabEnd,
        InfoMap &InfoMap,
        StringMap &StringMap,
        const enum SymbolTableEntryCollection::KeyKindEnum KeyKind,
        const SymbolTableEntryCollection::ParseOptions &Options,
        bool IsBigEndian) noexcept
    {
        if (Entry.Info.IsExternal()) {
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
            case SymbolTableEntryInfo::SymbolKind::Section:
                if (Options.IgnoreSection) {
                    return SymbolTableParseError::None;
                }

                break;
        }

        const auto StrTabRange = RelativeRange(StrTabEnd - StrTab);
        const auto StringIndex = Entry.getIndex(IsBigEndian);

        if (!StrTabRange.containsLocation(StringIndex)) {
            return SymbolTableParseError::InvalidStringOffset;
        }

        const auto StringIter = StringMap.find(StringIndex);
        auto StringPtr = static_cast<std::string *>(nullptr);

        if (StringIter == StringMap.end()) {
            const auto String = StrTab + StringIndex;
            const auto MaxLength = (StrTabEnd - StrTab) - StringIndex;
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

        auto SymbolInfo = SymbolTableEntryCollection::EntryInfo {
            .SymbolInfo = Entry.Info,
            .String = StringPtr,
            .Section = Entry.getSection(IsBigEndian),
            .Desc = static_cast<uint16_t>(Entry.getDesc(IsBigEndian)),
            .Index = Index,
            .Value = Entry.getValue(IsBigEndian)
        };

        auto Ptr =
            std::make_unique<SymbolTableEntryCollection::EntryInfo>(
                std::move(SymbolInfo));

        auto Key = uint64_t();
        switch (KeyKind) {
            case SymbolTableEntryCollection::KeyKindEnum::Index:
                Key = SymbolInfo.Index;
                break;
            case SymbolTableEntryCollection::KeyKindEnum::Value:
                Key = SymbolInfo.Value;
                break;
        }

        InfoMap.insert({ Key, std::move(Ptr) });
        return SymbolTableParseError::None;
    }

    template <PointerKind Kind>
    [[nodiscard]] static SymbolTableEntryCollection::Error
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

    SymbolTableEntryCollection &
    SymbolTableEntryCollection::Parse(const uint8_t *NlistBegin,
                                      const uint64_t NlistCount,
                                      const char *StrTab,
                                      const char *StrEnd,
                                      bool IsBigEndian,
                                      bool Is64Bit,
                                      enum KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept
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

    SymbolTableEntryCollection &
    SymbolTableEntryCollection::Parse(const uint8_t *Map,
                                      const SymTabCommand &SymTab,
                                      bool IsBigEndian,
                                      bool Is64Bit,
                                      KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept
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

    SymbolTableEntryCollection &
    SymbolTableEntryCollection::ParseIndirectSymbolIndexTable(
        const uint8_t *NlistBegin,
        const uint64_t NlistCount,
        const uint32_t *IndexBegin,
        uint64_t IndexCount,
        const char *StrTab,
        const char *StrEnd,
        bool IsBigEndian,
        bool Is64Bit,
        KeyKindEnum KeyKind,
        ParseOptions Options,
        Error *ErrorOut) noexcept
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

    SymbolTableEntryCollection &
    SymbolTableEntryCollection::ParseIndirectSymbolIndexTable(
        const uint8_t *Map,
        const SymTabCommand &SymTab,
        const DynamicSymTabCommand &DySymTab,
        bool IsBigEndian,
        bool Is64Bit,
        KeyKindEnum KeyKind,
        ParseOptions Options,
        Error *ErrorOut) noexcept
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

    SymbolTableEntryCollection &
    SymbolTableEntryCollection::ParseIndirectSymbolsPtrSection(
        const uint8_t *Map,
        const SymTabCommand &SymTab,
        const DynamicSymTabCommand &DySymTab,
        const SectionInfo &Sect,
        bool IsBigEndian,
        bool Is64Bit,
        KeyKindEnum KeyKind,
        ParseOptions Options,
        Error *ErrorOut) noexcept
    {
        const auto IndexListOffset =
            DySymTab.getIndirectSymbolTableOffset(IsBigEndian);
        const auto IndexListCount =
            DySymTab.getIndirectSymbolTableCount(IsBigEndian);

        const auto IndexListMap = Map + IndexListOffset;
        const auto IndexListRange = RelativeRange(IndexListCount);

        const auto SectionSize = Sect.getFileRange().size();
        const auto SectionIndexCount = SectionSize / PointerSize(Is64Bit);
        const auto SectionIndexRange =
            LocationRange::CreateWithSize(Sect.getReserved1(),
                                          SectionIndexCount);

        if (!SectionIndexRange.has_value()) {
            if (ErrorOut != nullptr) {
                *ErrorOut = SymbolTableParseError::InvalidSection;
            }

            return *this;
        }

        if (!IndexListRange.containsRange(SectionIndexRange.value())) {
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

    SymbolTableEntryCollection::EntryInfo *
    SymbolTableEntryCollection::GetInfoWithKey(KeyKindEnum KeyKind,
                                               uint64_t Value) const noexcept
    {
        assert(this->KeyKind == KeyKind);

        const auto Iter = InfoMap.find(Value);
        if (Iter != InfoMap.end()) {
            return Iter->second.get();
        }

        return nullptr;
    }
}
