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
        if (const auto Error = ValidateHeader(Map); Error != OpenError::None) {
            return Error;
        }

        return new FatMachO(Map);
    }

    auto FatMachO::OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept ->
        ADT::PointerOrError<FatMachO, OpenError>
    {
        if (const auto Error = ValidateHeader(Map); Error != OpenError::None) {
            return Error;
        }

        const auto Header = *Map.base<::MachO::FatHeader>();
        if (Header.ArchCount == 0) {
            return new FatMachO(Map);
        }

        const auto ArchCount = Header.archCount();
        if (!Map.range().canContain<::MachO::FatArch>(ArchCount)) {
            if (!Map.range().canContain<::MachO::FatArch>()) {
                return OpenError::SizeTooSmall;
            }

            return OpenError::TooManyArchitectures;
        }

        if (Header.is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);
            
            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                const auto ArchRange = Arch.range(Header.isBigEndian());

                if (!Map.range().contains(ArchRange)) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = 0; J != I; J++) {
                    const auto &Inner = ArchList[J];
                    const auto InnerRange = Inner.range(Header.isBigEndian());

                    if (InnerRange.overlaps(ArchRange)) {
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
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                const auto ArchRange = Arch.range(Header.isBigEndian());

                if (!Map.range().contains(ArchRange)) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = 0; J != I; J++) {
                    const auto &Inner = ArchList[J];
                    const auto InnerRange = Inner.range(Header.isBigEndian());

                    if (InnerRange.overlaps(ArchRange)) {
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
    FatMachO::getArchObjectForCpu(const Mach::CpuKind CpuKind,
                                  const int32_t SubKind) const noexcept
        -> Objects::OpenResult
    {
        const auto ArchCount = archCount();
        if (is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (Arch.CpuKind == CpuKind && Arch.CpuSubKind == SubKind) {
                    const auto ArchRange = Arch.range(isBigEndian());
                    const auto ObjectMap = ADT::MemoryMap(Map, ArchRange);

                    return Objects::OpenFrom(ObjectMap, Kind::FatMachO);
                }
            }
        } else {
            const auto ArchList =
                Map.get<::MachO::FatArch>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            for (auto I = 0; I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                if (Arch.CpuKind == CpuKind && Arch.CpuSubKind == SubKind) {
                    const auto ArchRange = Arch.range(isBigEndian());
                    const auto ObjectMap = ADT::MemoryMap(Map, ArchRange);

                    return Objects::OpenFrom(ObjectMap, Kind::FatMachO);
                }
            }
        }

        return OpenErrorNone;
    }

    [[nodiscard]]
    auto FatMachO::getArchObjectAtIndex(const uint32_t Index) const noexcept ->
        Objects::OpenResult
    {
        const auto ArchCount = archCount();
        assert(Index < ArchCount);

        if (is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            const auto ArchRange = ArchList[Index].range(isBigEndian());
            return Objects::Open(ADT::MemoryMap(Map, ArchRange));
        }

        const auto ArchList =
            Map.get<::MachO::FatArch>(
                sizeof(struct ::MachO::FatHeader), ArchCount);

        const auto ArchRange = ArchList[Index].range(isBigEndian());
        return Objects::Open(ADT::MemoryMap(Map, ArchRange));
    }
}
