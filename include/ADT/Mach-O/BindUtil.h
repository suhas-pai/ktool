//
//  include/ADT/Mach-O/BindUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <unordered_map>
#include "BindInfo.h"

namespace MachO {
    struct BindActionCollection {
    public:
        struct Info {
        protected:
            BindInfoKind Kind;
            BindWriteKind WriteKind;

            int64_t Addend = 0;
            uint32_t DylibOrdinal = 0;

            const std::string *Symbol;

            uint32_t SegmentIndex = 0;
            uint64_t SegmentOffset = 0;

            uint64_t Address = 0;
            uint64_t OpcodeAddress = 0;
            uint64_t AddrInSeg = 0;

            bool sIsNewSymbolName : 1;
            BindSymbolFlags Flags;
        public:
            [[nodiscard]]
            constexpr BindInfoKind getKind() const noexcept {
                return Kind;
            }

            [[nodiscard]]
            constexpr BindWriteKind getWriteKind() const noexcept {
                return WriteKind;
            }

            [[nodiscard]] constexpr int64_t getAddend() const noexcept {
                return Addend;
            }

            [[nodiscard]]
            constexpr int64_t getDylibOrdinal() const noexcept {
                return DylibOrdinal;
            }

            [[nodiscard]]
            constexpr std::string_view getSymbol() const noexcept {
                return *Symbol;
            }

            [[nodiscard]]
            constexpr const std::string &getSymbolRef() const noexcept {
                return *Symbol;
            }

            [[nodiscard]]
            constexpr uint32_t getSegmentIndex() const noexcept {
                return SegmentIndex;
            }

            [[nodiscard]]
            constexpr uint64_t getSegmentOffset() const noexcept {
                return SegmentOffset;
            }

            [[nodiscard]]
            constexpr uint64_t getAddress() const noexcept {
                return Address;
            }

            [[nodiscard]]
            constexpr uint64_t getOpcodeAddress() const noexcept {
                return OpcodeAddress;
            }

            [[nodiscard]]
            constexpr uint64_t getAddrInSeg() const noexcept {
                return AddrInSeg;
            }

            [[nodiscard]]
            constexpr bool isNewSymbolName() const noexcept {
                return sIsNewSymbolName;
            }

            [[nodiscard]]
            constexpr BindSymbolFlags getFlags() const noexcept {
                return Flags;
            }

            constexpr Info &setKind(const BindInfoKind Value) noexcept {
                this->Kind = Value;
                return *this;
            }

            constexpr Info &setWriteKind(const BindWriteKind Value) noexcept {
                this->WriteKind = Value;
                return *this;
            }

            constexpr Info &setDylibOrdinal(const uint32_t Value) noexcept {
                this->DylibOrdinal = Value;
                return *this;
            }

            constexpr Info &setAddend(const int64_t Value) noexcept {
                this->Addend = Value;
                return *this;
            }

            constexpr Info &setSymbol(const std::string *const Value) noexcept {
                this->Symbol = Value;
                return *this;
            }

            constexpr Info &setSegmentIndex(const uint32_t Value) noexcept {
                this->SegmentIndex = Value;
                return *this;
            }

            constexpr Info &setSegmentOffset(const uint64_t Value) noexcept {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr Info &setAddress(const uint64_t Value) noexcept {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr Info &setOpcodeAddress(const uint64_t Value) noexcept {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr Info &setAddrInSeg(const uint64_t Value) noexcept {
                this->AddrInSeg = Value;
                return *this;
            }

            constexpr
            Info &setIsNewSymbolName(const bool Value = true) noexcept {
                this->sIsNewSymbolName = Value;
                return *this;
            }

            constexpr Info &setFlags(const BindSymbolFlags &Value) noexcept {
                this->Flags = Value;
                return *this;
            }

            [[nodiscard]]
            inline bool operator==(const BindActionInfo &Info) const noexcept {
                if (Info.WriteKind != getWriteKind()) {
                    return false;
                }

                if (Info.Addend != getAddend()) {
                    return false;
                }

                if (Info.DylibOrdinal != getDylibOrdinal()) {
                    return false;
                }

                if (Info.SymbolName != getSymbol()) {
                    return false;
                }

                if (Info.SegmentIndex != getSegmentIndex()) {
                    return false;
                }

                if (Info.SegOffset != getSegmentOffset()) {
                    return false;
                }

                if (Info.AddrInSeg != getAddrInSeg()) {
                    return false;
                }

                if (Info.Flags != getFlags()) {
                    return false;
                }

                return true;
            }
        };

        using ActionListType =
            std::unordered_map<uint64_t, std::unique_ptr<Info>>;

        using SymbolListType =
            std::unordered_map<size_t, std::unique_ptr<std::string>>;

        enum class Error {
            None,
            MultipleBindsForAddress
        };

        using ParseError = BindOpcodeParseError;
    protected:
        SymbolListType SymbolList;
        ActionListType ActionList;
    public:
        BindActionCollection() noexcept = default;

        BindActionCollection &
        Parse(const SegmentInfoCollection &SegmentCollection,
              const BindActionList *BindList,
              const LazyBindActionList *LazyBindList,
              const WeakBindActionList *WeakBindList,
              const LocationRange &Range,
              ParseError *ParseErrorOut,
              Error *ErrorOut) noexcept;

        [[nodiscard]] inline static BindActionCollection
        Open(const SegmentInfoCollection &SegmentCollection,
             const BindActionList *BindList,
             const LazyBindActionList *LazyBindList,
             const WeakBindActionList *WeakBindList,
             const LocationRange &Range,
             ParseError *ParseErrorOut,
             Error *ErrorOut) noexcept
        {
            auto Collection = BindActionCollection();
            Collection.Parse(SegmentCollection,
                             BindList,
                             LazyBindList,
                             WeakBindList,
                             Range,
                             ParseErrorOut,
                             ErrorOut);

            return Collection;
        }

        [[nodiscard]]
        const Info *GetInfoForAddress(uint64_t Address) const noexcept;

        [[nodiscard]]
        const std::string *GetSymbolForAddress(uint64_t Address) const noexcept;
    };
}
