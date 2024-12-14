//
//  ADT/Mach-O/RebaseInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/19/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once
#include <cstdint>
#include <memory>

#include "ADT/EnumHelper.h"
#include "ADT/ExpectedAlloc.h"

#include "Utils/PointerUtils.h"

#include "LoadCommandsCommon.h"
#include "SegmentUtil.h"

namespace MachO {
    enum class RebaseByteOpcode : uint8_t {
        Done,
        SetKindImm                = 0x10,
        SetSegmentAndOffsetUleb   = 0x20,
        AddAddrUleb               = 0x30,
        AddAddrImmScaled          = 0x40,
        DoRebaseImmTimes          = 0x50,
        DoRebaseUlebTimes         = 0x60,
        DoRebaseAddAddrUleb       = 0x70,
        DoRebaseUlebTimesSkipUleb = 0x80
    };

    [[nodiscard]] constexpr auto
    RebaseByteOpcodeGetName(const RebaseByteOpcode Opcode) noexcept
        -> std::optional<std::string_view>
    {
        switch (Opcode) {
            case RebaseByteOpcode::Done:
                return "REBASE_OPCODE_DONE";
            case RebaseByteOpcode::SetKindImm:
                return "REBASE_OPCODE_SET_TYPE_IMM";
            case RebaseByteOpcode::SetSegmentAndOffsetUleb:
                return "REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB";
            case RebaseByteOpcode::AddAddrImmScaled:
                return "REBASE_OPCODE_ADD_ADDR_ULEB";
            case RebaseByteOpcode::AddAddrUleb:
                return "REBASE_OPCODE_ADD_ADDR_IMM_SCALED";
            case RebaseByteOpcode::DoRebaseAddAddrUleb:
                return "REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB";
            case RebaseByteOpcode::DoRebaseImmTimes:
                return "REBASE_OPCODE_DO_REBASE_IMM_TIMES";
            case RebaseByteOpcode::DoRebaseUlebTimes:
                return "REBASE_OPCODE_DO_REBASE_ULEB_TIMES";
            case RebaseByteOpcode::DoRebaseUlebTimesSkipUleb:
                return "REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIP_ULEB";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    RebaseByteOpcodeGetDesc(const RebaseByteOpcode Opcode) noexcept
        -> std::optional<std::string_view>
    {
        switch (Opcode) {
            case RebaseByteOpcode::Done:
                return "Done";
            case RebaseByteOpcode::SetKindImm:
                return "Set Special Dylib-Ordinal (Immediate)";
            case RebaseByteOpcode::SetSegmentAndOffsetUleb:
                return "Set Segment and Offset (Uleb128)";
            case RebaseByteOpcode::AddAddrImmScaled:
                return "Add Address Scaled (Immediate)";
            case RebaseByteOpcode::AddAddrUleb:
                return "Add Address (Uleb128)";
            case RebaseByteOpcode::DoRebaseAddAddrUleb:
                return "Do Rebase And Add Address (Uleb)";
            case RebaseByteOpcode::DoRebaseImmTimes:
                return "Do Rebase Multiple Times (Immediate) ";
            case RebaseByteOpcode::DoRebaseUlebTimes:
                return "Do Rebase Uleb128 Times (Immediate) ";
            case RebaseByteOpcode::DoRebaseUlebTimesSkipUleb:
                return "Do Rebase Uleb128 times Skipping Uleb128 Bytes";
        }

        return std::nullopt;
    }

    enum class RebaseWriteKind : uint8_t {
        None,
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    [[nodiscard]] constexpr auto
    RebaseWriteKindGetName(const RebaseWriteKind Kind) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = RebaseWriteKind;
        switch (Kind) {
            case Enum::None:
                break;
            case Enum::Pointer:
                return "REBASE_TYPE_POINTER";
            case Enum::TextAbsolute32:
                return "REBASE_TYPE_TEXT_ABSOLUTE32";
            case Enum::TextPcrel32:
                return "REBASE_TYPE_TEXT_PCREL32";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    RebaseWriteKindGetDescription(const RebaseWriteKind Kind) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = RebaseWriteKind;
        switch (Kind) {
            case Enum::None:
                break;
            case Enum::Pointer:
                return "Pointer";
            case Enum::TextAbsolute32:
                return "Text Absolute (32-Bit)";
            case Enum::TextPcrel32:
                return "PC Relative (32-Bit)";
        }

        return std::nullopt;
    }

    [[nodiscard]]
    constexpr uint64_t RebaseWriteKindGetLongestDescription() noexcept {
        const auto Result =
            EnumHelper<RebaseWriteKind>::GetLongestAssocLength(
                [](const RebaseWriteKind Kind) noexcept {
                    return RebaseWriteKindGetDescription(Kind).value_or("");
                });

        return Result;
    }

    enum class RebaseByteDylibSpecialOrdinal : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    struct RebaseByte : private DyldInfoByteMasksAndShiftsHandler {
        constexpr static auto DoneOpcode = RebaseByteOpcode::Done;
    public:
        constexpr RebaseByte() noexcept = default;
        constexpr RebaseByte(uint8_t Byte) noexcept
        : DyldInfoByteMasksAndShiftsHandler(Byte) {}

        using Masks = DyldInfoByteMasks;
        using Opcode = RebaseByteOpcode;
        using Shifts = DyldInfoByteShifts;
        using WriteKind = RebaseWriteKind;

        [[nodiscard]] constexpr auto getOpcode() const noexcept {
            return Opcode(this->getValueForMaskNoShift(Masks::Opcode));
        }

        [[nodiscard]] constexpr auto getImmediate() const noexcept {
            return this->getValueForMaskNoShift(Masks::Immediate);
        }

        constexpr auto SetOpcode(Opcode Opcode) noexcept -> decltype(*this) {
            this->setValueForMaskNoShift(Masks::Opcode,
                                         static_cast<IntegerType>(Opcode));
            return *this;
        }

        constexpr auto SetImmediate(const uint8_t Immediate) noexcept
            -> decltype(*this)
        {
            this->setValueForMaskNoShift(Masks::Immediate, Immediate);
            return *this;
        }
    };

    static_assert(sizeof(RebaseByte) == sizeof(uint8_t),
                  "RebaseByte is not Byte-Sized");

    struct RebaseNakedOpcodeList : public NakedOpcodeList {
        using NakedOpcodeList::NakedOpcodeList;
        struct Iterator : public NakedOpcodeList::Iterator {
            explicit Iterator(const uint8_t *Iter, const uint8_t *End) noexcept
            : NakedOpcodeList::Iterator(Iter, End) {}

            [[nodiscard]] inline auto getByte() const noexcept {
                return *reinterpret_cast<const RebaseByte *>(this->getPtr());
            }
        };

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End);
        }
    };

    using ConstRebaseNakedOpcodeList = RebaseNakedOpcodeList;

    struct RebaseActionInfo {
        RebaseWriteKind Kind;

        uint32_t SegmentIndex;
        uint64_t SegOffset;
        uint64_t AddrInSeg;

        bool HasSegmentIndex : 1;
        bool UseThreadedRebaseRebase : 1;
    };

    enum class RebaseOpcodeParseError {
        None,
        InvalidLeb128,

        IllegalRebaseOpcode,
        OutOfBoundsSegmentAddr,

        UnrecognizedRebaseWriteKind,
        UnrecognizedRebaseOpcode,

        NoSegmentIndex,
        NoWriteKind,
    };

    struct RebaseOpcodeIterateInfo {
        RebaseOpcodeParseError Error = RebaseOpcodeParseError::None;

        uint32_t SegmentIndex = 0;
        uint64_t SegOffset = 0;

        bool HasSegmentIndex : 1;

        union {
            int64_t AddAddr = 0;
            uint64_t Count;
        };

        union {
            uint64_t Skip = 0;
            uint64_t Scale;
        };

        RebaseWriteKind Kind;

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return Error != RebaseOpcodeParseError::None;
        }

        [[nodiscard]] constexpr auto getError() const noexcept {
            return Error;
        }

        [[nodiscard]] constexpr auto GetSegmentIndex() const noexcept
            -> int64_t
        {
            if (HasSegmentIndex) {
                return SegmentIndex;
            }

            return -1;
        }
    };

    struct RebaseOpcodeIteratorEnd {};
    struct RebaseOpcodeIterator {
    public:
        using ErrorEnum = RebaseOpcodeParseError;
    protected:
        typename NakedOpcodeList::Iterator Iter;
        std::unique_ptr<RebaseOpcodeIterateInfo> Info;

        const RebaseByte *Prev;
        bool ReachedEnd : 1 = false;
    public:
        explicit
        RebaseOpcodeIterator(const RebaseByte *const Begin,
                             const RebaseByte *const End) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End))
        {
            Info = std::make_unique<RebaseOpcodeIterateInfo>();
            Advance();
        }

        explicit
        RebaseOpcodeIterator(
            const RebaseByte *const Begin,
            const RebaseByte *const End,
            std::unique_ptr<RebaseOpcodeIterateInfo> &&Info) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          Info(std::move(Info))
        {
            Advance();
        }

        RebaseOpcodeIterator(const RebaseOpcodeIterator &) = delete;

        [[nodiscard]] constexpr bool isAtEnd() const noexcept {
            return ReachedEnd;
        }

        [[nodiscard]]
        constexpr auto operator*() const noexcept -> decltype(*this) {
            return *this;
        }

        [[nodiscard]] constexpr auto operator->() const noexcept {
            return this;
        }

        [[nodiscard]] constexpr auto &getInfo() noexcept {
            return *this->Info;
        }

        [[nodiscard]] constexpr auto &getInfo() const noexcept {
            return *this->Info;
        }

        [[nodiscard]] constexpr auto &getByte() const noexcept {
            return *this->Prev;
        }

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return Info->hasError();
        }

        [[nodiscard]] constexpr auto getError() const noexcept {
            return Info->getError();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            Info->Error = Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return ++(*this);
        }

        constexpr auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] constexpr
        auto operator==(const RebaseOpcodeIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr
        auto operator!=(const RebaseOpcodeIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        inline auto Advance() noexcept -> ErrorEnum {
            using ErrorEnum = RebaseOpcodeParseError;

            const auto BytePtr = this->Iter->getAsByte<RebaseByte>();
            const auto &Byte = *BytePtr;

            Info->Error = ErrorEnum::None;
            Prev = BytePtr;
            this->Iter++;

            if (this->Iter.isAtEnd()) {
                ReachedEnd = true;
                return ErrorEnum::None;
            }

            switch (Byte.getOpcode()) {
                case RebaseByte::Opcode::Done:
                    ReachedEnd = true;
                    return ErrorEnum::None;
                case RebaseByte::Opcode::SetKindImm:
                    Info->Kind = RebaseByte::WriteKind(Byte.getImmediate());
                    switch (Info->Kind) {
                        case RebaseByte::WriteKind::None:
                            break;
                        case RebaseByte::WriteKind::Pointer:
                        case RebaseByte::WriteKind::TextAbsolute32:
                        case RebaseByte::WriteKind::TextPcrel32:
                            return ErrorEnum::None;
                    }

                    return ErrorEnum::UnrecognizedRebaseWriteKind;
                case RebaseByte::Opcode::SetSegmentAndOffsetUleb:
                    if (const auto Uleb = this->Iter->ReadUleb128()) {
                        Info->SegOffset = Uleb.value();
                        Info->SegmentIndex = Byte.getImmediate();
                        Info->HasSegmentIndex = true;

                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::AddAddrImmScaled:
                    Info->Scale = Byte.getImmediate();
                    return ErrorEnum::None;
                case RebaseByte::Opcode::AddAddrUleb:
                    if (const auto Sleb = this->Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    if (const auto Sleb = this->Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseImmTimes:
                    Info->Count = Byte.getImmediate();
                    return ErrorEnum::None;
                case RebaseByte::Opcode::DoRebaseUlebTimes:
                    if (const auto Uleb = this->Iter->ReadUleb128()) {
                        Info->Count = Uleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb:
                    if (const auto Count = this->Iter->ReadUleb128()) {
                        if (const auto Skip = this->Iter->ReadSleb128()) {
                            const auto SkipValue = Skip.value();
                            const auto CountValue = Count.value();

                            Info->Skip = SkipValue;
                            Info->Count = CountValue;

                            return ErrorEnum::None;
                        }
                    }

                    return ErrorEnum::InvalidLeb128;
            }

            return ErrorEnum::UnrecognizedRebaseOpcode;
        }
    };

    struct RebaseOpcodeList {
        using IteratorType = RebaseOpcodeIterator;
    protected:
        const RebaseByte *Begin;
        const RebaseByte *End;
        bool Is64Bit : 1;
    public:
        explicit
        RebaseOpcodeList(const uint8_t *const Begin,
                         const uint8_t *const End,
                         bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const RebaseByte *>(Begin)),
          End(reinterpret_cast<const RebaseByte *>(End)),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] inline auto begin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return RebaseOpcodeIteratorEnd();
        }
    };

    struct RebaseActionIterateInfo : public RebaseOpcodeIterateInfo {
        using ErrorEnum = RebaseOpcodeParseError;

        [[nodiscard]] constexpr auto GetAction() const noexcept {
            const auto Result = RebaseActionInfo{
                .Kind = this->Kind,
                .SegmentIndex = static_cast<uint32_t>(this->SegmentIndex),
                .SegOffset = this->SegOffset,
                .AddrInSeg = this->AddrInSeg,
                .HasSegmentIndex = this->HasSegmentIndex,
                .UseThreadedRebaseRebase = this->UseThreadedRebaseRebase
            };

            return Result;
        };

        uint64_t AddrInSeg;

        bool NewSymbolName : 1;
        bool UseThreadedRebaseRebase : 1;

        [[nodiscard]]
        constexpr static auto CanIgnoreError(const ErrorEnum Error) noexcept {
            switch (Error) {
                case ErrorEnum::None:
                case ErrorEnum::OutOfBoundsSegmentAddr:
                case ErrorEnum::UnrecognizedRebaseWriteKind:
                case ErrorEnum::NoSegmentIndex:
                case ErrorEnum::NoWriteKind:
                    return true;
                case ErrorEnum::InvalidLeb128:
                case ErrorEnum::IllegalRebaseOpcode:
                case ErrorEnum::UnrecognizedRebaseOpcode:
                    return false;
            }

            assert(0 && "Unrecognized Rebase-Opcode Parse Error");
        }
    };

    struct RebaseActionIteratorEnd {};
    struct RebaseActionIterator {
    public:
        using ErrorEnum = RebaseOpcodeParseError;
    protected:
        RebaseOpcodeIterator Iter;
        RebaseByte LastByte;

        int64_t SegAddAddress = 0;
        uint64_t AddAmt = 0;
        uint64_t Count = 0;

        bool Is64Bit : 1;
    public:
        explicit
        RebaseActionIterator(const RebaseByte *const Begin,
                             const RebaseByte *const End,
                             const bool Is64Bit) noexcept
        : Iter(Begin, End, std::make_unique<RebaseActionIterateInfo>()),
          Is64Bit(Is64Bit)
        {
            LastByte.SetOpcode(RebaseByte::Opcode::SetKindImm);
            ++(*this);
        }

        [[nodiscard]] inline auto &getInfo() noexcept {
            auto &Info = Iter.getInfo();
            return reinterpret_cast<RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]] inline auto &getInfo() const noexcept {
            const auto &Info = Iter.getInfo();
            return reinterpret_cast<const RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]] auto
        IsValidForSegmentCollection(const SegmentInfoCollection &Collection,
                                    const bool Is64Bit) noexcept
        {
            const auto &Info = this->getInfo();
            const auto SegmentIndex = Info.SegmentIndex;

            if (Info.HasSegmentIndex) {
                return false;
            }

            if (IndexOutOfBounds(SegmentIndex, Collection.size())) {
                return false;
            }

            const auto &Segment = Collection.at(SegmentIndex);
            auto ContainsPtr = false;

            if (Is64Bit) {
                using Type = PointerAddrTypeFromKind<PointerKind::s64Bit>;
                ContainsPtr =
                    Segment.getFileRange().containsTypeAtRelativeLocation<Type>(
                        Info.AddrInSeg);
            } else {
                using Type = PointerAddrTypeFromKind<PointerKind::s32Bit>;
                ContainsPtr =
                    Segment.getFileRange().containsTypeAtRelativeLocation<Type>(
                        Info.AddrInSeg);
            }

            return ContainsPtr;
        }

        [[nodiscard]] inline auto isAtEnd() const noexcept {
            return this->Iter.isAtEnd();
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return this->getInfo();
        }

        [[nodiscard]] inline auto operator->() const noexcept {
            return &this->getInfo();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            auto &Info = this->getInfo();
            if (Info.hasError()) {
                Iter++;
            }

            Info.Error = Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return ++(*this);
        }

        inline auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]]
        inline auto operator==(const RebaseActionIteratorEnd &) const noexcept {
            return this->isAtEnd();
        }

        [[nodiscard]] inline
        auto operator!=(const RebaseActionIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        inline auto Advance() noexcept -> ErrorEnum{
            auto &Info = this->getInfo();
            const auto AddChangeToSegmentAddress = [&](int64_t Add) {
                if (DoesAddOverflow(SegAddAddress, Add, &SegAddAddress)) {
                    return false;
                }

                if (DoesAddOverflow(Info.AddrInSeg, SegAddAddress)) {
                    return false;
                }

                return true;
            };

            using ErrorEnum = RebaseOpcodeParseError;

            Info.Error = ErrorEnum::None;
            Info.NewSymbolName = false;

            const auto FinalizeChangesForSegmentAddress = [&]() noexcept {
                Info.AddrInSeg += SegAddAddress;
                SegAddAddress = 0;
            };

            switch (LastByte.getOpcode()) {
                case RebaseByte::Opcode::DoRebaseImmTimes:
                case RebaseByte::Opcode::DoRebaseUlebTimes:
                case RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb:
                    if (Count != 0) {
                        Info.AddrInSeg += AddAmt;
                        Count--;

                        if (Count != 0) {
                            return ErrorEnum::None;
                        }

                        Info.AddrInSeg += AddAmt;
                        this->LastByte.SetOpcode(
                            RebaseByte::Opcode::SetKindImm);

                        this->Iter++;
                        break;
                    }

                case RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    // Clear the Last-Opcode.
                    FinalizeChangesForSegmentAddress();

                    this->LastByte.SetOpcode(RebaseByte::Opcode::SetKindImm);
                    this->Iter++;

                    break;

                case RebaseByte::Opcode::Done:
                    return ErrorEnum::None;

                case RebaseByte::Opcode::SetKindImm:
                case RebaseByte::Opcode::SetSegmentAndOffsetUleb:
                case RebaseByte::Opcode::AddAddrImmScaled:
                case RebaseByte::Opcode::AddAddrUleb:
                    break;
            }

            const auto CheckChangeToSegmentAddress = [&](int64_t Add) noexcept {
                auto NewSegAddAddress = decltype(SegAddAddress)();
                if (DoesAddOverflow(SegAddAddress, Add, &NewSegAddAddress)) {
                    return false;
                }

                if (DoesAddOverflow(Info.AddrInSeg, NewSegAddAddress)) {
                    return false;
                }

                return true;
            };

            const auto CheckIfCanRebase = [&]() noexcept {
                if (!Info.HasSegmentIndex) {
                    return ErrorEnum::NoSegmentIndex;
                }

                if (Info.Kind == RebaseWriteKind::None) {
                    return ErrorEnum::NoWriteKind;
                }

                return ErrorEnum::None;
            };

            for (; !this->Iter.isAtEnd(); this->Iter++) {
                if (this->Iter->hasError()) {
                    return Iter->getError();
                }

                const auto &Byte = this->Iter.getByte();
                const auto &IterInfo = this->Iter.getInfo();

                switch (Byte.getOpcode()) {
                    case RebaseByte::Opcode::Done:
                    case RebaseByte::Opcode::SetKindImm:
                        continue;
                    case RebaseByte::Opcode::SetSegmentAndOffsetUleb:
                        Info.AddrInSeg = IterInfo.SegOffset;
                        continue;
                    case RebaseByte::Opcode::AddAddrImmScaled:
                        if (DoesAddOverflow(Info.AddrInSeg,
                                            Info.Scale * PointerSize(Is64Bit),
                                            &Info.AddrInSeg))
                        {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        continue;
                    case RebaseByte::Opcode::AddAddrUleb:
                        if (DoesAddOverflow(Info.AddrInSeg,
                                            IterInfo.AddAddr,
                                            &Info.AddrInSeg))
                        {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        continue;
                    case RebaseByte::Opcode::DoRebaseAddAddrUleb: {
                        const auto Error = CheckIfCanRebase();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        if (!AddChangeToSegmentAddress(IterInfo.AddAddr)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto PtrSize = PointerSize(Is64Bit);
                        if (AddChangeToSegmentAddress(PtrSize)) {
                            this->LastByte = Byte;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::None;
                    }
                    case RebaseByte::Opcode::DoRebaseImmTimes:
                    case RebaseByte::Opcode::DoRebaseUlebTimes: {
                        const auto Error = CheckIfCanRebase();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        auto Total = uint64_t();

                        const auto PtrSize = PointerSize(Is64Bit);
                        const auto Count = Info.Count;

                        if (DoesMultiplyOverflow(PtrSize, Count, &Total)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (CheckChangeToSegmentAddress(Total)) {
                            this->LastByte = Byte;
                            this->AddAmt = PtrSize;

                            this->Count = Count - 1;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                    case RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb: {
                        const auto Error = CheckIfCanRebase();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        auto SingleStep = int64_t();
                        auto Total = int64_t();

                        const auto PtrSize = PointerSize(Is64Bit);
                        if (DoesAddOverflow(PtrSize, Info.Skip, &SingleStep)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto Count = Info.Count;
                        if (DoesMultiplyOverflow(SingleStep, Count, &Total)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (CheckChangeToSegmentAddress(Total)) {
                            this->LastByte = Byte;
                            AddAmt = SingleStep;

                            this->Count = Count - 1;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                }

                return ErrorEnum::UnrecognizedRebaseOpcode;
            }

            this->LastByte.SetOpcode(RebaseByte::Opcode::Done);
            return ErrorEnum::None;
        }
    };

    using ConstRebaseActionIterator = RebaseActionIterator;
    struct RebaseActionList {
        using IteratorType = RebaseActionIterator;
    protected:
        const RebaseByte *Begin;
        const RebaseByte *End;
        bool Is64Bit : 1;
    public:
        explicit
        RebaseActionList(const uint8_t *const Begin,
                         const uint8_t *const End,
                         bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const RebaseByte *>(Begin)),
          End(reinterpret_cast<const RebaseByte *>(End)),
          Is64Bit(Is64Bit) {}

        explicit
        RebaseActionList(const RebaseByte *const Begin,
                         const RebaseByte *const End,
                         const bool Is64Bit) noexcept
        : Begin(Begin), End(End), Is64Bit(Is64Bit) {}

        [[nodiscard]] inline auto begin() const noexcept {
            return IteratorType(Begin, End, Is64Bit);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return RebaseActionIteratorEnd();
        }

        inline auto
        GetAsList(std::vector<RebaseActionInfo> &ListOut) const noexcept
            -> RebaseOpcodeParseError
        {
            for (const auto &Iter : *this) {
                const auto Error = Iter.getError();
                if (!Iter.CanIgnoreError(Error)) {
                    return Error;
                }

                ListOut.emplace_back(Iter.GetAction());
            }

            return RebaseOpcodeParseError::None;
        }

        inline RebaseOpcodeParseError
        GetListOfSymbols(std::vector<RebaseActionInfo> &SymbolListOut)
            const noexcept
        {
            for (const auto &Iter : *this) {
                const auto Error = Iter.getError();
                if (!Iter.CanIgnoreError(Error)) {
                    return Error;
                }

                if (Iter.NewSymbolName) {
                    SymbolListOut.emplace_back(Iter.GetAction());
                }
            }

            return RebaseOpcodeParseError::None;
        }
    };

    using ConstRebaseActionList = RebaseActionList;

    ExpectedAlloc<RebaseNakedOpcodeList, SizeRangeError>
    GetRebaseNakedOpcodeList(const MemoryMap &Map,
                             uint32_t RebaseOffset,
                             uint32_t RebaseSize) noexcept;

    ExpectedAlloc<ConstRebaseNakedOpcodeList, SizeRangeError>
    GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                  uint32_t RebaseOffset,
                                  uint32_t RebaseSize) noexcept;

    ExpectedAlloc<RebaseOpcodeList, SizeRangeError>
    GetRebaseOpcodeList(const ConstMemoryMap &Map,
                        uint32_t RebaseOffset,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept;

    ExpectedAlloc<RebaseActionList, SizeRangeError>
    GetRebaseActionList(const ConstMemoryMap &Map,
                        uint32_t RebaseOffset,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept;
}
