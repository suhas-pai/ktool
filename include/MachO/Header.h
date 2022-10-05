//
//  MachO/Header.h
//  ktool
//
//  Created by suhaspai on 10/2/22.
//

#pragma once

#include <string>
#include "ADT/Range.h"

#include "Mach/Machine.h"
#include "Mach/VmProt.h"

namespace MachO {
    enum class Magic : uint32_t {
        Default = 0xFEEDFACE,
        Swapped = 0xCEFADEFE,

        Default64 = 0xFEEDFACF,
        Swapped64 = 0xCEFADEFC
    };

    enum class FileKind {
        Object = 1,
        Executable,
        FixedVMSharedLib,
        Core,
        PreLoadedExecutable,
        DynamicLibrary,
        DynamicLinker,
        Bundle,
        DylibStub,
        Dsym,
        KextBundle,
        FileSet
    };

    struct Header {
        enum class FlagsEnum {
            NoUndefineds = 1 << 0,
            IncrementalLink = 1 << 1,
            DynamicLinkerLink = 1 << 2,

            BindAtLoad    = 1 << 3,
            PreBound      = 1 << 4,
            SplitSegments = 1 << 5,
            LazyInit      = 1 << 6,

            TwoLevelNamespaces  = 1 << 7,
            ForceFlatNamespaces = 1 << 8,

            NoMultipleDefinitions = 1 << 9,
            NoFixPrebinding = 1 << 10,

            Prebindable = 1 << 11,
            AllModulesBound = 1 << 12,
            SubsectionsViaSymbols = 1 << 13,

            Canonical = 1 << 14,
            WeakDefines = 1 << 15,

            AllowStackExecution = 1 << 16,

            RootSafe = 1 << 17,
            SetuidSafe = 1 << 18,

            NoReexportedDylibs = 1 << 19,
            PositionIndependentExec = 1 << 20,
            DeadStrippableDylib = 1 << 21,
            HasTlvDescriptors = 1 << 22,
            NoHeapExecution = 1 << 23,
            AppExtensionSafe = 1 << 24,

            NlistOutOfSyncDyldInfo = 1 << 25,

            SimSupport = 1 << 26,
            DylibInCache = 1 << 31,
        };

        Magic Magic;
        Mach::CpuKind CpuKind;
        int32_t CpuSubKind;
        uint32_t Ncmds;
        uint32_t SizeOfCmds;
        uint32_t Flags;
    };
}
