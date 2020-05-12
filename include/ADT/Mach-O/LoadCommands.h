//
//  include/ADT/MachO/LoadCommands.h
//  stool
//
//  Created by Suhas Pai on 3/7/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <optional>
#include <stack>
#include <string_view>
#include <vector>

#include "ADT/BasicContinguousList.h"
#include "ADT/BasicMasksHandler.h"
#include "ADT/LargestIntHelper.h"
#include "ADT/LocationRange.h"
#include "ADT/MemoryMap.h"
#include "ADT/PointerErrorStorage.h"
#include "ADT/PointerOrError.h"

#include "TypeTraits/IsEnumClass.h"

#include "Utils/DoesOverflow.h"
#include "Utils/SwitchEndian.h"

#include "Types.h"

namespace MachO {
    struct LoadCommand {
        constexpr static const auto KindRequiredByDyld = 0x80000000ul;
        enum class Kind : uint32_t {
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
            DyldChainedFixups = (KindRequiredByDyld | 0x34ul)
        };

        [[nodiscard]] static inline bool IsOfKind(LoadCommand::Kind) noexcept {
            return false;
        }

        static const std::string_view &KindGetName(Kind Kind) noexcept;
        static const std::string_view &KindGetDescription(Kind Kind) noexcept;

        [[nodiscard]]
        constexpr static bool KindIsRequiredByDyld(Kind Kind) noexcept {
            return (static_cast<uint32_t>(Kind) & KindRequiredByDyld);
        }

        constexpr static bool KindIsValid(Kind Kind) noexcept {
            switch (Kind) {
                case Kind::Segment:
                case Kind::SymbolTable:
                case Kind::SymbolSegment:
                case Kind::Thread:
                case Kind::UnixThread:
                case Kind::LoadFixedVMSharedLibrary:
                case Kind::IdFixedVMSharedLibrary:
                case Kind::Ident:
                case Kind::FixedVMFile:
                case Kind::PrePage:
                case Kind::DynamicSymbolTable:
                case Kind::LoadDylib:
                case Kind::IdDylib:
                case Kind::LoadDylinker:
                case Kind::IdDylinker:
                case Kind::PreBoundDylib:
                case Kind::Routines:
                case Kind::SubFramework:
                case Kind::SubUmbrella:
                case Kind::SubClient:
                case Kind::SubLibrary:
                case Kind::TwoLevelHints:
                case Kind::PrebindChecksum:
                case Kind::LoadWeakDylib:
                case Kind::Segment64:
                case Kind::Routines64:
                case Kind::Uuid:
                case Kind::Rpath:
                case Kind::CodeSignature:
                case Kind::SegmentSplitInfo:
                case Kind::ReexportDylib:
                case Kind::LazyLoadDylib:
                case Kind::EncryptionInfo:
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly:
                case Kind::LoadUpwardDylib:
                case Kind::VersionMinimumMacOSX:
                case Kind::VersionMinimumIPhoneOS:
                case Kind::FunctionStarts:
                case Kind::DyldEnvironment:
                case Kind::Main:
                case Kind::DataInCode:
                case Kind::SourceVersion:
                case Kind::DylibCodeSignDRS:
                case Kind::EncryptionInfo64:
                case Kind::LinkerOption:
                case Kind::LinkerOptimizationHint:
                case Kind::VersionMinimumTvOS:
                case Kind::VersionMinimumWatchOS:
                case Kind::Note:
                case Kind::BuildVersion:
                case Kind::DyldExportsTrie:
                case Kind::DyldChainedFixups:
                    return true;
            }

            return false;
        }

        constexpr static inline bool KindIsSharedLibrary(Kind Kind) noexcept {
            switch (Kind) {
                case Kind::LoadDylib:
                case Kind::LoadWeakDylib:
                case Kind::ReexportDylib:
                case Kind::LazyLoadDylib:
                case Kind::LoadUpwardDylib:
                    return true;

                case Kind::Segment:
                case Kind::SymbolTable:
                case Kind::SymbolSegment:
                case Kind::Thread:
                case Kind::UnixThread:
                case Kind::LoadFixedVMSharedLibrary:
                case Kind::IdFixedVMSharedLibrary:
                case Kind::Ident:
                case Kind::FixedVMFile:
                case Kind::PrePage:
                case Kind::DynamicSymbolTable:
                case Kind::IdDylib:
                case Kind::LoadDylinker:
                case Kind::IdDylinker:
                case Kind::PreBoundDylib:
                case Kind::Routines:
                case Kind::SubFramework:
                case Kind::SubUmbrella:
                case Kind::SubClient:
                case Kind::SubLibrary:
                case Kind::TwoLevelHints:
                case Kind::PrebindChecksum:
                case Kind::Segment64:
                case Kind::Routines64:
                case Kind::Uuid:
                case Kind::Rpath:
                case Kind::CodeSignature:
                case Kind::SegmentSplitInfo:
                case Kind::EncryptionInfo:
                case Kind::DyldInfo:
                case Kind::DyldInfoOnly:
                case Kind::VersionMinimumMacOSX:
                case Kind::VersionMinimumIPhoneOS:
                case Kind::FunctionStarts:
                case Kind::DyldEnvironment:
                case Kind::Main:
                case Kind::DataInCode:
                case Kind::SourceVersion:
                case Kind::DylibCodeSignDRS:
                case Kind::EncryptionInfo64:
                case Kind::LinkerOption:
                case Kind::LinkerOptimizationHint:
                case Kind::VersionMinimumTvOS:
                case Kind::VersionMinimumWatchOS:
                case Kind::Note:
                case Kind::BuildVersion:
                case Kind::DyldExportsTrie:
                case Kind::DyldChainedFixups:
                    return false;
            }
        }

        enum class CmdSizeInvalidType {
            None,
            TooSmall,
            TooLarge
        };

        LoadCommand::CmdSizeInvalidType
        ValidateCmdsize(const LoadCommand *LoadCmd, bool IsBigEndian) noexcept;

        uint32_t Cmd;
        uint32_t CmdSize;

        inline Kind GetKind(bool IsBigEndian) const noexcept {
            return Kind(SwitchEndianIf(Cmd, IsBigEndian));
        }
    };

    enum class SegmentFlagsEnum : uint32_t {
        HighVM             = 1 << 0,
        FixedVMLibary      = 1 << 1,
        NoRelocations      = 1 << 2,
        ProectionVersion1  = 1 << 3,
        ReadOnly           = 1 << 4
    };

    enum class SegmentSectionType {
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
        TheradLocalVariables,
        ThreadLocalVariablePointers,
        ThreadLocalInitFunctionPointers,
        InitFunction32BitOffsets
    };

    template <SegmentSectionType Type>
    struct SegmentSectionTypeInfo {};

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::Regular> {
        constexpr static const auto Kind = SegmentSectionType::Regular;

        constexpr static const auto Name = std::string_view("S_REGULAR");
        constexpr static const auto Description = std::string_view("Regular");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::ZeroFillOnDemand> {
        constexpr static const auto Kind = SegmentSectionType::ZeroFillOnDemand;

        constexpr static const auto Name = std::string_view("S_ZEROFILL");
        constexpr static const auto Description = std::string_view("Zerofill");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::CStringLiterals> {
        constexpr static const auto Kind = SegmentSectionType::CStringLiterals;

        constexpr static const auto Name =
            std::string_view("S_CSTRING_LITERALS");
        constexpr static const auto Description =
            std::string_view("C-String Literals");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::FourByteLiterals> {
        constexpr static const auto Kind = SegmentSectionType::FourByteLiterals;

        constexpr static const auto Name = std::string_view("S_4BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("Four-Byte Literal");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::EightByteLiterals> {
        constexpr static const auto Kind =
            SegmentSectionType::EightByteLiterals;

        constexpr static const auto Name = std::string_view("S_8BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("Eight-Byte Literal");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::LiteralPointers> {
        constexpr static const auto Kind = SegmentSectionType::CStringLiterals;

        constexpr static const auto Name =
            std::string_view("S_LITERAL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Literal Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::NonLazySymbolPointers> {
        constexpr static const auto Kind =
            SegmentSectionType::NonLazySymbolPointers;

        constexpr static const auto Name =
            std::string_view("S_NON_LAZY_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Non-Lazy Symbol-Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::LazySymbolPointers> {
        constexpr static const auto Kind =
            SegmentSectionType::LazySymbolPointers;

        constexpr static const auto Name =
            std::string_view("S_LAZY_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Lazy Symbol-Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::SymbolStubs> {
        constexpr static const auto Kind = SegmentSectionType::CStringLiterals;

        constexpr static const auto Name = std::string_view("S_SYMBOL_STUBS");
        constexpr static const auto Description =
            std::string_view("Symbol Stubs");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::ModInitFunctionPointers> {
        constexpr static const auto Kind =
            SegmentSectionType::ModInitFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_MOD_INIT_FUNC_POINTERS");
        constexpr static const auto Description =
            std::string_view("Mod-Init Function Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::ModTermFunctionPointers> {
        constexpr static const auto Kind =
            SegmentSectionType::ModTermFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_MOD_TERM_FUNC_POINTERS");
        constexpr static const auto Description =
            std::string_view("Mod-Term Function Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::CoalescedSymbols> {
        constexpr static const auto Kind = SegmentSectionType::CoalescedSymbols;

        constexpr static const auto Name = std::string_view("S_COALESCED");
        constexpr static const auto Description =
            std::string_view("Coalesced Symbols");
    };

    template <>
    struct SegmentSectionTypeInfo<
        SegmentSectionType::ZeroFillOnDemandGigaBytes>
    {
        constexpr static const auto Kind =
            SegmentSectionType::ZeroFillOnDemandGigaBytes;

        constexpr static const auto Name = std::string_view("S_GB_ZEROFILL");
        constexpr static const auto Description =
            std::string_view("Zero-Fill (GigaBytes)");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::InterposingFunctions> {
        constexpr static const auto Kind =
            SegmentSectionType::InterposingFunctions;

        constexpr static const auto Name = std::string_view("S_INTERPOSING");
        constexpr static const auto Description =
            std::string_view("Interposing Functions");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::SixteenByteLiterals> {
        constexpr static const auto Kind =
            SegmentSectionType::SixteenByteLiterals;

        constexpr static const auto Name =
            std::string_view("S_16BYTE_LITERALS");
        constexpr static const auto Description =
            std::string_view("16-Byte Literals");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::DTraceDOF> {
        constexpr static const auto Kind = SegmentSectionType::DTraceDOF;

        constexpr static const auto Name = std::string_view("S_DTRACE_DOF");
        constexpr static const auto Description =
            std::string_view("DTrace DOFs");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::LazyDylibSymbolPointers> {
        constexpr static const auto Kind = SegmentSectionType::DTraceDOF;

        constexpr static const auto Name =
            std::string_view("S_LAZY_DYLIB_SYMBOL_POINTERS");
        constexpr static const auto Description =
            std::string_view("Lazy Dylib Lazy-Symbol Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::ThreadLocalRegular> {
        constexpr static const auto Kind =
            SegmentSectionType::ThreadLocalRegular;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_REGULAR");
        constexpr static const auto Description =
            std::string_view("Thread-Local Regular");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::ThreadLocalZeroFill> {
        constexpr static const auto Kind =
            SegmentSectionType::ThreadLocalZeroFill;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_ZEROFILL");
        constexpr static const auto Description =
            std::string_view("Thread-Local Zero-Fill");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::TheradLocalVariables> {
        constexpr static const auto Kind =
            SegmentSectionType::TheradLocalVariables;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_VARIABLES");
        constexpr static const auto Description =
            std::string_view("Thread-Local Variables");
    };

    template <>
    struct SegmentSectionTypeInfo<
        SegmentSectionType::ThreadLocalVariablePointers>
    {
        constexpr static const auto Kind =
            SegmentSectionType::ThreadLocalVariablePointers;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_VARIABLE_POINTERS");
        constexpr static const auto Description =
            std::string_view("Thread-Local Variable-Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<
        SegmentSectionType::ThreadLocalInitFunctionPointers>
    {
        constexpr static const auto Kind =
            SegmentSectionType::ThreadLocalInitFunctionPointers;

        constexpr static const auto Name =
            std::string_view("S_THREAD_LOCAL_INIT_FUNCTION_POINTERS");
        constexpr static const auto Description =
            std::string_view("Thread-Local Init-Function Pointers");
    };

    template <>
    struct SegmentSectionTypeInfo<SegmentSectionType::InitFunction32BitOffsets>
    {
        constexpr static const auto Kind =
            SegmentSectionType::InitFunction32BitOffsets;

        constexpr static const auto Name =
            std::string_view("S_INIT_FUNC_OFFSETS");
        constexpr static const auto Description =
            std::string_view("32-Bit Init Function Offsets");
    };

    constexpr static const auto EmptyStringValue = std::string_view();

    constexpr std::string_view
    SegmentSectionTypeGetName(SegmentSectionType Type) noexcept {
        switch (Type) {
            case SegmentSectionType::Regular:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::Regular>::Name;
            case SegmentSectionType::ZeroFillOnDemand:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ZeroFillOnDemand>::Name;
            case SegmentSectionType::CStringLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::CStringLiterals>::Name;
            case SegmentSectionType::FourByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::FourByteLiterals>::Name;
            case SegmentSectionType::EightByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::EightByteLiterals>::Name;
            case SegmentSectionType::LiteralPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LiteralPointers>::Name;
            case SegmentSectionType::NonLazySymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::NonLazySymbolPointers>::Name;
            case SegmentSectionType::LazySymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LazySymbolPointers>::Name;
            case SegmentSectionType::SymbolStubs:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::SymbolStubs>::Name;
            case SegmentSectionType::ModInitFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ModInitFunctionPointers>::Name;
            case SegmentSectionType::ModTermFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ModTermFunctionPointers>::Name;
            case SegmentSectionType::CoalescedSymbols:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::CoalescedSymbols>::Name;
            case SegmentSectionType::ZeroFillOnDemandGigaBytes:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ZeroFillOnDemandGigaBytes>::Name;
            case SegmentSectionType::InterposingFunctions:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::InterposingFunctions>::Name;
            case SegmentSectionType::SixteenByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::SixteenByteLiterals>::Name;
            case SegmentSectionType::DTraceDOF:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::DTraceDOF>::Name;
            case SegmentSectionType::LazyDylibSymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LazyDylibSymbolPointers>::Name;
            case SegmentSectionType::ThreadLocalRegular:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalRegular>::Name;
            case SegmentSectionType::ThreadLocalZeroFill:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalZeroFill>::Name;
            case SegmentSectionType::TheradLocalVariables:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::TheradLocalVariables>::Name;
            case SegmentSectionType::ThreadLocalVariablePointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalVariablePointers>::Name;
            case SegmentSectionType::ThreadLocalInitFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalInitFunctionPointers>::Name;
            case SegmentSectionType::InitFunction32BitOffsets:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::InitFunction32BitOffsets>::Name;
        }

        return std::string_view();
    }

    constexpr std::string_view
    SegmentSectionTypeGetDescription(SegmentSectionType Type) noexcept {
        switch (Type) {
            case SegmentSectionType::Regular:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::Regular>::Description;
            case SegmentSectionType::ZeroFillOnDemand:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ZeroFillOnDemand>::Description;
            case SegmentSectionType::CStringLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::CStringLiterals>::Description;
            case SegmentSectionType::FourByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::FourByteLiterals>::Description;
            case SegmentSectionType::EightByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::EightByteLiterals>::Description;
            case SegmentSectionType::LiteralPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LiteralPointers>::Description;
            case SegmentSectionType::NonLazySymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::NonLazySymbolPointers>::Description;
            case SegmentSectionType::LazySymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LazySymbolPointers>::Description;
            case SegmentSectionType::SymbolStubs:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::SymbolStubs>::Description;
            case SegmentSectionType::ModInitFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ModInitFunctionPointers>::Description;
            case SegmentSectionType::ModTermFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ModTermFunctionPointers>::Description;
            case SegmentSectionType::CoalescedSymbols:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::CoalescedSymbols>::Description;
            case SegmentSectionType::ZeroFillOnDemandGigaBytes:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ZeroFillOnDemandGigaBytes>::Description;
            case SegmentSectionType::InterposingFunctions:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::InterposingFunctions>::Description;
            case SegmentSectionType::SixteenByteLiterals:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::SixteenByteLiterals>::Description;
            case SegmentSectionType::DTraceDOF:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::DTraceDOF>::Description;
            case SegmentSectionType::LazyDylibSymbolPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::LazyDylibSymbolPointers>::Description;
            case SegmentSectionType::ThreadLocalRegular:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalRegular>::Description;
            case SegmentSectionType::ThreadLocalZeroFill:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalZeroFill>::Description;
            case SegmentSectionType::TheradLocalVariables:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::TheradLocalVariables>::Description;
            case SegmentSectionType::ThreadLocalVariablePointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalVariablePointers>
                        ::Description;
            case SegmentSectionType::ThreadLocalInitFunctionPointers:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::ThreadLocalInitFunctionPointers>
                        ::Description;
            case SegmentSectionType::InitFunction32BitOffsets:
                return SegmentSectionTypeInfo<
                    SegmentSectionType::InitFunction32BitOffsets>::Description;
        }

        return std::string_view();
    }

    static const auto SegmentSectionUserSettableAttributesMask =
        static_cast<uint32_t>(0xff000000);

    static const auto SegmentSectionSystemSettableAttributesMask =
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
        public BasicMasksHandlerBigEndian<SegmentSectionAttributesMasks>
    {
    protected:
        uint32_t Flags;
    public:
        using Masks = SegmentSectionAttributesMasks;

        explicit SegmentSectionAttributes(uint32_t Flags) noexcept;
        inline bool IsAllInstructions(bool IsBigEndian) const noexcept {
            return Has(Masks::IsAllInstructions, IsBigEndian);
        }

        inline bool NoRanlibTableOfContents(bool IsBigEndian) const noexcept {
            const auto NoRanlibTableOfContents =
                Has(Masks::NoRanlibTableOfContents, IsBigEndian);

            return NoRanlibTableOfContents;
        }

        inline bool StripStaticSymbols(bool IsBigEndian) const noexcept {
            return Has(Masks::StripStaticSymbols, IsBigEndian);
        }

        inline bool NoDeadStripping(bool IsBigEndian) const noexcept {
            return Has(Masks::NoDeadStripping, IsBigEndian);
        }

        inline bool LiveSupport(bool IsBigEndian) const noexcept {
            return Has(Masks::LiveSupport, IsBigEndian);
        }

        inline bool SelfModifyingCode(bool IsBigEndian) const noexcept {
            return Has(Masks::SelfModifyingCode, IsBigEndian);
        }

        inline bool IsDebugSection(bool IsBigEndian) const noexcept {
            return Has(Masks::IsDebugSection, IsBigEndian);
        }

        inline bool HasSomeInstructions(bool IsBigEndian) const noexcept {
            return Has(Masks::HasSomeInstructions, IsBigEndian);
        }

        inline bool HasExternalRelocEntries(bool IsBigEndian) const noexcept {
            return Has(Masks::HasExternalRelocEntries, IsBigEndian);
        }

        inline bool HasLocalRelocEntries(bool IsBigEndian) const noexcept {
            return Has(Masks::HasLocalRelocEntries, IsBigEndian);
        }
    };

    enum class SegmentSectionMasks : uint32_t {
        Attributes = 0xffffff00,
        Type = 0x000000ff,
    };

    struct SegmentSectionFlags :
        private BasicMasksHandlerBigEndian<SegmentSectionMasks>
    {
        using Type = SegmentSectionType;
        using Attributes = SegmentSectionAttributes;

        Type GetType(bool IsBigEndian) const noexcept;
        Attributes GetAttributes(bool IsBigEndian) const noexcept;
    };

    struct SegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Section {
            using SectionFlags = SegmentSectionFlags;

            char Name[16];
            char SegName[16];
            uint32_t Addr;
            uint32_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            SectionFlags Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;
        };

        using FlagsEnum = SegmentFlagsEnum;
        using SegmentFlags = BasicFlags<FlagsEnum>;
        using SectionList = BasicContiguousList<Section>;
        using ConstSectionList = BasicContiguousList<const Section>;

        char Name[16];
        uint32_t VmAddr;
        uint32_t VmSize;
        uint32_t FileOff;
        uint32_t FileSize;
        int32_t MaxProt;
        int32_t InitProt;
        uint32_t Nsects;
        SegmentFlags Flags;

        [[nodiscard]] SectionList GetSectionList(bool IsBigEndian) noexcept;

        [[nodiscard]]
        ConstSectionList GetConstSectionList(bool IsBigEndian) const noexcept;
    };

    struct SegmentCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Segment64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SegmentCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Section {
            using SectionFlags = SegmentSectionFlags;

            char Name[16];
            char SegName[16];
            uint64_t Addr;
            uint64_t Size;
            uint32_t Offset;
            uint32_t Align;
            uint32_t Reloff;
            uint32_t Nreloc;
            SectionFlags Flags;
            uint32_t Reserved1;
            uint32_t Reserved2;
        };

        using FlagsEnum = SegmentFlagsEnum;
        using SegmentFlags = BasicFlags<FlagsEnum>;
        using SectionList = BasicContiguousList<Section>;
        using ConstSectionList = BasicContiguousList<const Section>;

        char Name[16];
        uint64_t VmAddr;
        uint64_t VmSize;
        uint64_t FileOff;
        uint64_t FileSize;
        int32_t MaxProt;
        int32_t InitProt;
        uint32_t Nsects;
        SegmentFlags Flags;

        [[nodiscard]] SectionList GetSectionList(bool IsBigEndian) noexcept;

        [[nodiscard]]
        ConstSectionList GetConstSectionList(bool IsBigEndian) const noexcept;
    };

    template <typename Error>
    struct LoadCommandStringViewOrError {
        static_assert(TypeTraits::IsEnumClassValue<Error>,
                      "Error-Type is not an enum class");
    protected:
        std::string_view View;

        inline uintptr_t GetStorage() const noexcept {
            return reinterpret_cast<uintptr_t>(View.begin());
        }
    public:
        LoadCommandStringViewOrError(const std::string_view &View) noexcept
        : View(View) {}

        LoadCommandStringViewOrError(Error Value) noexcept {
            View = std::string_view(reinterpret_cast<const char *>(Value), 1);
        }

        inline bool HasError() const noexcept {
            const auto HasError =
                PointerErrorStorage<Error>::PointerHasErrorValue(GetStorage());

            return HasError;
        }

        inline Error GetError() const noexcept {
            if (!HasError()) {
                return Error::None;
            }

            return Error(GetStorage());
        }

        inline const std::string_view &GetString() const noexcept {
            assert(!HasError());
            return View;
        }
    };

    union LoadCommandString {
        uint32_t Offset;
    #ifndef __LP64__
        char *Ptr;
    #endif

        enum class GetStringError {
            None,
            OffsetOutOfBounds,
            NoNullTerminator,
            EmptyString,
        };

        bool
        IsOffsetValid(uint32_t MinSize,
                      uint32_t CmdSize,
                      bool IsBigEndian) const noexcept;

        uint32_t GetLength(uint32_t CmdSize, bool IsBigEndian) const noexcept;
        using GetValueResult = LoadCommandStringViewOrError<GetStringError>;
    };

    enum class PackedVersionMasks : uint32_t {
        Revision = 0xff,
        Minor = 0xff00,
        Major = 0xffff0000
    };

    enum class PackedVersionShifts : uint32_t {
        Revision,
        Minor = 8,
        Major = 16
    };

    struct PackedVersion :
        private BasicMasksAndShifts<PackedVersionMasks, PackedVersionShifts>
    {
    public:
        using Masks = PackedVersionMasks;
        using Shifts = PackedVersionShifts;

        inline uint8_t GetRevision(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Revision, Shifts::Revision, IsBE);
        }

        inline uint8_t GetMinor(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Minor, Shifts::Minor, IsBE);
        }

        inline uint16_t GetMajor(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Major, Shifts::Major, IsBE);
        }
    };

    struct FixedVmSharedLibraryInfo {
        LoadCommandString Name;
        PackedVersion MinorVersion;
        uint32_t HeaderAddr;
    };

    struct FixedVMSharedLibraryCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdFixedVMSharedLibrary) ||
                (Kind == LoadCommand::Kind::LoadFixedVMSharedLibrary);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMSharedLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        FixedVmSharedLibraryInfo Info;

        [[nodiscard]] bool IsNameOffsetValid(bool IsBigEndian) const noexcept;
        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct DylibCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::LoadDylib) ||
                (Kind == LoadCommand::Kind::IdDylib) ||
                (Kind == LoadCommand::Kind::LoadWeakDylib) ||
                (Kind == LoadCommand::Kind::ReexportDylib) ||
                (Kind == LoadCommand::Kind::LazyLoadDylib) ||
                (Kind == LoadCommand::Kind::LoadUpwardDylib);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylibCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Info {
            LoadCommandString Name;

            uint32_t Timestamp;
            PackedVersion CurrentVersion;
            PackedVersion CompatibilityVersion;
        };

        Info Info;

        [[nodiscard]] bool IsNameOffsetValid(bool IsBigEndian) const noexcept;
        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct SubFrameworkCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubFramework);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubFrameworkCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        bool IsUmbrellaOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubClientCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubClient);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubClientCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Client;

        [[nodiscard]]
        bool IsClientOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetClient(bool IsBigEndian) const noexcept;
    };

    struct SubUmbrellaCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubUmbrella);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubUmbrellaCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Umbrella;

        [[nodiscard]]
        bool IsUmbrellaOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetUmbrella(bool IsBigEndian) const noexcept;
    };

    struct SubLibraryCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SubLibrary);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SubLibraryCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Library;

        [[nodiscard]]
        bool IsLibraryOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLibrary(bool IsBigEndian) const noexcept;
    };

    struct PreBoundDylibCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PreBoundDylib);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PreBoundDylibCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;
        uint32_t NModules;
        LoadCommandString LinkedModules;

        [[nodiscard]] bool IsNameOffsetValid(bool IsBigEndian) const noexcept;
        [[nodiscard]]
        bool IsLinkedModulesOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetLinkedModules(bool IsBigEndian) const noexcept;
    };

    struct DylinkerCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::IdDylinker) ||
                (Kind == LoadCommand::Kind::LoadDylinker) ||
                (Kind == LoadCommand::Kind::DyldEnvironment);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DylinkerCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;

        [[nodiscard]]
        bool IsNameOffsetValid(bool IsBigEndian) const noexcept;

        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct ThreadCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::Thread) ||
                (Kind == LoadCommand::Kind::UnixThread);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(ThreadCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }
    };

    struct RoutinesCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t InitAddress;
        uint32_t InitRoutine;

        uint32_t Reserved1;
        uint32_t Reserved2;
        uint32_t Reserved3;
        uint32_t Reserved4;
        uint32_t Reserved5;
        uint32_t Reserved6;
    };

    struct RoutinesCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Routines64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(RoutinesCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint64_t InitAddress;
        uint64_t InitRoutine;

        uint64_t Reserved1;
        uint64_t Reserved2;
        uint64_t Reserved3;
        uint64_t Reserved4;
        uint64_t Reserved5;
        uint64_t Reserved6;
    };

    enum class SymbolTableEntryInfoMasks : uint8_t {
        IsExternal = 0x1,
        SymbolTypeMask = 0xe,
        IsPrivateExternal = 0x10,
        IsDebugSymbolMask = 0xe0
    };

    using SymbolTableEntryInfoMasksHandler =
        BasicMasksHandler<SymbolTableEntryInfoMasks>;

    struct SymbolTableEntryInfo : private SymbolTableEntryInfoMasksHandler {
        enum class SymbolType : uint8_t {
            Undefined,
            Absolute          = 2,
            Indirect          = 10,
            PreboundUndefined = 12,
            Section           = 14,
        };

        using Masks = SymbolTableEntryInfoMasks;

        [[nodiscard]] SymbolType GetSymbolType() const noexcept;
        [[nodiscard]] inline bool IsExternal() const noexcept {
            return Has(Masks::IsExternal);
        }

        [[nodiscard]] inline bool IsPrivateExternal() const noexcept {
            return Has(Masks::IsExternal);
        }

        [[nodiscard]] inline bool IsDebugSymbol() const noexcept {
            return Has(Masks::IsExternal);
        }

    };

    struct SymbolTableEntry32 {
        union {
    #ifndef __LP64__
            char *Name;
    #endif
            uint32_t Index;
        };

        SymbolTableEntryInfo Info;
        uint8_t Section;
        int16_t Desc;
        uint32_t Value;
    };

    struct SymbolTableEntry64 {
        uint32_t Index;
        SymbolTableEntryInfo Info;
        uint8_t Section;
        uint16_t Desc;
        uint64_t Value;
    };

    enum class SizeRangeError {
        None,
        Overflows,
        PastEnd,
    };

    struct SymtabCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymtabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SymtabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t SymOff;
        uint32_t Nsyms;
        uint32_t StrOff;
        uint32_t StrSize;

        using Entry32 = SymbolTableEntry32;
        using Entry64 = SymbolTableEntry64;

        using Entry32List = BasicContiguousList<Entry32>;
        using Entry64List = BasicContiguousList<Entry64>;

        using ConstEntry32List = BasicContiguousList<const Entry32>;
        using ConstEntry64List = BasicContiguousList<const Entry64>;

        [[nodiscard]] TypedAllocationOrError<Entry32List *, SizeRangeError>
        GetEntry32List(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List *, SizeRangeError>
        GetEntry64List(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List *, SizeRangeError>
        GetConstEntry32List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List *, SizeRangeError>
        GetConstEntry64List(const ConstMemoryMap &Map,
                            bool IsBigEndian) const noexcept;
    };

    struct DynamicSymtabCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::DynamicSymbolTable);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DynamicSymtabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(DynamicSymtabCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t ILocalSymbols;
        uint32_t NLocalSymbols;

        uint32_t IExternallyDefinedSymbols;
        uint32_t NExternallyDefinedSymbols;

        uint32_t IUndefinedSymbols;
        uint32_t NUndefinedSymbols;

        uint32_t TableOfContentsOff;
        uint32_t NTableOfContentsEntries;

        uint32_t ModuleTableOff;
        uint32_t NModuleTableEntries;

        uint32_t ExternalReferenceSymbolTableoff;
        uint32_t NExtReferencedSymbols;

        uint32_t IndirectSymbolTableOff;
        uint32_t NIndirectSymbols;

        uint32_t ExternalRelocationsOff;
        uint32_t NExternalRelocations;

        uint32_t LocalRelocationsOff;
        uint32_t NLocalRelocations;

        using Entry32 = SymbolTableEntry32;
        using Entry64 = SymbolTableEntry64;

        using Entry32List = BasicContiguousList<Entry32>;
        using Entry64List = BasicContiguousList<Entry64>;

        using ConstEntry32List = BasicContiguousList<const Entry32>;
        using ConstEntry64List = BasicContiguousList<const Entry64>;

        [[nodiscard]] TypedAllocationOrError<Entry32List *, SizeRangeError>
        GetExport32List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List *, SizeRangeError>
        GetExport64List(const MemoryMap &Map,
                        uint32_t SymOff,
                        bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry32List *, SizeRangeError>
        GetLocalSymbol32List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<Entry64List *, SizeRangeError>
        GetLocalSymbol64List(const MemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List *, SizeRangeError>
        GetConstExport32List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List *, SizeRangeError>
        GetConstExport64List(const ConstMemoryMap &Map,
                             uint32_t SymOff,
                             bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry32List *, SizeRangeError>
        GetConstLocalSymbol32List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstEntry64List *, SizeRangeError>
        GetConstLocalSymbol64List(const ConstMemoryMap &Map,
                                  uint32_t SymOff,
                                  bool IsBigEndian) const noexcept;
    };

    struct TwoLevelHintsCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::TwoLevelHints);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(TwoLevelHintsCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        struct Hint {
            uint32_t SubImagesIndex       : 8,
                     TableOfContentsIndex : 24;
        };

        using HintList = BasicContiguousList<Hint>;
        using ConstHintList = BasicContiguousList<const Hint>;

        uint32_t Offset;
        uint32_t NHints;

        [[nodiscard]] TypedAllocationOrError<HintList *, SizeRangeError>
        GetHintList(const MemoryMap &Map, bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstHintList *, SizeRangeError>
        GetConstHintList(const ConstMemoryMap &Map,
                         bool IsBigEndian) const noexcept;
    };

    struct PrebindChecksumCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::PrebindChecksum);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(PrebindChecksumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CheckSum;
    };

    struct UuidCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Uuid);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(UuidCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint8_t uuid[16];
    };

    struct RpathCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Rpath);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(RpathCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Path;

        [[nodiscard]] bool IsPathOffsetValid(bool IsBigEndian) const noexcept;
        [[nodiscard]] LoadCommandString::GetValueResult
        GetPath(bool IsBigEndian) const noexcept;
    };

    struct LinkeditDataCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::CodeSignature) ||
                (Kind == LoadCommand::Kind::SegmentSplitInfo) ||
                (Kind == LoadCommand::Kind::DataInCode) ||
                (Kind == LoadCommand::Kind::FunctionStarts) ||
                (Kind == LoadCommand::Kind::DylibCodeSignDRS) ||
                (Kind == LoadCommand::Kind::LinkerOptimizationHint) ||
                (Kind == LoadCommand::Kind::DyldExportsTrie) ||
                (Kind == LoadCommand::Kind::DyldChainedFixups);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(LinkeditDataCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t DataOff;
        uint32_t DataSize;
    };

    struct EncryptionInfoCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
    };

    struct EncryptionInfoCommand64 : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::EncryptionInfo64);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EncryptionInfoCommand64)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t CryptOff;
        uint32_t CryptSize;
        uint32_t CryptId;
        uint32_t Pad;
    };

    struct VersionMinimumCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::VersionMinimumMacOSX) ||
                (Kind == LoadCommand::Kind::VersionMinimumIPhoneOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumTvOS) ||
                (Kind == LoadCommand::Kind::VersionMinimumWatchOS);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(VersionMinimumCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        PackedVersion Version;
        PackedVersion Sdk;
    };

    struct BuildVersionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::BuildVersion);
        }

        struct Tool {
            enum class Kind {
                Clang = 1,
                Swift,
                Ld,
            };

            template <Kind Kind>
            struct KindInfo {};

            template <>
            struct KindInfo<Kind::Clang> {
                constexpr static const auto Kind = Kind::Clang;

                constexpr static const auto Name =
                    std::string_view("TOOL_CLANG");
                constexpr static const auto Description =
                    std::string_view("Clang");
            };

            template <>
            struct KindInfo<Kind::Swift> {
                constexpr static const auto Kind = Kind::Swift;

                constexpr static const auto Name =
                    std::string_view("TOOL_SWIFT");
                constexpr static const auto Description =
                    std::string_view("Swift");
            };

            template <>
            struct KindInfo<Kind::Ld> {
                constexpr static const auto Kind = Kind::Clang;
                constexpr static const auto Name =
                    std::string_view("TOOL_LD");
                constexpr static const auto Description =
                    std::string_view("ld");
            };

            uint32_t Kind;
            PackedVersion Version;

            enum Kind GetKind(bool IsBigEndian) const noexcept;

            constexpr static const std::string_view &
            KindGetName(enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return KindInfo<Kind::Clang>::Name;
                    case Kind::Swift:
                        return KindInfo<Kind::Swift>::Name;
                    case Kind::Ld:
                        return KindInfo<Kind::Ld>::Name;
                }

                return EmptyStringValue;
            }

            constexpr static const std::string_view &
            KindGetDescription(enum Kind Kind) noexcept {
                switch (Kind) {
                    case Kind::Clang:
                        return KindInfo<Kind::Clang>::Description;
                    case Kind::Swift:
                        return KindInfo<Kind::Swift>::Description;
                    case Kind::Ld:
                        return KindInfo<Kind::Ld>::Description;
                }

                return EmptyStringValue;
            }
        };

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            const auto InvalidType =
                HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));

            if (InvalidType != LoadCommand::CmdSizeInvalidType::None) {
                return InvalidType;
            }

            auto ExpectedSize = uint32_t();
            if (DoesMultiplyAndAddOverflow(sizeof(Tool), NTools, sizeof(*this),
                                           &ExpectedSize))
            {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > ExpectedSize) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            } else if (CmdSize < ExpectedSize) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(BuildVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        enum class PlatformKind : uint32_t {
            macOS = 1,
            iOS,
            tvOS,
            watchOS,
            bridgeOS,
            iOSMac,
            macCatalyst = iOSMac,
            iOSSimulator,
            tvOSSimulator,
            watchOSSimulator,
            DriverKit
        };

        template <PlatformKind>
        struct PlatformKindInfo {};

        template <>
        struct PlatformKindInfo<PlatformKind::macOS> {
            constexpr static const auto Kind = PlatformKind::macOS;

            constexpr static const auto Name =
                std::string_view("PLATFORM_MACOS");
            constexpr static const auto Description = std::string_view("macOS");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::iOS> {
            constexpr static const auto Kind = PlatformKind::iOS;

            constexpr static const auto Name = std::string_view("PLATFORM_IOS");
            constexpr static const auto Description = std::string_view("iOS");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::tvOS> {
            constexpr static const auto Kind = PlatformKind::tvOS;

            constexpr static const auto Name =
                std::string_view("PLATFORM_TVOS");
            constexpr static const auto Description = std::string_view("tvOS");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::watchOS> {
            constexpr static const auto Kind = PlatformKind::watchOS;

            constexpr static const auto Name =
                std::string_view("PLATFORM_WATCHOS");
            constexpr static const auto Description =
                std::string_view("watchOS");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::bridgeOS> {
            constexpr static const auto Kind = PlatformKind::bridgeOS;

            constexpr static const auto Name =
                std::string_view("PLATFORM_BRDIGEOS");
            constexpr static const auto Description =
                std::string_view("bridgeOS");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::iOSMac> {
            constexpr static const auto Kind = PlatformKind::iOSMac;

            constexpr static const auto Name =
                std::string_view("PLATFORM_IOSMAC");
            constexpr static const auto Description =
                std::string_view("iOSMac");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::iOSSimulator> {
            constexpr static const auto Kind = PlatformKind::iOSSimulator;

            constexpr static const auto Name =
                std::string_view("PLATFORM_IOSSIMULATOR");
            constexpr static const auto Description =
                std::string_view("iOS Simulator");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::tvOSSimulator> {
            constexpr static const auto Kind = PlatformKind::tvOSSimulator;

            constexpr static const auto Name =
                std::string_view("PLATFORM_TVOSSIMULATOR");
            constexpr static const auto Description =
                std::string_view("tvOS Simulator");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::watchOSSimulator> {
            constexpr static const auto Kind = PlatformKind::watchOSSimulator;

            constexpr static const auto Name =
                std::string_view("PLATFORM_WATCHOSSIMULATOR");
            constexpr static const auto Description =
                std::string_view("watchOS Simulator");
        };

        template <>
        struct PlatformKindInfo<PlatformKind::DriverKit> {
            constexpr static const auto Kind = PlatformKind::DriverKit;

            constexpr static const auto Name =
                std::string_view("PLATFORM_DRIVERKIT");
            constexpr static const auto Description =
                std::string_view("DriverKit");
        };

        constexpr static const std::string_view &
        PlatformKindGetName(PlatformKind Kind) noexcept {
            switch (Kind) {
                case PlatformKind::macOS:
                    return PlatformKindInfo<PlatformKind::macOS>::Name;
                case PlatformKind::iOS:
                    return PlatformKindInfo<PlatformKind::iOS>::Name;
                case PlatformKind::tvOS:
                    return PlatformKindInfo<PlatformKind::tvOS>::Name;
                case PlatformKind::watchOS:
                    return PlatformKindInfo<PlatformKind::watchOS>::Name;
                case PlatformKind::bridgeOS:
                    return PlatformKindInfo<PlatformKind::bridgeOS>::Name;
                case PlatformKind::iOSMac:
                    return PlatformKindInfo<PlatformKind::iOSMac>::Name;
                case PlatformKind::iOSSimulator:
                    return PlatformKindInfo<PlatformKind::iOSSimulator>::Name;
                case PlatformKind::tvOSSimulator:
                    return PlatformKindInfo<PlatformKind::tvOSSimulator>::Name;
                case PlatformKind::watchOSSimulator:
                    return
                        PlatformKindInfo<PlatformKind::watchOSSimulator>::Name;
                case PlatformKind::DriverKit:
                    return PlatformKindInfo<PlatformKind::DriverKit>::Name;
            }

            return EmptyStringValue;
        }

        static const std::string_view &
        PlatformKindGetDescription(PlatformKind Kind) noexcept {
            switch (Kind) {
                case PlatformKind::macOS:
                    return PlatformKindInfo<PlatformKind::macOS>::Description;
                case PlatformKind::iOS:
                    return PlatformKindInfo<PlatformKind::iOS>::Description;
                case PlatformKind::tvOS:
                    return PlatformKindInfo<PlatformKind::tvOS>::Description;
                case PlatformKind::watchOS:
                    return PlatformKindInfo<PlatformKind::watchOS>::Description;
                case PlatformKind::bridgeOS:
                    return
                        PlatformKindInfo<PlatformKind::bridgeOS>::Description;
                case PlatformKind::iOSMac:
                    return PlatformKindInfo<PlatformKind::iOSMac>::Description;
                case PlatformKind::iOSSimulator:
                    return
                        PlatformKindInfo<PlatformKind::iOSSimulator>
                            ::Description;
                case PlatformKind::tvOSSimulator:
                    return
                        PlatformKindInfo<PlatformKind::tvOSSimulator>
                        ::Description;
                case PlatformKind::watchOSSimulator:
                    return
                        PlatformKindInfo<PlatformKind::watchOSSimulator>
                            ::Description;
                case PlatformKind::DriverKit:
                    return
                        PlatformKindInfo<PlatformKind::DriverKit>::Description;
            }

            return EmptyStringValue;
        }

        uint32_t Platform;

        PackedVersion MinOS;
        PackedVersion Sdk;

        uint32_t NTools;
        PlatformKind GetPlatform(bool IsBigEndian) const noexcept;

        using ToolList = BasicContiguousList<Tool>;
        using ConstToolList = BasicContiguousList<const Tool>;

        [[nodiscard]] TypedAllocationOrError<ToolList *, SizeRangeError>
        GetToolList(bool IsBigEndian) noexcept;

        [[nodiscard]] TypedAllocationOrError<ConstToolList *, SizeRangeError>
        GetConstToolList(bool IsBigEndian) const noexcept;
    };

    enum class DyldInfoByteMasks : uint8_t {
        Opcode    = 0xF0,
        Immediate = 0x0F
    };

    using DyldInfoByteMasksHandler =
        BasicMasksHandler<DyldInfoByteMasks, uint32_t>;

    enum class BindByteOpcode : uint8_t {
        Done,
        SetDylibOrdinalImm,
        SetDylibOrdinalULEB,
        SetDylibSpecialImm,
        SetSymbolTrailingFlagsImm,
        SetTypeImm,
        SetAddendSleb,
        SetSegmentAndOffsetUleb,
        AddAddrUleb,
        DoBind,
        DOBindAddAddrUleb,
        DOBindAddAddrImmScaled,
        DOBindUlebTimesSkippingUleb,
        Threaded
    };

    enum class BindByteType : uint8_t {
        Pointer = 1,
        TextAbsolute32,
        TextPcrel32
    };

    enum class BindSpecialImm : uint8_t {
        DylibSelf = 0,
        DylibMainExecutable = static_cast<uint8_t>(-1),
        DylibFlatLookup = static_cast<uint8_t>(-2),
        DylibWeakLookup = static_cast<uint8_t>(-3),
    };

    enum class BindSymbolFlags : uint8_t {
        WeakImport = 1,
        NonWeakDefinition = 8
    };

    struct BindByte : private DyldInfoByteMasksHandler {
    public:
        explicit BindByte(uint8_t Byte) noexcept;

        using Masks = DyldInfoByteMasks;
        using Opcode = BindByteOpcode;
        using Type = BindByteType;

        [[nodiscard]] Opcode GetOpcode() const noexcept;
        [[nodiscard]] uint8_t GetImmediate() const noexcept;
    };

    enum class RebaseByteOpcode : uint8_t {
        Done,
        SetTypeImmediate          = 1 << 1,
        SetSegmentAndOffsetUleb   = 2 << 1,
        AddAddrUleb               = 3 << 1,
        AddAddrImmediateScaled    = 4 << 1,
        DoRebaseImmTimes          = 5 << 1,
        DoRebaseUlebTimes         = 6 << 1,
        DoRebaseAddAddrUleb       = 7 << 1,
        DoRebaseUlebTimesSkipUleb = 8 << 1
    };

    enum class RebaseByteType : uint8_t {
        None,
        Pointer,
        TextAbsolute32,
        TextPcrel32
    };

    struct RebaseByte : private DyldInfoByteMasksHandler {
    public:
        explicit RebaseByte(uint8_t Byte) noexcept;

        using Masks = DyldInfoByteMasks;
        using Opcode = RebaseByteOpcode;
        using Type = RebaseByteType;

        [[nodiscard]] Opcode GetOpcode() const noexcept;
        [[nodiscard]] uint8_t GetImmediate() const noexcept;
    };

    enum class ExportSymbolMasks : uint8_t {
        KindMask        = 0x3,
        WeakDefinition  = 0x4,
        Reexport        = 0x8,
        StubAndResolver = 0x10
    };

    using ExportSymbolMasksHandler =
        BasicMasksHandler<ExportSymbolMasks, uint32_t>;

    enum class ExportSymbolKind : uint8_t {
        Regular,
        ThreadLocal,
        Absolute
    };

    struct ExportTrieFlagsByte : private ExportSymbolMasksHandler {
    public:
        constexpr explicit ExportTrieFlagsByte() noexcept = default;
        constexpr explicit ExportTrieFlagsByte(uint8_t Byte) noexcept
        : ExportSymbolMasksHandler(Byte) {}

        using Masks = ExportSymbolMasks;
        using Kind = ExportSymbolKind;

        [[nodiscard]] constexpr inline Kind GetKind() const noexcept {
            return Kind(GetValueForMask(Masks::KindMask));
        }

        [[nodiscard]] uint8_t GetImmediate() const noexcept;

        [[nodiscard]] inline bool IsWeak() const noexcept {
            return Has(ExportSymbolMasks::WeakDefinition);
        }

        [[nodiscard]] inline bool IsReexport() const noexcept {
            return Has(ExportSymbolMasks::Reexport);
        }

        [[nodiscard]] inline bool IsStubAndResolver() const noexcept {
            return Has(ExportSymbolMasks::StubAndResolver);
        }

        inline void Clear() noexcept {
            this->Integer = 0;
        }
    };

    enum class ExportTrieSymbolType {
        Regular,
        Absolute,
        Reexport,
        WeakDefinition,
        StubAndResolver,
        ThreadLocal
    };

    template <ExportTrieSymbolType>
    struct ExportTrieSymbolTypeInfo {};

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::Regular> {
        constexpr static const auto Kind = ExportTrieSymbolType::Regular;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_REGULAR");
        constexpr static const auto Description =
            std::string_view("Regular");
    };

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::Absolute> {
        constexpr static const auto Kind = ExportTrieSymbolType::Absolute;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE");
        constexpr static const auto Description =
            std::string_view("Absolute");
    };

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::Reexport> {
        constexpr static const auto Kind = ExportTrieSymbolType::Reexport;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_REEXPORT");
        constexpr static const auto Description =
            std::string_view("Re-export");
    };

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::WeakDefinition> {
        constexpr static const auto Kind = ExportTrieSymbolType::WeakDefinition;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION");
        constexpr static const auto Description =
            std::string_view("Weak-Definition");
    };

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::StubAndResolver> {
        constexpr static const auto Kind =
            ExportTrieSymbolType::StubAndResolver;

        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER");
        constexpr static const auto Description =
            std::string_view("Stub-Resolver");
    };

    template <>
    struct ExportTrieSymbolTypeInfo<ExportTrieSymbolType::ThreadLocal> {
        constexpr static const auto Kind = ExportTrieSymbolType::ThreadLocal;
        constexpr static const auto Name =
            std::string_view("EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL");
        constexpr static const auto Description =
            std::string_view("Thread-Local");
    };

    constexpr static const std::string_view &
    ExportTrieSymbolTypeGetName(ExportTrieSymbolType Type) {
        switch (Type) {
            case ExportTrieSymbolType::Regular:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Regular>::Name;
            case ExportTrieSymbolType::Absolute:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Absolute>::Name;
            case ExportTrieSymbolType::Reexport:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Reexport>::Name;
            case ExportTrieSymbolType::WeakDefinition:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::WeakDefinition>::Name;
            case ExportTrieSymbolType::StubAndResolver:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::StubAndResolver>::Name;
            case ExportTrieSymbolType::ThreadLocal:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::ThreadLocal>::Name;
        }
    }

    constexpr static const std::string_view &
    ExportTrieSymbolTypeGetDescription(ExportTrieSymbolType Type) {
        switch (Type) {
            case ExportTrieSymbolType::Regular:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Regular>::Description;
            case ExportTrieSymbolType::Absolute:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Absolute>::Description;
            case ExportTrieSymbolType::Reexport:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::Reexport>::Description;
            case ExportTrieSymbolType::WeakDefinition:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::WeakDefinition>::Description;
            case ExportTrieSymbolType::StubAndResolver:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::StubAndResolver>::Description;
            case ExportTrieSymbolType::ThreadLocal:
                return
                    ExportTrieSymbolTypeInfo<
                        ExportTrieSymbolType::ThreadLocal>::Description;
        }
    }

    constexpr static
    const uint64_t ExportTrieSymbolTypeGetLongestNameLength() noexcept {
        constexpr const auto Type = ExportTrieSymbolType::Regular;
        auto LongestLength = LargestIntHelper<uint64_t>();

        switch (Type) {
            case ExportTrieSymbolType::Regular:
                LongestLength =
                    ExportTrieSymbolTypeGetName(ExportTrieSymbolType::Regular)
                        .length();
            case ExportTrieSymbolType::WeakDefinition:
                LongestLength =
                    ExportTrieSymbolTypeGetName(
                        ExportTrieSymbolType::WeakDefinition).length();
            case ExportTrieSymbolType::ThreadLocal:
                LongestLength =
                    ExportTrieSymbolTypeGetName(
                        ExportTrieSymbolType::ThreadLocal).length();
        }

        return LongestLength;
    }

    constexpr static
    const uint64_t ExportTrieSymbolTypeGetLongestDescriptionLength() noexcept {
        constexpr const auto Type = ExportTrieSymbolType::Regular;
        auto LongestLength = LargestIntHelper<uint64_t>();

        switch (Type) {
            case ExportTrieSymbolType::Regular:
                LongestLength =
                    ExportTrieSymbolTypeGetDescription(
                        ExportTrieSymbolType::Regular).length();
            case ExportTrieSymbolType::WeakDefinition:
                LongestLength =
                    ExportTrieSymbolTypeGetDescription(
                        ExportTrieSymbolType::WeakDefinition).length();
            case ExportTrieSymbolType::ThreadLocal:
                LongestLength =
                    ExportTrieSymbolTypeGetDescription(
                        ExportTrieSymbolType::ThreadLocal).length();
        }

        return LongestLength;
    }

    struct ExportTrieIterator;
    struct ExportTrieSymbol {
        friend struct ExportTrieIterator;
    public:
        using Type = ExportTrieSymbolType;
    protected:
        std::string String;
        std::string ReexportImportName;
        union {
            uint32_t ReexportDylibOrdinal;

            struct {
                uint64_t ResolverStubAddress;
                uint64_t ImageOffset;
            };
        };

        ExportTrieFlagsByte Flags;
        Type sType;
    public:
        inline std::string_view GetString() const noexcept {
            return String;
        }

        inline Type GetType() const noexcept {
            return sType;
        }

        inline std::string_view GetReexportImportName() const noexcept {
            assert(Flags.IsReexport());
            return ReexportImportName;
        }

        inline uint32_t GetReexportDylibOrdinal() const noexcept {
            assert(Flags.IsReexport());
            return ReexportDylibOrdinal;
        }

        inline uint64_t GetResolverStubAddress() const noexcept {
            assert(Flags.IsStubAndResolver());
            return ResolverStubAddress;
        }

        inline uint64_t GetImageOffset() const noexcept {
            assert(!Flags.IsReexport());
            return ImageOffset;
        }

        inline ExportTrieFlagsByte GetFlags() const noexcept {
            return Flags;
        }

        inline void ClearExportExclusiveInfo() noexcept {
            ImageOffset = 0;
            ReexportDylibOrdinal = 0;
            ResolverStubAddress = 0;

            Flags.Clear();
            sType = Type::Regular;
        }
    };

    struct ExportTrieIteratorEnd {};
    struct ExportTrieIterator {
    public:
        enum class Error {
            None,
            ReachedEnd,

            InvalidUleb128,
            InvalidFormat,
            OverlappingRanges,

            EmptyExport
        };
    protected:
        struct NodeInfo {
            const uint8_t *Ptr = nullptr;
            uint32_t Size = 0;
            uint8_t ChildCount = 0;
        };

        struct StackInfo {
            constexpr explicit StackInfo() noexcept = default;
            constexpr explicit StackInfo(const NodeInfo &Node) noexcept
            : Node(Node) {}

            NodeInfo Node;
            uint16_t ChildIndex = 0;

            std::string::size_type StringSize = 0;
            std::vector<LocationRange>::size_type RangeListSize = 0;
        };

        std::vector<StackInfo> StackList;
        union {
            PointerErrorStorage<enum Error> ParseError;
            const uint8_t *Begin;
        };

        const uint8_t *End;

        ExportTrieSymbol Symbol;
        std::vector<LocationRange> RangeList;

        void SetupInfoForTopNode() noexcept;
        void MoveUptoParentNode() noexcept;

        Error ParseNode(const uint8_t *Begin, NodeInfo *InfoOut) noexcept;
        Error Advance() noexcept;
    public:
        explicit ExportTrieIterator(const uint8_t *Begin,
                                    const uint8_t *End) noexcept;

        inline ExportTrieIterator &operator++() noexcept {
            assert(!ParseError.HasValue());
            Advance();

            return *this;
        }

        inline ExportTrieIterator &operator++(int) noexcept {
            return ++(*this);
        }

        inline ExportTrieIterator &operator+(uint64_t Amt) noexcept {
            assert(!ParseError.HasValue());
            for (auto I = uint64_t(); I != Amt; I++) {
                Advance();
            }

            return *this;
        }

        inline ExportTrieSymbol &operator*() noexcept {
            assert(!ParseError.HasValue());
            return Symbol;
        }

        inline const ExportTrieSymbol &operator*() const noexcept {
            assert(!ParseError.HasValue());
            return Symbol;
        }

        inline ExportTrieSymbol *operator->() noexcept {
            assert(!ParseError.HasValue());
            return &Symbol;
        }

        inline const ExportTrieSymbol *operator->() const noexcept {
            assert(!ParseError.HasValue());
            return &Symbol;
        }

        inline bool operator==(const ExportTrieIteratorEnd &End) {
            return StackList.empty();
        }

        inline bool operator!=(const ExportTrieIteratorEnd &End) {
            return !(*this == End);
        }
    };

    struct ExportTrieList {
        uint8_t *Begin;
        uint8_t *End;
    public:
        explicit ExportTrieList(uint8_t *Begin, uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        ExportTrieIterator begin() noexcept {
            return ExportTrieIterator(Begin, End);
        }

        ExportTrieIterator cbegin() const noexcept {
            return ExportTrieIterator(Begin, End);
        }

        ExportTrieIteratorEnd end() noexcept { return ExportTrieIteratorEnd(); }
        ExportTrieIteratorEnd cend() const noexcept {
            return ExportTrieIteratorEnd();
        }
    };

    struct ConstExportTrieList {
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        explicit ConstExportTrieList(const uint8_t *Begin,
                                     const uint8_t *End) noexcept
        : Begin(Begin), End(End) {}

        inline ExportTrieIterator begin() const noexcept {
            return ExportTrieIterator(Begin, End);
        }

        inline ExportTrieIteratorEnd end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        inline uint64_t GetSize() const noexcept {
            return static_cast<uint64_t>(End - Begin);
        }
    };

    struct DyldInfoCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            const auto IsOfKind =
                (Kind == LoadCommand::Kind::DyldInfo) ||
                (Kind == LoadCommand::Kind::DyldInfoOnly);

            return IsOfKind;
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(DyldInfoCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t RebaseOff;
        uint32_t RebaseSize;

        uint32_t BindOff;
        uint32_t BindSize;

        uint32_t WeakBindOff;
        uint32_t WeakBindSize;

        uint32_t LazyBindOff;
        uint32_t LazyBindSize;

        uint32_t ExportOff;
        uint32_t ExportSize;

        template <typename ByteType, typename PtrType>
        struct OpcodeListBase {
        protected:
            ByteType *Begin;
            ByteType *End;
        public:
            explicit OpcodeListBase(ByteType *Begin, ByteType *End) noexcept
            : Begin(Begin), End(End) {}

            explicit OpcodeListBase(ByteType *Begin, uint32_t Size) noexcept
            : Begin(Begin), End(this->Begin + Size) {}

            explicit OpcodeListBase(PtrType *Begin, PtrType *End) noexcept
            : Begin(reinterpret_cast<ByteType *>(Begin)),
              End(reinterpret_cast<ByteType *>(End)) {}

            explicit OpcodeListBase(PtrType *Begin, uint32_t Size) noexcept
            : Begin(reinterpret_cast<ByteType *>(Begin)),
              End(this->Begin + Size) {}

            struct EndIterator {};

            template <typename IteratorByteType>
            struct IteratorInfoBase {
            protected:
                IteratorByteType *Iter;
                IteratorByteType *End;
            public:
                explicit IteratorInfoBase(IteratorByteType *Iter,
                                          IteratorByteType *End) noexcept
                : Iter(Iter), End(End) {}

                [[nodiscard]] inline PtrType *GetPtr() const noexcept {
                    return reinterpret_cast<PtrType *>(Iter);
                }

                [[nodiscard]] inline ByteType &GetByte() const noexcept {
                    return *Iter;
                }

                [[nodiscard]]
                std::optional<uint64_t> ReadUleb128() const noexcept {
                    /* TODO: */
                    return 0;
                }
            };

            template <typename IteratorByteType>
            struct IteratorBase : BasicContiguousIterator<IteratorByteType> {
            private:
                using Super = BasicContiguousIterator<IteratorByteType>;
            protected:
                IteratorByteType *End;
            public:
                explicit IteratorBase(IteratorByteType *Item,
                                      IteratorByteType *End) noexcept
                : Super(Item), End(End) {}

                using IteratorInfo = IteratorInfoBase<IteratorByteType>;
                inline IteratorInfo operator*() const noexcept {
                    return IteratorInfo(this->Item, End);
                }

                inline bool operator==(const EndIterator &End) const noexcept {
                    const auto Opcode = this->GetByte().GetOpcode();
                    const auto &Item = this->Item;

                    return (Item >= End || Opcode == RebaseByte::Opcode::Done);
                }

                inline bool operator!=(const EndIterator &) const noexcept {
                    return !(this->Item == End);
                }
            };

            using Iterator = IteratorBase<ByteType>;
            using ConstIterator = IteratorBase<std::add_const_t<ByteType>>;

            [[nodiscard]] Iterator begin() noexcept {
                return Iterator(Begin, End);
            }

            [[nodiscard]] EndIterator end() noexcept { return EndIterator(); }
            [[nodiscard]] ConstIterator cbegin() const noexcept {
                return ConstIterator(Begin, End);
            }

            [[nodiscard]] EndIterator cend() const noexcept {
                return EndIterator();
            }
        };

        using BindOpcodeList = OpcodeListBase<BindByte, uint8_t>;
        using ConstBindOpcodeList =
            OpcodeListBase<const BindByte, const uint8_t>;

        using RebaseOpcodeList = OpcodeListBase<RebaseByte, uint8_t>;
        using ConstRebaseOpcodeList =
            OpcodeListBase<const RebaseByte, const uint8_t>;

        [[nodiscard]] BindOpcodeList
        GetBindOpcodeList(const MemoryMap &Map,
                          bool IsBigEndian) const noexcept;

        [[nodiscard]] ConstBindOpcodeList
        GetConstBindOpcodeList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept;

        [[nodiscard]] RebaseOpcodeList
        GetRebaseOpcodeList(const MemoryMap &Map,
                            bool IsBigEndian) const noexcept;

        [[nodiscard]] ConstRebaseOpcodeList
        GetConstRebaseOpcodeList(const ConstMemoryMap &Map,
                                 bool IsBigEndian) const noexcept;

        TypedAllocationOrError<ExportTrieList *, SizeRangeError>
        GetExportTrieList(const MemoryMap &Map, bool IsBigEndian) noexcept;

        TypedAllocationOrError<ConstExportTrieList *, SizeRangeError>
        GetConstExportTrieList(const ConstMemoryMap &Map,
                               bool IsBigEndian) const noexcept;

        TypedAllocationOrError<std::vector<ExportTrieSymbol> *, SizeRangeError>
        GetExportList(const ConstMemoryMap &Map, bool IsBigEndian) noexcept;
    };

    struct LinkerOptionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::LinkerOption);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(LinkerOptionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Offset;
	    uint32_t Size;
    };

    struct SymbolSegmentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SymbolSegment);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SymbolSegmentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint32_t Offset;
        uint32_t Size;
    };

    struct IdentCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Ident);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(IdentCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }
    };

    struct FixedVMFileCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::FixedVMFile);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(FixedVMFileCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        LoadCommandString Name;
        uint32_t HeaderAddr;

        [[nodiscard]] bool IsNameOffsetValid(bool IsBigEndian) const noexcept;
        [[nodiscard]] LoadCommandString::GetValueResult
        GetName(bool IsBigEndian) const noexcept;
    };

    struct EntryPointCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Main);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(EntryPointCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        uint64_t EntryOffset;
        uint64_t StackSize;
    };

    enum class PackedVersion64Masks : uint64_t {
        Revision3 = 0x3ff,
        Revision2 = 0xffc00,
        Revision1 = 0x3ff00000,
        Minor = 0xffc0000000,
        Major = 0xffffff0000000000
    };

    enum class PackedVersion64Shifts : uint64_t {
        Revision3,
        Revision2 = 10,
        Revision1 = 20,
        Minor = 30,
        Major = 40
    };

    struct PackedVersion64 :
        private BasicMasksAndShifts<PackedVersion64Masks,
                                    PackedVersion64Shifts> {
    public:
        using Masks = PackedVersion64Masks;
        using Shifts = PackedVersion64Shifts;

        inline uint16_t GetRevision3(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Revision3, Shifts::Revision3, IsBE);
        }

        inline uint16_t GetRevision2(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Revision2, Shifts::Revision2, IsBE);
        }

        inline uint16_t GetRevision1(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Revision1, Shifts::Revision1, IsBE);
        }

        inline uint16_t GetMinor(bool IsBE) const noexcept {
            return GetValueForMask(Masks::Minor, Shifts::Minor, IsBE);
        }

        inline uint32_t GetMajor(bool IsBE) const noexcept {
            const auto Value =
                static_cast<uint32_t>(
                    GetValueForMask(Masks::Major, Shifts::Major, IsBE));

            return Value;
        }
    };

    struct SourceVersionCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::SourceVersion);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(SourceVersionCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        PackedVersion64 Version;
    };

    struct NoteCommand : public LoadCommand {
        [[nodiscard]] constexpr
        static inline bool IsOfKind(LoadCommand::Kind Kind) noexcept {
            return (Kind == LoadCommand::Kind::Note);
        }

        [[nodiscard]] inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(bool IsBigEndian) noexcept {
            return HasValidCmdSize(SwitchEndianIf(CmdSize, IsBigEndian));
        }

        [[nodiscard]] constexpr
        static inline LoadCommand::CmdSizeInvalidType
        HasValidCmdSize(uint32_t CmdSize) noexcept {
            if (CmdSize < sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooSmall;
            }

            if (CmdSize > sizeof(NoteCommand)) {
                return LoadCommand::CmdSizeInvalidType::TooLarge;
            }

            return LoadCommand::CmdSizeInvalidType::None;
        }

        char DataOwner[16];
        uint64_t Offset;
        uint64_t Size;
    };
}
