/*
 * MachO/RebaseInfo.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <memory>
#include <string_view>

#include "ADT/FlagsBase.h"
#include "ADT/MemoryMap.h"

#include "MachO/DyldInfo.h"
#include "MachO/OpcodeList.h"
#include "MachO/SegmentList.h"

#include "Utils/Overflow.h"

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
    RebaseByteOpcodeIsValid(const RebaseByteOpcode Opcode) noexcept {
        switch (Opcode) {
            case RebaseByteOpcode::Done:
            case RebaseByteOpcode::SetKindImm:
            case RebaseByteOpcode::SetSegmentAndOffsetUleb:
            case RebaseByteOpcode::AddAddrUleb:
            case RebaseByteOpcode::AddAddrImmScaled:
            case RebaseByteOpcode::DoRebaseImmTimes:
            case RebaseByteOpcode::DoRebaseUlebTimes:
            case RebaseByteOpcode::DoRebaseAddAddrUleb:
            case RebaseByteOpcode::DoRebaseUlebTimesSkipUleb:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto
    RebaseByteOpcodeGetName(const RebaseByteOpcode Opcode) noexcept
        -> std::string_view
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

        assert(false &&
               "MachO::RebaseByteOpcodeGetString() got invalid "
               "RebaseByteOpcode");
    }

    [[nodiscard]] constexpr auto
    RebaseByteOpcodeGetDesc(const RebaseByteOpcode Opcode) noexcept
        -> std::string_view
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

        assert(false &&
               "MachO::RebaseByteOpcodeGetDesc() got invalid RebaseByteOpcode");
    }

    enum class RebaseWriteKind : uint8_t {
        None,
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    [[nodiscard]] constexpr auto
    RebaseWriteKindIsValid(const RebaseWriteKind Kind) noexcept {
        switch (Kind) {
            case RebaseWriteKind::None:
            case RebaseWriteKind::Pointer:
            case RebaseWriteKind::TextAbsolute32:
            case RebaseWriteKind::TextPcrel32:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto
    RebaseWriteKindGetString(const RebaseWriteKind Kind) noexcept
        -> std::string_view
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

        assert(false &&
               "MachO::RebaseWriteKindGetString() got invalid RebaseWritKind");
    }

    [[nodiscard]] constexpr auto
    RebaseWriteKindGetDesc(const RebaseWriteKind Kind) noexcept
        -> std::string_view
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

        assert(false &&
               "MachO::RebaseWriteKindGetDesc() got invalid RebaseWritKind");
    }

    enum class RebaseByteDylibSpecialOrdinal : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    struct RebaseByte : private ADT::FlagsBase<uint8_t> {
        constexpr static auto DoneOpcode = RebaseByteOpcode::Done;
    public:
        using ADT::FlagsBase<uint8_t>::FlagsBase;

        using Masks = DyldInfoByteMasks;
        using Opcode = RebaseByteOpcode;
        using Shifts = DyldInfoByteShifts;
        using WriteKind = RebaseWriteKind;

        [[nodiscard]] constexpr auto opcode() const noexcept {
            return Opcode(valueForMask(Masks::Opcode));
        }

        [[nodiscard]] constexpr auto immediate() const noexcept {
            return uint8_t(valueForMask(Masks::Immediate));
        }

        constexpr auto SetOpcode(const Opcode Opcode) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Opcode, 0, static_cast<uint8_t>(Opcode));
            return *this;
        }

        constexpr auto setImmediate(const uint8_t Value) noexcept
            -> decltype(*this)
        {
            assert(Value <= 0xF);
            setValueForMask(Masks::Immediate, 0, Value);

            return *this;
        }
    };

    static_assert(sizeof(RebaseByte) == sizeof(uint8_t),
                  "MachO::RebaseByte is not 1 byte large");

    struct RebaseActionInfo {
        RebaseWriteKind Kind;

        uint32_t SegmentIndex;
        uint64_t SegOffset;
        uint64_t AddrInSeg;

        bool HasSegmentIndex : 1;
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
            int64_t Skip = 0;
            uint64_t Scale;
        };

        RebaseWriteKind Kind;

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return Error != RebaseOpcodeParseError::None;
        }

        [[nodiscard]] constexpr auto error() const noexcept {
            return Error;
        }

        [[nodiscard]]
        constexpr auto getSegmentIndex() const noexcept -> int64_t {
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
        typename OpcodeList::Iterator Iter;
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

        [[nodiscard]] constexpr auto isAtEnd() const noexcept {
            return ReachedEnd;
        }

        [[nodiscard]]
        constexpr auto operator*() const noexcept -> decltype(*this) {
            return *this;
        }

        [[nodiscard]] constexpr auto operator->() const noexcept {
            return this;
        }

        [[nodiscard]] inline auto &info() noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto &info() const noexcept {
            return *Info;
        }

        [[nodiscard]] constexpr auto byte() const noexcept {
            return *Prev;
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return Info->hasError();
        }

        [[nodiscard]] inline auto error() const noexcept {
            return Info->error();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            Info->Error = Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return operator++();
        }

        constexpr auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                operator++();
            }

            return *this;
        }

        [[nodiscard]] constexpr
        auto operator==(const RebaseOpcodeIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr
        auto operator!=(const RebaseOpcodeIteratorEnd &End) const noexcept {
            return !operator==(End);
        }

        inline RebaseOpcodeParseError Advance() noexcept {
            using ErrorEnum = RebaseOpcodeParseError;

            const auto BytePtr = Iter->asByte<RebaseByte>();
            const auto &Byte = *BytePtr;

            Info->Error = ErrorEnum::None;
            Prev = BytePtr;
            Iter++;

            if (Iter.isAtEnd()) {
                ReachedEnd = true;
                return ErrorEnum::None;
            }

            switch (Byte.opcode()) {
                case RebaseByte::Opcode::Done:
                    ReachedEnd = true;
                    return ErrorEnum::None;
                case RebaseByte::Opcode::SetKindImm:
                    Info->Kind = RebaseByte::WriteKind(Byte.immediate());
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
                    if (const auto Uleb = Iter->ReadUleb128()) {
                        Info->SegOffset = Uleb.value();
                        Info->SegmentIndex = Byte.immediate();
                        Info->HasSegmentIndex = true;

                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::AddAddrImmScaled:
                    Info->Scale = Byte.immediate();
                    return ErrorEnum::None;
                case RebaseByte::Opcode::AddAddrUleb:
                    if (const auto Sleb = Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    if (const auto Sleb = Iter->ReadSleb128()) {
                        Info->AddAddr = Sleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseImmTimes:
                    Info->Count = Byte.immediate();
                    return ErrorEnum::None;
                case RebaseByte::Opcode::DoRebaseUlebTimes:
                    if (const auto Uleb = Iter->ReadUleb128()) {
                        Info->Count = Uleb.value();
                        return ErrorEnum::None;
                    }

                    return ErrorEnum::InvalidLeb128;
                case RebaseByte::Opcode::DoRebaseUlebTimesSkipUleb:
                    if (const auto Count = Iter->ReadUleb128()) {
                        if (const auto Skip = Iter->ReadSleb128()) {
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
        using Iterator = RebaseOpcodeIterator;
    protected:
        const RebaseByte *Begin;
        const RebaseByte *End;
        bool Is64Bit : 1;
    public:
        explicit
        RebaseOpcodeList(const uint8_t *const Begin,
                         const uint8_t *const End,
                         const bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const RebaseByte *>(Begin)),
          End(reinterpret_cast<const RebaseByte *>(End)),
          Is64Bit(Is64Bit) {}

        explicit
        RebaseOpcodeList(const ADT::MemoryMap &Map,
                         const bool Is64Bit) noexcept
        : Begin(Map.base<const RebaseByte>()), End(Map.end<const RebaseByte>()),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return RebaseOpcodeIteratorEnd();
        }
    };

    struct RebaseActionIterateInfo : public RebaseOpcodeIterateInfo {
        using ErrorEnum = RebaseOpcodeParseError;

        [[nodiscard]] constexpr auto getAction() const noexcept {
            const auto Result = RebaseActionInfo{
                .Kind = this->Kind,
                .SegmentIndex = static_cast<uint32_t>(this->SegmentIndex),
                .SegOffset = this->SegOffset,
                .AddrInSeg = this->AddrInSeg,
                .HasSegmentIndex = this->HasSegmentIndex,
            };

            return Result;
        };

        uint64_t AddrInSeg;
        bool NewSymbolName : 1;

        [[nodiscard]]
        constexpr static auto canIgnoreError(const ErrorEnum Error) noexcept {
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
            operator++();
        }

        [[nodiscard]] inline auto &info() noexcept {
            auto &Info = Iter.info();
            return reinterpret_cast<RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]] auto &info() const noexcept {
            const auto &Info = Iter.info();
            return reinterpret_cast<const RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]] bool
        IsValidForSegmentCollection(const SegmentList &Collection,
                                    const bool Is64Bit) noexcept
        {
            const auto &Info = info();
            const auto SegmentIndex = Info.SegmentIndex;

            if (Info.HasSegmentIndex) {
                return false;
            }

            if (SegmentIndex >= Collection.size()) {
                return false;
            }

            const auto &Segment = Collection.at(SegmentIndex);
            auto ContainsPtr = false;

            if (Is64Bit) {
                ContainsPtr =
                    Segment.VmRange.template containsIndex<uint64_t>(
                        Info.AddrInSeg);
            } else {
                ContainsPtr =
                    Segment.VmRange.template containsIndex<uint32_t>(
                        Info.AddrInSeg);
            }

            return ContainsPtr;
        }

        [[nodiscard]] inline auto isAtEnd() const noexcept {
            return Iter.isAtEnd();
        }

        [[nodiscard]] inline auto &operator*() const noexcept {
            return info();
        }

        [[nodiscard]] inline auto operator->() const noexcept {
            return &info();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            auto &Info = info();
            if (Info.hasError()) {
                Iter++;
            }

            Info.Error = Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return operator++();
        }

        inline auto &operator+=(const uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                operator++();
            }

            return *this;
        }

        [[nodiscard]]
        inline auto operator==(const RebaseActionIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] inline
        auto operator!=(const RebaseActionIteratorEnd &End) const noexcept {
            return !operator==(End);
        }

        inline RebaseOpcodeParseError Advance() noexcept {
            auto &Info = info();
            const auto AddChangeToSegmentAddress =
                [&](const int64_t Add) noexcept
            {
                const auto SegAddAddressOpt =
                    Utils::AddAndCheckOverflow<int64_t>(SegAddAddress, Add);

                if (!SegAddAddressOpt.has_value()) {
                    return false;
                }

                SegAddAddress = SegAddAddressOpt.value();
                if (Utils::WillAddOverflow(SegAddAddress, Info.AddrInSeg)) {
                    return false;
                }

                return true;
            };

            using ErrorEnum = RebaseOpcodeParseError;

            Info.Error = ErrorEnum::None;
            Info.NewSymbolName = false;

            const auto FinalizeChangesForSegmentAddress = [&]() noexcept {
                if (SegAddAddress >= 0) {
                    Info.AddrInSeg += static_cast<uint64_t>(SegAddAddress);
                } else {
                    Info.AddrInSeg -= static_cast<uint64_t>(-SegAddAddress);
                }

                SegAddAddress = 0;
            };

            switch (LastByte.opcode()) {
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
                        LastByte.SetOpcode(RebaseByte::Opcode::SetKindImm);

                        Iter++;
                        break;
                    }
                case RebaseByte::Opcode::DoRebaseAddAddrUleb:
                    // Clear the Last-Opcode.
                    LastByte.SetOpcode(RebaseByte::Opcode::SetKindImm);
                    FinalizeChangesForSegmentAddress();
                    Iter++;

                    break;
                case RebaseByte::Opcode::Done:
                    return ErrorEnum::None;
                case RebaseByte::Opcode::SetKindImm:
                case RebaseByte::Opcode::SetSegmentAndOffsetUleb:
                case RebaseByte::Opcode::AddAddrImmScaled:
                case RebaseByte::Opcode::AddAddrUleb:
                    break;
            }

            const auto CheckChangeToSegmentAddress =
                [&](const int64_t Add) noexcept
            {
                const auto NewSegAddAddressOpt =
                    Utils::AddAndCheckOverflow<int64_t>(SegAddAddress,
                                                        Add,
                                                        Info.AddrInSeg);

                if (!NewSegAddAddressOpt.has_value()) {
                    return false;
                }

                if (Utils::WillAddOverflow(NewSegAddAddressOpt.value(),
                                           Info.AddrInSeg))
                {
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

            for (; !Iter.isAtEnd(); Iter++) {
                if (Iter->hasError()) {
                    return Iter->error();
                }

                const auto &Byte = Iter.byte();
                const auto &IterInfo = Iter.info();

                switch (Byte.opcode()) {
                    case RebaseByte::Opcode::Done:
                    case RebaseByte::Opcode::SetKindImm:
                        continue;
                    case RebaseByte::Opcode::SetSegmentAndOffsetUleb:
                        Info.AddrInSeg = IterInfo.SegOffset;
                        continue;
                    case RebaseByte::Opcode::AddAddrImmScaled: {
                        const auto AddrInSegOpt =
                            Utils::MulAddAndCheckOverflow(
                                Info.Scale,
                                Utils::PointerSize(Is64Bit),
                                Info.AddrInSeg);

                        if (!AddrInSegOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        Info.AddrInSeg = AddrInSegOpt.value();
                        continue;
                    }
                    case RebaseByte::Opcode::AddAddrUleb: {
                        const auto AddrInSegOpt =
                            Utils::AddAndCheckOverflow(
                                Info.AddrInSeg, IterInfo.AddAddr);

                        if (!AddrInSegOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        Info.AddrInSeg = AddrInSegOpt.value();
                        continue;
                    }
                    case RebaseByte::Opcode::DoRebaseAddAddrUleb: {
                        const auto Error = CheckIfCanRebase();
                        if (Error != ErrorEnum::None) {
                            return Error;
                        }

                        if (!AddChangeToSegmentAddress(IterInfo.AddAddr)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        if (AddChangeToSegmentAddress(PtrSize)) {
                            LastByte = Byte;
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

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        const auto Count = Info.Count;
                        const auto TotalOpt =
                            Utils::MulAndCheckOverflow<int64_t>(PtrSize, Count);

                        if (!TotalOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto Total = TotalOpt.value();
                        if (CheckChangeToSegmentAddress(Total)) {
                            LastByte = Byte;
                            AddAmt = PtrSize;

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

                        const auto PtrSize = Utils::PointerSize(Is64Bit);
                        const auto SingleStepOpt =
                            Utils::AddAndCheckOverflow(PtrSize, Info.Skip);

                        if (!SingleStepOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        const auto Count = Info.Count;
                        const auto SingleStep = SingleStepOpt.value();
                        const auto TotalOpt =
                            Utils::MulAndCheckOverflow<int64_t>(SingleStep,
                                                                Count);

                        if (!TotalOpt.has_value()) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

                        if (CheckChangeToSegmentAddress(TotalOpt.value())) {
                            LastByte = Byte;
                            AddAmt = SingleStep;

                            this->Count = Count - 1;
                            return ErrorEnum::None;
                        }

                        return ErrorEnum::OutOfBoundsSegmentAddr;
                    }
                }

                return ErrorEnum::UnrecognizedRebaseOpcode;
            }

            LastByte.SetOpcode(RebaseByte::Opcode::Done);
            return ErrorEnum::None;
        }
    };

    struct RebaseActionList {
        using Iterator = RebaseActionIterator;
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
            return Iterator(Begin, End, Is64Bit);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return RebaseActionIteratorEnd();
        }

        inline
        auto getAsList(std::vector<RebaseActionInfo> &ListOut) const noexcept {
            for (const auto &Iter : *this) {
                const auto Error = Iter.error();
                if (!Iter.canIgnoreError(Error)) {
                    return Error;
                }

                ListOut.emplace_back(Iter.getAction());
            }

            return RebaseOpcodeParseError::None;
        }

        inline auto
        getListOfSymbols(
            std::vector<RebaseActionInfo> &SymbolListOut) const noexcept
        {
            for (const auto &Iter : *this) {
                const auto Error = Iter.error();
                if (!Iter.canIgnoreError(Error)) {
                    return Error;
                }

                if (Iter.NewSymbolName) {
                    SymbolListOut.emplace_back(Iter.getAction());
                }
            }

            return RebaseOpcodeParseError::None;
        }
    };
}