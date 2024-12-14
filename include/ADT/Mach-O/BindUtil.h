//
//  ADT/Mach-O/BindUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/17/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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
            [[nodiscard]] constexpr auto getKind() const noexcept {
                return this->Kind;
            }

            [[nodiscard]] constexpr auto getWriteKind() const noexcept {
                return this->WriteKind;
            }

            [[nodiscard]] constexpr auto getAddend() const noexcept {
                return this->Addend;
            }

            [[nodiscard]] constexpr auto getDylibOrdinal() const noexcept {
                return this->DylibOrdinal;
            }

            [[nodiscard]]
            constexpr auto getSymbol() const noexcept -> std::string_view {
                return *this->Symbol;
            }

            [[nodiscard]] constexpr auto &getSymbolRef() const noexcept {
                return *this->Symbol;
            }

            [[nodiscard]] constexpr auto getSegmentIndex() const noexcept {
                return SegmentIndex;
            }

            [[nodiscard]] constexpr auto getSegmentOffset() const noexcept {
                return SegmentOffset;
            }

            [[nodiscard]] constexpr auto getAddress() const noexcept {
                return Address;
            }

            [[nodiscard]] constexpr auto getOpcodeAddress() const noexcept {
                return OpcodeAddress;
            }

            [[nodiscard]] constexpr auto getAddrInSeg() const noexcept {
                return AddrInSeg;
            }

            [[nodiscard]] constexpr auto isNewSymbolName() const noexcept {
                return this->sIsNewSymbolName;
            }

            [[nodiscard]] constexpr auto getFlags() const noexcept {
                return this->Flags;
            }

            constexpr auto setKind(const BindInfoKind Value) noexcept
                -> decltype(*this)
            {
                this->Kind = Value;
                return *this;
            }

            constexpr auto setWriteKind(const BindWriteKind Value) noexcept
                -> decltype(*this)
            {
                this->WriteKind = Value;
                return *this;
            }

            constexpr auto setDylibOrdinal(const uint32_t Value) noexcept
                -> decltype(*this)
            {
                this->DylibOrdinal = Value;
                return *this;
            }

            constexpr auto setAddend(const int64_t Value) noexcept
                -> decltype(*this)
            {
                this->Addend = Value;
                return *this;
            }

            constexpr auto setSymbol(const std::string *const Value) noexcept
                -> decltype(*this)
            {
                this->Symbol = Value;
                return *this;
            }

            constexpr auto setSegmentIndex(const uint32_t Value) noexcept
                -> decltype(*this)
            {
                this->SegmentIndex = Value;
                return *this;
            }

            constexpr auto setSegmentOffset(const uint64_t Value) noexcept
                -> decltype(*this)
            {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr auto setAddress(const uint64_t Value) noexcept
                -> decltype(*this)
            {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr auto setOpcodeAddress(const uint64_t Value) noexcept
                -> decltype(*this)
            {
                this->SegmentOffset = Value;
                return *this;
            }

            constexpr auto setAddrInSeg(const uint64_t Value) noexcept
                -> decltype(*this)
            {
                this->AddrInSeg = Value;
                return *this;
            }

            constexpr auto setIsNewSymbolName(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->sIsNewSymbolName = Value;
                return *this;
            }

            constexpr auto setFlags(const BindSymbolFlags &Value) noexcept
                -> decltype(*this)
            {
                this->Flags = Value;
                return *this;
            }

            [[nodiscard]]
            inline auto operator==(const BindActionInfo &Info) const noexcept {
                if (Info.WriteKind != this->getWriteKind()) {
                    return false;
                }

                if (Info.Addend != this->getAddend()) {
                    return false;
                }

                if (Info.DylibOrdinal != this->getDylibOrdinal()) {
                    return false;
                }

                if (Info.SymbolName != this->getSymbol()) {
                    return false;
                }

                if (Info.SegmentIndex != this->getSegmentIndex()) {
                    return false;
                }

                if (Info.SegOffset != this->getSegmentOffset()) {
                    return false;
                }

                if (Info.AddrInSeg != this->getAddrInSeg()) {
                    return false;
                }

                if (Info.Flags != this->getFlags()) {
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
        explicit BindActionCollection() noexcept = default;

        auto
        Parse(const SegmentInfoCollection &SegmentCollection,
              const BindActionList *BindList,
              const LazyBindActionList *LazyBindList,
              const WeakBindActionList *WeakBindList,
              const Range &Range,
              ParseError *ParseErrorOut,
              Error *ErrorOut) noexcept
            -> decltype(*this);

        [[nodiscard]] inline static BindActionCollection
        Open(const SegmentInfoCollection &SegmentCollection,
             const BindActionList *BindList,
             const LazyBindActionList *LazyBindList,
             const WeakBindActionList *WeakBindList,
             const Range &Range,
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
