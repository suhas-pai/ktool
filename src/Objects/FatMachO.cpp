//
//  Objects/FatMachO.cpp
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#include "Objects/FatMachO.h"

namespace Objects {
    static auto ValidateHeader(const ADT::MemoryMap &Map) {
        const auto Header = Map.base<::MachO::FatHeader>();
        if (Header == nullptr) {
            const auto Magic = Map.base<::MachO::Magic>();
            if (Magic == nullptr) {
                return FatMachO::OpenError::SizeTooSmall;
            }

            const auto Result =
                ::MachO::MagicIsFat(*Magic) ?
                    FatMachO::OpenError::SizeTooSmall :
                    FatMachO::OpenError::WrongFormat;

            return Result;
        }

        if (!::MachO::MagicIsFat(Header->Magic)) {
            return FatMachO::OpenError::WrongFormat;
        }

        return FatMachO::OpenError::None;
    }

    auto FatMachO::Open(const ADT::MemoryMap &Map) noexcept ->
        ADT::PointerOrError<FatMachO, OpenError>
    {
        if (const auto Error = ValidateHeader(Map);
            Error != OpenError::None)
        {
            return Error;
        }

        return new FatMachO(Map);
    }

    auto FatMachO::OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept ->
        ADT::PointerOrError<FatMachO, OpenError>
    {
        if (const auto Error = ValidateHeader(Map);
            Error != OpenError::None)
        {
            return Error;
        }

        const auto Header = *Map.base<::MachO::FatHeader>();
        if (Header.ArchCount == 0) {
            return new FatMachO(Map);
        }

        if (!Map.range().canContain<::MachO::FatArch>(Header.archCount())) {
            if (!Map.range().canContain<::MachO::FatArch>()) {
                return OpenError::SizeTooSmall;
            }

            return OpenError::TooManyArchitectures;
        }

        const auto ArchCount = Header.archCount();
        if (Header.is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);
            
            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (!Map.range().contains(Arch.range())) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = 0; J != I; J++) {
                    const auto &Inner = ArchList[J];
                    if (Inner.range().overlaps(Arch.range())) {
                        return OpenError::OverlappingArchs;
                    }

                    if (Inner.CpuKind == Arch.CpuKind &&
                        Inner.CpuSubKind == Arch.CpuSubKind)
                    {
                        return OpenError::ArchsForSameCpu;
                    }
                }
            }
        } else {
            const auto ArchList =
                Map.get<::MachO::FatArch>(
                    sizeof(struct ::MachO::FatHeader), Header.archCount());

            for (auto I = 0; I != Header.ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (!Arch.range().canRepresentIn<uint32_t>()) {
                    return OpenError::ArchOutOfBounds;
                }

                if (!Map.range().contains(Arch.range())) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = 0; J != I; J++) {
                    const auto &Inner = ArchList[J];
                    if (Inner.range().overlaps(Arch.range())) {
                        return OpenError::OverlappingArchs;
                    }

                    if (Inner.CpuKind == Arch.CpuKind &&
                        Inner.CpuSubKind == Arch.CpuSubKind)
                    {
                        return OpenError::ArchsForSameCpu;
                    }
                }
            }
        }

        return new FatMachO(Map);
    }

    [[nodiscard]] auto
    FatMachO::getMachOForCpu(const Mach::CpuKind CpuKind,
                             const int32_t SubKind) const noexcept ->
        ADT::PointerOrError<MachO, MachO::OpenError>
    {
        const auto Header = header();
        const auto ArchCount = Header.archCount();

        if (Header.is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (Arch.CpuKind == CpuKind && Arch.CpuSubKind == SubKind) {
                    return MachO::Open(ADT::MemoryMap(Map, Arch.range()));
                }
            }
        } else {
            const auto ArchList =
                Map.get<::MachO::FatArch>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            for (auto I = 0; I != Header.ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (Arch.CpuKind == CpuKind && Arch.CpuSubKind == SubKind) {
                    return MachO::Open(ADT::MemoryMap(Map, Arch.range()));
                }
            }
        }

        return nullptr;
    }

    [[nodiscard]] auto
    FatMachO::getMachOForIndex(const uint32_t Index) const noexcept ->
        ADT::PointerOrError<MachO, MachO::OpenError>
    {
        const auto Header = header();
        const auto ArchCount = Header.archCount();

        assert(Index < ArchCount);
        if (Header.is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            return MachO::Open(ADT::MemoryMap(Map, ArchList[Index].range()));
        }

        const auto ArchList =
            Map.get<::MachO::FatArch>(
                sizeof(struct ::MachO::FatHeader), ArchCount);

        return MachO::Open(ADT::MemoryMap(Map, ArchList[Index].range()));
    }
}
