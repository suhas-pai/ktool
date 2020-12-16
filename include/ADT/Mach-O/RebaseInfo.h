//
//  include/ADT/Mach-O/RebaseInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once
#include <memory>

#include "ADT/EnumHelper.h"
#include "ADT/PointerOrError.h"
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

    [[nodiscard]] constexpr
    std::string_view RebaseByteOpcodeGetName(RebaseByteOpcode Opcode) noexcept {
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

        return std::string_view();
    }

    [[nodiscard]] constexpr
    std::string_view RebaseByteOpcodeGetDesc(RebaseByteOpcode Opcode) noexcept {
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

        return std::string_view();
    }

    enum class RebaseWriteKind : uint8_t {
        None,
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    [[nodiscard]] constexpr std::string_view
    RebaseWriteKindGetName(RebaseWriteKind Kind) noexcept {
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

        return std::string_view();
    }

    [[nodiscard]] constexpr std::string_view
    RebaseWriteKindGetDescription(RebaseWriteKind Kind) noexcept {
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

        return std::string_view();
    }

    [[nodiscard]]
    constexpr inline uint64_t RebaseWriteKindGetLongestDescription() noexcept {
        const auto Result =
            EnumHelper<RebaseWriteKind>::GetLongestAssocLength(
                RebaseWriteKindGetName);

        return Result;
    }

    enum class RebaseByteDylibSpecialOrdinal : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    struct RebaseByte : private DyldInfoByteMasksAndShiftsHandler {
        constexpr static inline auto DoneOpcode = RebaseByteOpcode::Done;
    public:
        constexpr RebaseByte() noexcept = default;
        constexpr RebaseByte(uint8_t Byte) noexcept
        : DyldInfoByteMasksAndShiftsHandler(Byte) {}

        using Masks = DyldInfoByteMasks;
        using Opcode = RebaseByteOpcode;
        using Shifts = DyldInfoByteShifts;
        using WriteKind = RebaseWriteKind;

        [[nodiscard]] constexpr inline Opcode getOpcode() const noexcept {
            return Opcode(getValueForMaskNoShift(Masks::Opcode));
        }

        [[nodiscard]] constexpr inline uint8_t getImmediate() const noexcept {
            return getValueForMaskNoShift(Masks::Immediate);
        }

        constexpr inline RebaseByte &SetOpcode(Opcode Opcode) noexcept {
            setValueForMaskNoShift(Masks::Opcode,
                                   static_cast<IntegerType>(Opcode));
            return *this;
        }

        constexpr inline RebaseByte &SetImmediate(uint8_t Immediate) noexcept {
            setValueForMaskNoShift(Masks::Immediate, Immediate);
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

            [[nodiscard]] inline RebaseByte getByte() const noexcept {
                return *reinterpret_cast<const RebaseByte *>(this->getPtr());
            }
        };

        [[nodiscard]] inline Iterator begin() const noexcept {
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

        [[nodiscard]] constexpr inline bool hasError() const noexcept {
            return (Error != RebaseOpcodeParseError::None);
        }

        [[nodiscard]]
        constexpr inline RebaseOpcodeParseError getError() const noexcept {
            return Error;
        }

        [[nodiscard]]
        constexpr inline int64_t GetSegmentIndex() const noexcept {
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
        bool ReachedEnd : 1;
    public:
        explicit
        RebaseOpcodeIterator(const RebaseByte *Begin,
                             const RebaseByte *End) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          ReachedEnd(false)
        {
            Info = std::make_unique<RebaseOpcodeIterateInfo>();
            Advance();
        }

        explicit
        RebaseOpcodeIterator(
            const RebaseByte *Begin,
            const RebaseByte *End,
            std::unique_ptr<RebaseOpcodeIterateInfo> &&Info) noexcept
        : Iter(reinterpret_cast<const uint8_t *>(Begin),
               reinterpret_cast<const uint8_t *>(End)),
          Info(std::move(Info))
        {
            ReachedEnd = false;
            Advance();
        }

        RebaseOpcodeIterator(const RebaseOpcodeIterator &) = delete;

        [[nodiscard]] constexpr inline bool isAtEnd() const noexcept {
            return ReachedEnd;
        }

        [[nodiscard]] constexpr
        inline const RebaseOpcodeIterator &operator*() const noexcept {
            return *this;
        }

        [[nodiscard]] constexpr
        inline const RebaseOpcodeIterator *operator->() const noexcept {
            return this;
        }

        [[nodiscard]]
        inline RebaseOpcodeIterateInfo &getInfo() noexcept {
            return *Info;
        }

        [[nodiscard]]
        inline const RebaseOpcodeIterateInfo &getInfo() const noexcept {
            return *Info;
        }

        [[nodiscard]] constexpr inline RebaseByte getByte() const noexcept {
            return *Prev;
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return Info->hasError();
        }

        [[nodiscard]] inline ErrorEnum getError() const noexcept {
            return Info->getError();
        }

        inline RebaseOpcodeIterator &operator++() noexcept {
            Info->Error = Advance();
            return *this;
        }

        inline RebaseOpcodeIterator &operator++(int) noexcept {
            return ++(*this);
        }

        constexpr
        inline RebaseOpcodeIterator &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]] constexpr
        inline bool operator==(const RebaseOpcodeIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] constexpr inline
        bool operator!=(const RebaseOpcodeIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        inline RebaseOpcodeParseError Advance() noexcept {
            using ErrorEnum = RebaseOpcodeParseError;

            const auto BytePtr = Iter->getAsByte<RebaseByte>();
            const auto &Byte = *BytePtr;

            Info->Error = ErrorEnum::None;
            Prev = BytePtr;
            Iter++;

            if (Iter.isAtEnd()) {
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
                    if (const auto Uleb = Iter->ReadUleb128()) {
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
                    Info->Count = Byte.getImmediate();
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
        using IteratorType = RebaseOpcodeIterator;
    protected:
        const RebaseByte *Begin;
        const RebaseByte *End;
        bool Is64Bit : 1;
    public:
        explicit
        RebaseOpcodeList(const uint8_t *Begin,
                         const uint8_t *End,
                         bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const RebaseByte *>(Begin)),
          End(reinterpret_cast<const RebaseByte *>(End)),
          Is64Bit(Is64Bit) {}

        [[nodiscard]] inline IteratorType begin() const noexcept {
            return IteratorType(Begin, End);
        }

        [[nodiscard]]
        constexpr inline RebaseOpcodeIteratorEnd end() const noexcept {
            return RebaseOpcodeIteratorEnd();
        }
    };

    struct RebaseActionIterateInfo : public RebaseOpcodeIterateInfo {
        using ErrorEnum = RebaseOpcodeParseError;

        [[nodiscard]]
        constexpr struct RebaseActionInfo GetAction() const noexcept {
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
        constexpr static inline bool CanIgnoreError(ErrorEnum Error) noexcept {
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
        RebaseActionIterator(const RebaseByte *Begin,
                             const RebaseByte *End,
                             bool Is64Bit) noexcept
        : Iter(Begin, End, std::make_unique<RebaseActionIterateInfo>()),
          Is64Bit(Is64Bit)
        {
            LastByte.SetOpcode(RebaseByte::Opcode::SetKindImm);
            ++(*this);
        }

        [[nodiscard]]
        inline RebaseActionIterateInfo &getInfo() noexcept {
            auto &Info = Iter.getInfo();
            return reinterpret_cast<RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]]
        const RebaseActionIterateInfo &getInfo() const noexcept {
            const auto &Info = Iter.getInfo();
            return reinterpret_cast<const RebaseActionIterateInfo &>(Info);
        }

        [[nodiscard]] bool
        IsValidForSegmentCollection(const SegmentInfoCollection &Collection,
                                    bool Is64Bit) noexcept
        {
            const auto &Info = getInfo();
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

        [[nodiscard]] inline bool isAtEnd() const noexcept {
            return Iter.isAtEnd();
        }

        [[nodiscard]]
        inline const RebaseActionIterateInfo &operator*() const noexcept {
            return getInfo();
        }

        [[nodiscard]]
        inline const RebaseActionIterateInfo *operator->() const noexcept {
            return &getInfo();
        }

        inline RebaseActionIterator &operator++() noexcept {
            auto &Info = getInfo();
            if (Info.hasError()) {
                Iter++;
            }

            Info.Error = Advance();
            return *this;
        }

        inline RebaseActionIterator &operator++(int) noexcept {
            return ++(*this);
        }

        inline RebaseActionIterator &operator+=(uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                ++(*this);
            }

            return *this;
        }

        [[nodiscard]]
        inline bool operator==(const RebaseActionIteratorEnd &) const noexcept {
            return isAtEnd();
        }

        [[nodiscard]] inline
        bool operator!=(const RebaseActionIteratorEnd &End) const noexcept {
            return !(*this == End);
        }

        inline RebaseOpcodeParseError Advance() noexcept {
            auto &Info = getInfo();
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

            for (; !Iter.isAtEnd(); Iter++) {
                if (Iter->hasError()) {
                    return Iter->getError();
                }

                const auto &Byte = Iter.getByte();
                const auto &IterInfo = Iter.getInfo();

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

                        auto Total = uint64_t();

                        const auto PtrSize = PointerSize(Is64Bit);
                        const auto Count = Info.Count;

                        if (DoesMultiplyOverflow(PtrSize, Count, &Total)) {
                            return ErrorEnum::OutOfBoundsSegmentAddr;
                        }

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

    using ConstRebaseActionIterator = RebaseActionIterator;
    struct RebaseActionList {
        using IteratorType = RebaseActionIterator;
    protected:
        const RebaseByte *Begin;
        const RebaseByte *End;
        bool Is64Bit : 1;
    public:
        explicit
        RebaseActionList(const uint8_t *Begin,
                         const uint8_t *End,
                         bool Is64Bit) noexcept
        : Begin(reinterpret_cast<const RebaseByte *>(Begin)),
          End(reinterpret_cast<const RebaseByte *>(End)),
          Is64Bit(Is64Bit) {}

        explicit
        RebaseActionList(const RebaseByte *Begin,
                         const RebaseByte *End,
                         bool Is64Bit) noexcept
        : Begin(Begin), End(End), Is64Bit(Is64Bit) {}

        [[nodiscard]] inline IteratorType begin() const noexcept {
            return IteratorType(Begin, End, Is64Bit);
        }

        [[nodiscard]] inline RebaseActionIteratorEnd end() const noexcept {
            return RebaseActionIteratorEnd();
        }

        inline RebaseOpcodeParseError
        GetAsList(std::vector<RebaseActionInfo> &ListOut) const noexcept {
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
        const noexcept {
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

    TypedAllocationOrError<RebaseNakedOpcodeList, SizeRangeError>
    GetRebaseNakedOpcodeList(const MemoryMap &Map,
                             uint32_t RebaseOffset,
                             uint32_t RebaseSize) noexcept;

    TypedAllocationOrError<ConstRebaseNakedOpcodeList, SizeRangeError>
    GetConstRebaseNakedOpcodeList(const ConstMemoryMap &Map,
                                  uint32_t RebaseOffset,
                                  uint32_t RebaseSize) noexcept;

    TypedAllocationOrError<RebaseOpcodeList, SizeRangeError>
    GetRebaseOpcodeList(const ConstMemoryMap &Map,
                        uint32_t RebaseOffset,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept;

    TypedAllocationOrError<RebaseActionList, SizeRangeError>
    GetRebaseActionList(const ConstMemoryMap &Map,
                        uint32_t RebaseOffset,
                        uint32_t RebaseSize,
                        bool Is64Bit) noexcept;
}
