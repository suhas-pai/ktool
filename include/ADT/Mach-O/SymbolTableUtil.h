//
//  include/ADT/Mach-O/SymbolTableUtil.h
//  stool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SegmentInfo.h"

namespace MachO {
    struct SymbolTableEntryCollectionEntryInfo {
        MachO::SymbolTableEntryInfo SymbolInfo;
        std::string *String = nullptr;

        uint8_t Section;
        uint16_t Desc;

        uint64_t Index;
        uint64_t Value;

        [[nodiscard]] inline uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }
    };

    enum class SymbolTableParseError {
        None,
        InvalidStringOffset,
        OutOfBoundsIndirectIndex,
        InvalidSection
    };

    struct SymbolTableEntryCollection {
    public:
        using EntryInfo = SymbolTableEntryCollectionEntryInfo;
        using Error = SymbolTableParseError;

        enum class KeyKindEnum {
            Index,
            Value
        };
    protected:
        std::unordered_map<uint64_t, std::unique_ptr<EntryInfo>> InfoMap;
        std::unordered_map<uint64_t, std::unique_ptr<std::string>> StringMap;

        KeyKindEnum KeyKind;
    public:
        SymbolTableEntryCollection() noexcept = default;

        struct ParseOptions {
            bool IgnoreAbsolute          : 1;
            bool IgnoreExternal          : 1;
            bool IgnoreUndefined         : 1;
            bool IgnoreIndirect          : 1;
            bool IgnorePreboundUndefined : 1;
            bool IgnoreSection           : 1;

            ParseOptions() noexcept :
            IgnoreAbsolute(false), IgnoreExternal(false),
            IgnoreUndefined(false), IgnoreIndirect(false),
            IgnorePreboundUndefined(false), IgnoreSection(false) {}
        };

        SymbolTableEntryCollection &
        Parse(const uint8_t *NlistBegin,
              const uint64_t NlistCount,
              const char *StrTab,
              const char *StrEnd,
              bool Is64Bit,
              bool IsBigEndian,
              KeyKindEnum KeyKind,
              ParseOptions Options,
              Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        Parse(const uint8_t *Map,
              const SymTabCommand &SymTab,
              bool Is64Bit,
              bool IsBigEndian,
              KeyKindEnum KeyKind,
              ParseOptions Options,
              Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolIndexTable(const uint8_t *NlistBegin,
                                      const uint64_t NlistCount,
                                      const uint32_t *IndexBegin,
                                      uint64_t IndexCount,
                                      const char *StrTab,
                                      const char *StrEnd,
                                      bool Is64Bit,
                                      bool IsBigEndian,
                                      KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolIndexTable(const uint8_t *Map,
                                      const SymTabCommand &SymTab,
                                      const DynamicSymTabCommand &DySymTab,
                                      bool Is64Bit,
                                      bool IsBigEndian,
                                      KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolsPtrSection(const uint8_t *NlistBegin,
                                       const uint64_t NlistCount,
                                       const uint32_t *IndexBegin,
                                       uint64_t IndexCount,
                                       uint32_t SectReserved1,
                                       uint64_t SectSize,
                                       const char *StrTab,
                                       const char *StrEnd,
                                       bool Is64Bit,
                                       bool IsBigEndian,
                                       KeyKindEnum KeyKind,
                                       ParseOptions Options,
                                       Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolsPtrSection(const uint8_t *Map,
                                       const SymTabCommand &SymTab,
                                       const DynamicSymTabCommand &DySymTab,
                                       const SectionInfo &Sect,
                                       bool Is64Bit,
                                       bool IsBigEndian,
                                       KeyKindEnum KeyKind,
                                       ParseOptions Options,
                                       Error *ErrorOut) noexcept;

        [[nodiscard]] static inline SymbolTableEntryCollection
        Open(const uint8_t *NlistBegin,
             const uint64_t NlistCount,
             const char *StrTab,
             const char *StrEnd,
             bool Is64Bit,
             bool IsBigEndian,
             KeyKindEnum KeyKind,
             ParseOptions Options,
             Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.Parse(NlistBegin,
                             NlistCount,
                             StrTab,
                             StrEnd,
                             Is64Bit,
                             IsBigEndian,
                             KeyKind,
                             Options,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        Open(const uint8_t *Map,
             const MachO::SymTabCommand &SymTab,
             bool Is64Bit,
             bool IsBigEndian,
             KeyKindEnum KeyKind,
             ParseOptions Options,
             Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.Parse(Map,
                             SymTab,
                             Is64Bit,
                             IsBigEndian,
                             KeyKind,
                             Options,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolIndexTable(
            const uint8_t *Map,
            const MachO::SymTabCommand &SymTab,
            const MachO::DynamicSymTabCommand &DySymTab,
            bool Is64Bit,
            bool IsBigEndian,
            KeyKindEnum KeyKind,
            ParseOptions Options,
            Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolIndexTable(Map,
                                                     SymTab,
                                                     DySymTab,
                                                     Is64Bit,
                                                     IsBigEndian,
                                                     KeyKind,
                                                     Options,
                                                     ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolIndexTable(const uint8_t *NlistBegin,
                                        const uint64_t NlistCount,
                                        const uint32_t *IndexBegin,
                                        uint64_t IndexCount,
                                        const char *StrTab,
                                        const char *StrEnd,
                                        bool Is64Bit,
                                        bool IsBigEndian,
                                        KeyKindEnum KeyKind,
                                        ParseOptions Options,
                                        Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolIndexTable(NlistBegin,
                                                     NlistCount,
                                                     IndexBegin,
                                                     IndexCount,
                                                     StrTab,
                                                     StrEnd,
                                                     Is64Bit,
                                                     IsBigEndian,
                                                     KeyKind,
                                                     Options,
                                                     ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolsPtrSection(const uint8_t *NlistBegin,
                                         const uint64_t NlistCount,
                                         const uint32_t *IndexBegin,
                                         uint64_t IndexCount,
                                         uint32_t SectReserved1,
                                         uint64_t SectSize,
                                         const char *StrTab,
                                         const char *StrEnd,
                                         bool Is64Bit,
                                         bool IsBigEndian,
                                         KeyKindEnum KeyKind,
                                         ParseOptions Options,
                                         Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolsPtrSection(NlistBegin,
                                                      NlistCount,
                                                      IndexBegin,
                                                      IndexCount,
                                                      SectReserved1,
                                                      SectSize,
                                                      StrTab,
                                                      StrEnd,
                                                      Is64Bit,
                                                      IsBigEndian,
                                                      KeyKind,
                                                      Options,
                                                      ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolsPtrSection(const uint8_t *Map,
                                         const SymTabCommand &SymTab,
                                         const DynamicSymTabCommand &DySymTab,
                                         const SectionInfo &Sect,
                                         bool Is64Bit,
                                         bool IsBigEndian,
                                         KeyKindEnum KeyKind,
                                         ParseOptions Options,
                                         Error *ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolsPtrSection(Map,
                                                      SymTab,
                                                      DySymTab,
                                                      Sect,
                                                      Is64Bit,
                                                      IsBigEndian,
                                                      KeyKind,
                                                      Options,
                                                      ErrorOut);

            return Collection;
        }

        [[nodiscard]] EntryInfo *
        GetInfoWithKey(KeyKindEnum KeyKind, uint64_t Value) const noexcept;

        [[nodiscard]] inline uint64_t size() const noexcept {
            return InfoMap.size();
        }

        [[nodiscard]] inline bool empty() const noexcept {
            return InfoMap.empty();
        }

        [[nodiscard]]
        inline decltype(InfoMap)::const_iterator begin() const noexcept {
            return InfoMap.begin();
        }

        [[nodiscard]]
        inline decltype(InfoMap)::const_iterator end() const noexcept {
            return InfoMap.end();
        }

        [[nodiscard]]
        inline std::vector<EntryInfo *> GetAsList() const noexcept {
            auto List = std::vector<EntryInfo *>();
            List.reserve(size());
            
            for (const auto &Info : *this) {
                List.emplace_back(Info.second.get());
            }

            return List;
        }
    };
}
