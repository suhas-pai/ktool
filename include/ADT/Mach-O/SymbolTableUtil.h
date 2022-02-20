//
//  include/ADT/Mach-O/SymbolTableUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/3/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "LoadCommands.h"
#include "SegmentInfo.h"

namespace MachO {
    struct SymbolTableEntryCollectionEntryInfo {
    protected:
        SymbolTableEntryInfo SymbolInfo;
        const std::string *String = nullptr;

        uint8_t SectionOrdinal;
        uint16_t Desc;

        uint64_t Index;
        uint64_t Value;
    public:
        [[nodiscard]]
        inline SymbolTableEntryInfo getSymbolInfo() const noexcept {
            return SymbolInfo;
        }

        [[nodiscard]]
        inline SymbolTableEntryInfo::SymbolKind getSymbolKind() const noexcept {
            return getSymbolInfo().getKind();
        }

        [[nodiscard]] inline bool isUndefined() const noexcept {
            return getSymbolInfo().isUndefined();
        }

        [[nodiscard]] inline bool isAbsolute() const noexcept {
            return getSymbolInfo().isAbsolute();
        }

        [[nodiscard]] inline bool isIndirect() const noexcept {
            return getSymbolInfo().isIndirect();
        }

        [[nodiscard]] inline bool isPreboundUndefined() const noexcept {
            return getSymbolInfo().isPreboundUndefined();
        }

        [[nodiscard]] inline bool isSectionDefined() const noexcept {
            return getSymbolInfo().isSectionDefined();
        }

        [[nodiscard]] inline bool isExternal() const noexcept {
            return getSymbolInfo().isExternal();
        }

        [[nodiscard]] inline bool isPrivateExternal() const noexcept {
            return getSymbolInfo().isPrivateExternal();
        }

        [[nodiscard]] inline bool isDebugSymbol() const noexcept {
            return getSymbolInfo().isDebugSymbol();
        }

        [[nodiscard]] inline const std::string *getString() const noexcept {
            return String;
        }

        [[nodiscard]] inline uint8_t getSectionOrdinal() const noexcept {
            assert(this->isSectionDefined());
            return SectionOrdinal;
        }

        [[nodiscard]] inline uint16_t getDescription() const noexcept {
            return Desc;
        }

        [[nodiscard]] inline uint64_t getIndex() const noexcept {
            return Index;
        }

        [[nodiscard]] inline uint64_t getValue() const noexcept {
            return Value;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setSymbolInfo(const SymbolTableEntryInfo &Value) noexcept {
            this->SymbolInfo = Value;
            return *this;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setString(const std::string *const Value) noexcept {
            this->String = Value;
            return *this;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setSectionOrdinal(const uint8_t Value) noexcept {
            this->SectionOrdinal = Value;
            return *this;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setDescription(const uint16_t Value) noexcept {
            this->Desc = Value;
            return *this;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setIndex(const uint64_t Index) noexcept {
            this->Index = Index;
            return *this;
        }

        inline SymbolTableEntryCollectionEntryInfo &
        setValue(const uint64_t Val) noexcept {
            this->Value = Val;
            return *this;
        }

        [[nodiscard]] inline uint16_t getDylibOrdinal() const noexcept {
            return GetDylibOrdinal(Desc);
        }
    };

    enum class SymbolTableParseError {
        None,
        InvalidStringOffset,
        NoNullTerminator,
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
            bool IgnoreSectionDefined    : 1;

            ParseOptions() noexcept
            : IgnoreAbsolute(false), IgnoreExternal(false),
              IgnoreUndefined(false), IgnoreIndirect(false),
              IgnorePreboundUndefined(false), IgnoreSectionDefined(false) {}
        };

        SymbolTableEntryCollection &
        Parse(const uint8_t *NlistBegin,
              uint64_t NlistCount,
              const char *StrTab,
              const char *StrEnd,
              bool IsBigEndian,
              bool Is64Bit,
              KeyKindEnum KeyKind,
              ParseOptions Options,
              Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        Parse(const uint8_t *Map,
              const SymTabCommand &SymTab,
              bool IsBigEndian,
              bool Is64Bit,
              KeyKindEnum KeyKind,
              ParseOptions Options,
              Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolIndexTable(const uint8_t *NlistBegin,
                                      uint64_t NlistCount,
                                      const uint32_t *IndexBegin,
                                      uint64_t IndexCount,
                                      const char *StrTab,
                                      const char *StrEnd,
                                      bool IsBigEndian,
                                      bool Is64Bit,
                                      KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolIndexTable(const uint8_t *Map,
                                      const SymTabCommand &SymTab,
                                      const DynamicSymTabCommand &DySymTab,
                                      bool IsBigEndian,
                                      bool Is64Bit,
                                      KeyKindEnum KeyKind,
                                      ParseOptions Options,
                                      Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolsPtrSection(const uint8_t *NlistBegin,
                                       uint64_t NlistCount,
                                       const uint32_t *IndexBegin,
                                       uint64_t IndexCount,
                                       uint32_t SectReserved1,
                                       uint64_t SectSize,
                                       const char *StrTab,
                                       const char *StrEnd,
                                       bool IsBigEndian,
                                       bool Is64Bit,
                                       KeyKindEnum KeyKind,
                                       ParseOptions Options,
                                       Error *ErrorOut) noexcept;

        SymbolTableEntryCollection &
        ParseIndirectSymbolsPtrSection(const uint8_t *Map,
                                       const SymTabCommand &SymTab,
                                       const DynamicSymTabCommand &DySymTab,
                                       const SectionInfo &Sect,
                                       bool IsBigEndian,
                                       bool Is64Bit,
                                       KeyKindEnum KeyKind,
                                       ParseOptions Options,
                                       Error *ErrorOut) noexcept;

        [[nodiscard]] static inline SymbolTableEntryCollection
        Open(const uint8_t *const NlistBegin,
             const uint64_t NlistCount,
             const char *const StrTab,
             const char *const StrEnd,
             const bool IsBigEndian,
             const bool Is64Bit,
             const KeyKindEnum KeyKind,
             const ParseOptions Options,
             Error *const ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.Parse(NlistBegin,
                             NlistCount,
                             StrTab,
                             StrEnd,
                             IsBigEndian,
                             Is64Bit,
                             KeyKind,
                             Options,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        Open(const uint8_t *const Map,
             const SymTabCommand &SymTab,
             const bool IsBigEndian,
             const bool Is64Bit,
             const KeyKindEnum KeyKind,
             const ParseOptions Options,
             Error *const ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.Parse(Map,
                             SymTab,
                             IsBigEndian,
                             Is64Bit,
                             KeyKind,
                             Options,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolIndexTable(
            const uint8_t *const Map,
            const SymTabCommand &SymTab,
            const DynamicSymTabCommand &DySymTab,
            const bool IsBigEndian,
            const bool Is64Bit,
            const KeyKindEnum KeyKind,
            const ParseOptions Options,
            Error *const ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolIndexTable(Map,
                                                     SymTab,
                                                     DySymTab,
                                                     IsBigEndian,
                                                     Is64Bit,
                                                     KeyKind,
                                                     Options,
                                                     ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolIndexTable(const uint8_t *const NlistBegin,
                                        const uint64_t NlistCount,
                                        const uint32_t *const IndexBegin,
                                        uint64_t IndexCount,
                                        const char *const StrTab,
                                        const char *const StrEnd,
                                        const bool IsBigEndian,
                                        const bool Is64Bit,
                                        const KeyKindEnum KeyKind,
                                        const ParseOptions Options,
                                        Error *const ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolIndexTable(NlistBegin,
                                                     NlistCount,
                                                     IndexBegin,
                                                     IndexCount,
                                                     StrTab,
                                                     StrEnd,
                                                     IsBigEndian,
                                                     Is64Bit,
                                                     KeyKind,
                                                     Options,
                                                     ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolsPtrSection(const uint8_t *const NlistBegin,
                                         const uint64_t NlistCount,
                                         const uint32_t *const IndexBegin,
                                         const uint64_t IndexCount,
                                         const uint32_t SectReserved1,
                                         const uint64_t SectSize,
                                         const char *const StrTab,
                                         const char *const StrEnd,
                                         const bool IsBigEndian,
                                         const bool Is64Bit,
                                         const KeyKindEnum KeyKind,
                                         const ParseOptions Options,
                                         Error *const ErrorOut) noexcept
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
                                                      IsBigEndian,
                                                      Is64Bit,
                                                      KeyKind,
                                                      Options,
                                                      ErrorOut);

            return Collection;
        }

        [[nodiscard]] static inline SymbolTableEntryCollection
        OpenForIndirectSymbolsPtrSection(const uint8_t *const Map,
                                         const SymTabCommand &SymTab,
                                         const DynamicSymTabCommand &DySymTab,
                                         const SectionInfo &Sect,
                                         const bool IsBigEndian,
                                         const bool Is64Bit,
                                         const KeyKindEnum KeyKind,
                                         const ParseOptions Options,
                                         Error *const ErrorOut) noexcept
        {
            auto Collection = SymbolTableEntryCollection();
            Collection.ParseIndirectSymbolsPtrSection(Map,
                                                      SymTab,
                                                      DySymTab,
                                                      Sect,
                                                      IsBigEndian,
                                                      Is64Bit,
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
