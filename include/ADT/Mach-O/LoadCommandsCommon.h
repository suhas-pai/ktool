//
//  ADT/Mach-O/DyldInfoCommon.h
//  ktool
//
//  Created by Suhas Pai on 5/18/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <optional>

#include "ADT/BasicMasksHandler.h"
#include "ADT/Range.h"
#include "ADT/MemoryMap.h"
#include "Utils/Leb128.h"

namespace MachO {
    constexpr static auto KindRequiredByDyld = 0x80000000ul;
    enum class LoadCommandKind : uint32_t {
        Segment = 1,
        SymbolTable,
        SymbolSegment,
        Thread,
        UnixThread,
        LoadFixedVMSharedLibrary,
        IdFixedVMSharedLibrary,
        Ident,
        FixedVMFile,
        PrePage,
        DynamicSymbolTable,
        LoadDylib,
        IdDylib,
        LoadDylinker,
        IdDylinker,
        PreBoundDylib,
        Routines,
        SubFramework,
        SubUmbrella,
        SubClient,
        SubLibrary,
        TwoLevelHints,
        PrebindChecksum,
        LoadWeakDylib = (KindRequiredByDyld | 0x18ul),
        Segment64 = 0x19,
        Routines64,
        Uuid,
        Rpath = (KindRequiredByDyld | 0x1cul),
        CodeSignature = 0x1d,
        SegmentSplitInfo,
        ReexportDylib = (KindRequiredByDyld | 0x1ful),
        LazyLoadDylib = 0x20,
        EncryptionInfo,
        DyldInfo,
        DyldInfoOnly = (KindRequiredByDyld | 0x22ul),
        LoadUpwardDylib = (KindRequiredByDyld | 0x23ul),
        VersionMinimumMacOSX = 0x24,
        VersionMinimumIPhoneOS,
        FunctionStarts,
        DyldEnvironment,
        Main = (KindRequiredByDyld | 0x28ul),
        DataInCode = 0x29,
        SourceVersion,
        DylibCodeSignDRS,
        EncryptionInfo64,
        LinkerOption,
        LinkerOptimizationHint,
        VersionMinimumTvOS,
        VersionMinimumWatchOS,
        Note,
        BuildVersion,
        DyldExportsTrie = (KindRequiredByDyld | 0x33ul),
        DyldChainedFixups = (KindRequiredByDyld | 0x34ul),
        FileSetEntry = (KindRequiredByDyld | 0x35ul)
    };

    enum class SizeRangeError {
        None,
        Empty,
        Overflows,
        PastEnd,
    };

    [[nodiscard]] inline auto
    CheckSizeRange(const ConstMemoryMap &Map,
                   const uint32_t Offset,
                   const uint32_t Size,
                   uint64_t *const EndOut) noexcept
        -> SizeRangeError
    {
        auto End = static_cast<uint64_t>(Offset) + Size;
        auto Range = Range::CreateWithEnd(Offset, End);

        if (!Map.getRange().contains(Range)) {
            return SizeRangeError::PastEnd;
        }

        if (Size == 0) {
            return SizeRangeError::Empty;
        }

        if (EndOut != nullptr) {
            *EndOut = End;
        }

        return SizeRangeError::None;
    }

    enum class DyldInfoByteMasks : uint8_t {
        Opcode    = 0xF0,
        Immediate = 0x0F
    };

    enum class DyldInfoByteShifts : uint8_t {
        Opcode    = 4,
        Immediate = 0
    };

    using DyldInfoByteMasksHandler =
        BasicMasksHandler<DyldInfoByteMasks, uint8_t>;

    using DyldInfoByteMasksAndShiftsHandler =
        BasicMasksAndShiftsHandler<DyldInfoByteMasks,
                                   DyldInfoByteShifts,
                                   uint8_t>;

    struct NakedOpcodeList {
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        explicit
        NakedOpcodeList(const uint8_t *const Begin,
                        const uint8_t *const End) noexcept
        : Begin(Begin), End(End) {}

        explicit
        NakedOpcodeList(const uint8_t *const Begin,
                        const uint32_t Size) noexcept
        : Begin(Begin), End(Begin + Size) {}

        struct EndIterator {};

        struct Iterator;
        struct IteratorInfo {
            friend Iterator;
        protected:
            const uint8_t *Iter;
            const uint8_t *End;
        public:
            explicit
            IteratorInfo(const uint8_t *const Iter,
                         const uint8_t *const End) noexcept
            : Iter(Iter), End(End) {}

            [[nodiscard]] inline auto getPtr() const noexcept {
                return this->Iter;
            }

            template <typename T>
            [[nodiscard]] inline auto getAsByte() const noexcept {
                return reinterpret_cast<const T *>(this->Iter);
            }

            template <typename T = uint64_t>
            [[nodiscard]] auto ReadUleb128() noexcept
                -> std::optional<T>
            {
                auto Value = T();
                auto *Ptr = ::ReadUleb128(Iter, End, &Value);

                if (Ptr == nullptr) {
                    return std::nullopt;
                }

                this->Iter = Ptr;
                return Value;
            }

            template <typename T = int64_t>
            [[nodiscard]] auto ReadSleb128() noexcept
                -> std::optional<T>
            {
                auto Value = T();
                auto *Ptr = ::ReadSleb128(Iter, End, &Value);

                if (Ptr == nullptr) {
                    return std::nullopt;
                }

                this->Iter = Ptr;
                return Value;
            }

            [[nodiscard]] auto ParseString() noexcept
                -> std::optional<std::string_view>;

            [[nodiscard]] inline auto isAtEnd() const noexcept {
                return Iter >= End;
            }
        };

        struct Iterator {
        protected:
            IteratorInfo Info;
        public:
            explicit
            Iterator(const uint8_t *const Iter,
                     const uint8_t *const End) noexcept
            : Info(Iter, End) {}

            [[nodiscard]] inline auto getPtr() const noexcept {
                return this->Info.getPtr();
            }

            [[nodiscard]] inline auto &operator*() noexcept {
                return this->Info;
            }

            [[nodiscard]] inline auto operator->() noexcept {
                return &this->Info;
            }

            [[nodiscard]] inline auto &operator*() const noexcept {
                return this->Info;
            }

            [[nodiscard]] inline auto operator->() const noexcept {
                return &this->Info;
            }

            [[nodiscard]] inline auto isAtEnd() const noexcept {
                return this->Info.isAtEnd();
            }

            [[nodiscard]]
            inline auto operator==(const EndIterator &) const noexcept {
                return this->isAtEnd();
            }

            [[nodiscard]]
            inline auto operator!=(const EndIterator &) const noexcept {
                return !this->isAtEnd();
            }

            inline auto operator++() noexcept
                -> decltype(*this)
            {
                this->Info.Iter++;
                return *this;
            }

            inline auto operator++(int) noexcept
                -> decltype(*this)
            {
                return ++(*this);
            }

            inline auto operator+=(const uint64_t Amt) noexcept
                -> decltype(*this)
            {
                for (auto I = uint64_t(); I != Amt; I++) {
                    (*this)++;
                }

                return *this;
            }
        };

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return EndIterator();
        }
    };

    enum class SegmentFlagsEnum : uint32_t {
        HighVM             = 1 << 0,
        FixedVMLibary      = 1 << 1,
        NoRelocations      = 1 << 2,
        ProtectionVersion1 = 1 << 3,
        ReadOnlyAfterFixup = 1 << 4
    };

    [[nodiscard]]
    constexpr auto SegmentFlagsGetName(const SegmentFlagsEnum Val) noexcept
        -> std::optional<std::string_view>
    {
        switch (Val) {
            using Enum = SegmentFlagsEnum;
            case Enum::HighVM:
                return "SG_HIGHVM";
            case Enum::FixedVMLibary:
                return "SG_FVMLIB";
            case Enum::NoRelocations:
                return "SG_NORELOC";
            case Enum::ProtectionVersion1:
                return "SG_PROTECTED_VERSION_1";
            case Enum::ReadOnlyAfterFixup:
                return "SG_READ_ONLY";
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    SegmentFlagsGetDesc(const SegmentFlagsEnum Val) noexcept
        -> std::optional<std::string_view>
    {
        switch (Val) {
            using Enum = SegmentFlagsEnum;
            case Enum::HighVM:
                return "High Vm";
            case Enum::FixedVMLibary:
                return "Fixed Vm Library";
            case Enum::NoRelocations:
                return "No Relocations";
            case Enum::ProtectionVersion1:
                return "Protected (Version 1)";
            case Enum::ReadOnlyAfterFixup:
                return "Read-Only (After Fix-Ups)";
        }

        return std::nullopt;
    }

    struct SegmentFlags : public ::BasicFlags<SegmentFlagsEnum> {
    private:
        using Base = ::BasicFlags<SegmentFlagsEnum>;
    public:
        using FlagEnum = SegmentFlagsEnum;
        using FlagIntegerType = std::underlying_type_t<FlagEnum>;

        constexpr SegmentFlags() noexcept = default;
        constexpr SegmentFlags(const FlagIntegerType Integer) noexcept
        : Base(Integer) {}

        [[nodiscard]] constexpr auto isHighVM() const noexcept {
            return this->hasFlag(FlagEnum::HighVM);
        }

        [[nodiscard]] constexpr auto isFixedVMLibary() const noexcept {
            return this->hasFlag(FlagEnum::FixedVMLibary);
        }

        [[nodiscard]] constexpr auto isNoRelocations() const noexcept {
            return this->hasFlag(FlagEnum::NoRelocations);
        }

        [[nodiscard]] constexpr auto isProtected() const noexcept {
            return this->hasFlag(FlagEnum::ProtectionVersion1);
        }

        [[nodiscard]] constexpr auto isReadOnlyAfterFixup() const noexcept {
            return this->hasFlag(FlagEnum::ReadOnlyAfterFixup);
        }

        constexpr auto setHighVM(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(FlagEnum::HighVM, Value);
            return *this;
        }

        constexpr auto setFixedVMLibary(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(FlagEnum::FixedVMLibary, Value);
            return *this;
        }

        constexpr auto setNoRelocations(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(FlagEnum::NoRelocations, Value);
            return *this;
        }

        constexpr auto setIsProtected(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(FlagEnum::ProtectionVersion1, Value);
            return *this;
        }

        constexpr auto setReadOnlyAfterFixup(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForFlag(FlagEnum::ReadOnlyAfterFixup, Value);
            return *this;
        }
    };

    enum class SegmentSectionKind {
        Regular,
        ZeroFillOnDemand,
        CStringLiterals,
        FourByteLiterals,
        EightByteLiterals,
        LiteralPointers,
        NonLazySymbolPointers,
        LazySymbolPointers,
        SymbolStubs,
        ModInitFunctionPointers,
        ModTermFunctionPointers,
        CoalescedSymbols,
        ZeroFillOnDemandGigaBytes,
        InterposingFunctions,
        SixteenByteLiterals,
        DTraceDOF,
        LazyDylibSymbolPointers,

        ThreadLocalRegular,
        ThreadLocalZeroFill,
        ThreadLocalVariables,
        ThreadLocalVariablePointers,
        ThreadLocalInitFunctionPointers,
        InitFunction32BitOffsets
    };

    constexpr auto
    SegmentSectionKindGetName(const SegmentSectionKind Type) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = SegmentSectionKind;
        switch (Type) {
            case Enum::Regular:
                return "S_REGULAR";
            case Enum::ZeroFillOnDemand:
                return "S_ZEROFILL";
            case Enum::CStringLiterals:
                return "S_CSTRING_LITERALS";
            case Enum::FourByteLiterals:
                return "S_4BYTE_LITERALS";
            case Enum::EightByteLiterals:
                return "S_8BYTE_LITERALS";
            case Enum::LiteralPointers:
                return "S_LITERAL_POINTERS";
            case Enum::NonLazySymbolPointers:
                return "S_NON_LAZY_SYMBOL_POINTERS";
            case Enum::LazySymbolPointers:
                return "S_LAZY_SYMBOL_POINTERS";
            case Enum::SymbolStubs:
                return "S_SYMBOL_STUBS";
            case Enum::ModInitFunctionPointers:
                return "S_MOD_INIT_FUNC_POINTERS";
            case Enum::ModTermFunctionPointers:
                return "S_MOD_TERM_FUNC_POINTERS";
            case Enum::CoalescedSymbols:
                return "Coalesced Symbols";
            case Enum::ZeroFillOnDemandGigaBytes:
                return "Zero-Fill (GigaBytes)";
            case Enum::InterposingFunctions:
                return "Interposing Functions";
            case Enum::SixteenByteLiterals:
                return "S_16BYTE_LITERALS";
            case Enum::DTraceDOF:
                return "S_DTRACE_DOF";
            case Enum::LazyDylibSymbolPointers:
                return "S_THREAD_LOCAL_REGULAR";
            case Enum::ThreadLocalRegular:
                return "S_THREAD_LOCAL_REGULAR";
            case Enum::ThreadLocalZeroFill:
                return "S_THREAD_LOCAL_ZEROFILL";
            case Enum::ThreadLocalVariables:
                return "S_THREAD_LOCAL_VARIABLES";
            case Enum::ThreadLocalVariablePointers:
                return "S_THREAD_LOCAL_VARIABLE_POINTERS";
            case Enum::ThreadLocalInitFunctionPointers:
                return "S_THREAD_LOCAL_INIT_FUNCTION_POINTERS";
            case Enum::InitFunction32BitOffsets:
                return "S_INIT_FUNC_OFFSETS";
        }

        return std::nullopt;
    }

    constexpr auto
    SegmentSectionKindGetDescription(const SegmentSectionKind Type) noexcept
        -> std::optional<std::string_view>
    {
        using Enum = SegmentSectionKind;
        switch (Type) {
            case Enum::Regular:
                return "Regular";
            case Enum::ZeroFillOnDemand:
                return "Zerofill";
            case Enum::CStringLiterals:
                return "C-String Literals";
            case Enum::FourByteLiterals:
                return "Four-Byte Literal";
            case Enum::EightByteLiterals:
                return "Eight-Byte Literal";
            case Enum::LiteralPointers:
                return "Literal Pointers";
            case Enum::NonLazySymbolPointers:
                return "Non-Lazy Symbol-Pointers";
            case Enum::LazySymbolPointers:
                return "Lazy Symbol-Pointers";
            case Enum::SymbolStubs:
                return "Symbol Stubs";
            case Enum::ModInitFunctionPointers:
                return "Mod-Init Function Pointers";
            case Enum::ModTermFunctionPointers:
                return "Mod-Term Function Pointers";
            case Enum::CoalescedSymbols:
                return "Coalesced Symbols";
            case Enum::ZeroFillOnDemandGigaBytes:
                return "Zero-Fill (GigaBytes)";
            case Enum::InterposingFunctions:
                return "Interposing Functions";
            case Enum::SixteenByteLiterals:
                return "16-Byte Literals";
            case Enum::DTraceDOF:
                return "DTrace DOFs";
            case Enum::LazyDylibSymbolPointers:
                return "Lazy Dylib Lazy-Symbol Pointers";
            case Enum::ThreadLocalRegular:
                return "Thread-Local Regular";
            case Enum::ThreadLocalZeroFill:
                return "Thread-Local Zero-Fill";
            case Enum::ThreadLocalVariables:
                return "Thread-Local Variables";
            case Enum::ThreadLocalVariablePointers:
                return "Thread-Local Variable-Pointers";
            case Enum::ThreadLocalInitFunctionPointers:
                return "Thread-Local Init-Function Pointers";
            case Enum::InitFunction32BitOffsets:
                return "32-Bit Init Function Offsets";
        }

        return std::nullopt;
    }

    constexpr static auto SegmentSectionUserSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    constexpr static auto SegmentSectionSystemSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    enum class SegmentSectionAttributesMasks : uint32_t {
        IsAllInstructions = 0x80000000,
        NoRanlibTableOfContents = 0x40000000,
        StripStaticSymbols = 0x20000000,
        NoDeadStripping = 0x10000000,
        LiveSupport = 0x08000000,
        SelfModifyingCode = 0x04000000,

        IsDebugSection = 0x02000000,
        HasSomeInstructions = 0x00000400,
        HasExternalRelocEntries = 0x00000200,
        HasLocalRelocEntries = 0x00000100,
    };

    struct SegmentSectionAttributes :
        public ::BasicFlags<SegmentSectionAttributesMasks>
    {
    private:
        using Base = ::BasicFlags<SegmentSectionAttributesMasks>;
    public:
        using MaskKind = SegmentSectionAttributesMasks;
        using MaskIntegerType = std::underlying_type_t<MaskKind>;

        using Base::Base;

        [[nodiscard]] constexpr auto isAllInstructions() const noexcept {
            return this->hasValueForMask(MaskKind::IsAllInstructions);
        }

        [[nodiscard]] constexpr auto noRanlibTableOfContents() const noexcept {
            const auto NoRanlibTableOfContents =
                this->hasValueForMask(MaskKind::NoRanlibTableOfContents);

            return NoRanlibTableOfContents;
        }

        [[nodiscard]] constexpr auto shouldStripStaticSymbols() const noexcept {
            return this->hasValueForMask(MaskKind::StripStaticSymbols);
        }

        [[nodiscard]] constexpr auto noDeadStripping() const noexcept {
            return this->hasValueForMask(MaskKind::NoDeadStripping);
        }

        [[nodiscard]] constexpr auto hasLiveSupport() const noexcept {
            return this->hasValueForMask(MaskKind::LiveSupport);
        }

        [[nodiscard]] constexpr auto hasSelfModifyingCode() const noexcept {
            return this->hasValueForMask(MaskKind::SelfModifyingCode);
        }

        [[nodiscard]] constexpr bool isDebugSection() const noexcept {
            return this->hasValueForMask(MaskKind::IsDebugSection);
        }

        [[nodiscard]] constexpr auto hasSomeInstructions() const noexcept {
            return this->hasValueForMask(MaskKind::HasSomeInstructions);
        }

        [[nodiscard]] constexpr auto hasExternalRelocEntries() const noexcept {
            return this->hasValueForMask(MaskKind::HasExternalRelocEntries);
        }

        [[nodiscard]] constexpr auto hasLocalRelocEntries() const noexcept {
            return this->hasValueForMask(MaskKind::HasLocalRelocEntries);
        }

        constexpr auto setIsAllInstructions(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::IsAllInstructions, Value);
            return *this;
        }

        constexpr
        auto setNoRanlibTableOfContents(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::NoRanlibTableOfContents, Value);
            return *this;
        }

        constexpr auto setStripStaticSymbols(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::StripStaticSymbols, Value);
            return *this;
        }

        constexpr auto setNoDeadStripping(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::NoDeadStripping, Value);
            return *this;
        }

        constexpr auto setLiveSupport(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::LiveSupport, Value);
            return *this;
        }

        constexpr auto setSelfModifyingCode(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::SelfModifyingCode, Value);
            return *this;
        }

        constexpr auto setIsDebugSection(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::IsDebugSection, Value);
            return *this;
        }

        constexpr auto setHasSomeInstructions(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::HasSomeInstructions, Value);
            return *this;
        }

        constexpr
        auto setHasExternalRelocEntries(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::HasExternalRelocEntries, Value);
            return *this;
        }

        constexpr auto setHasLocalRelocEntries(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskKind::HasLocalRelocEntries, Value);
            return *this;
        }
    };

    enum class SegmentSectionMasks : uint32_t {
        Attributes = 0xffffff00,
        Kind = 0x000000ff,
    };

    struct SegmentSectionFlags :
        public BasicMasksHandler<SegmentSectionMasks> {
    private:
        using Base = BasicMasksHandler<SegmentSectionMasks>;
    public:
        using MaskType = SegmentSectionMasks;
        using MaskIntegerType = std::underlying_type_t<MaskType>;

        using Kind = SegmentSectionKind;
        using Attributes = SegmentSectionAttributes;

        using Base::Base;

        [[nodiscard]] constexpr auto getAttributes() const noexcept {
            return Attributes(this->getValueForMask(MaskType::Attributes));
        }

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind(this->getValueForMask(MaskType::Kind));
        }

        [[nodiscard]] constexpr auto
        setAttributes(const Attributes Attributes) noexcept
            -> decltype(*this)
        {
            this->setValueForMask(MaskType::Attributes, Attributes.value());
            return *this;
        }

        [[nodiscard]]
        constexpr auto setType(const Kind Kind) noexcept -> decltype(*this) {
            this->setValueForMask(MaskType::Kind,
                                  static_cast<MaskIntegerType>(Kind));

            return *this;
        }
    };
}
