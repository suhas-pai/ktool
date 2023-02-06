//
//  Objects/FatMachO.cpp
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#include "Objects/FatMachO.h"

namespace Objects {
    static auto ValidateHeader(const ADT::MemoryMap &Map) noexcept {
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

        const auto ArchCount = Header->archCount();
        const auto ArchSize =
            Header->is64Bit() ?
                sizeof(::MachO::FatArch64) : sizeof(::MachO::FatArch);

        const auto TotalArchListSize = ArchSize * ArchCount;
        if (!Map.range().canContainSize(TotalArchListSize)) {
            if (ArchCount == 1) {
                return FatMachO::OpenError::SizeTooSmall;
            }

            return FatMachO::OpenError::TooManyArchitectures;
        }

        return FatMachO::OpenError::None;
    }

    auto FatMachO::Open(const ADT::MemoryMap &Map) noexcept
        -> ADT::PointerOrError<FatMachO, OpenError>
    {
        if (const auto Error = ValidateHeader(Map); Error != OpenError::None) {
            return Error;
        }

        return new FatMachO(Map);
    }

    auto FatMachO::OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept
        -> ADT::PointerOrError<FatMachO, OpenError>
    {
        if (const auto Error = ValidateHeader(Map); Error != OpenError::None) {
            return Error;
        }

        const auto Header = *Map.base<::MachO::FatHeader, false>();
        if (Header.ArchCount == 0) {
            return new FatMachO(Map);
        }

        const auto ArchCount = Header.archCount();
        if (Header.is64Bit()) {
            const auto ArchList =
                Map.get<::MachO::FatArch64, false>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            // Don't Swap CpuKind and CpuSubKind, because we're only comparing
            // them against values in the same endian.

            for (auto I = uint32_t(); I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                const auto ArchRange = Arch.range(Header.isBigEndian());

                if (!Map.range().contains(ArchRange)) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = uint32_t(); J != I; J++) {
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
                Map.get<::MachO::FatArch, false>(
                    sizeof(struct ::MachO::FatHeader), ArchCount);

            // Don't Swap CpuKind and CpuSubKind, because we're only comparing
            // them against values in the same endian.

            for (auto I = uint32_t(); I != ArchCount; I++) {
                const auto &Arch = ArchList[I];
                const auto ArchRange = Arch.range(Header.isBigEndian());

                if (!Map.range().contains(ArchRange)) {
                    return OpenError::ArchOutOfBounds;
                }

                for (auto J = uint32_t(); J != I; J++) {
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
        const auto IsBigEndian = this->isBigEndian();
        if (is64Bit()) {
            for (const auto &Arch : arch64List()) {
                if (Arch.cpuKind(IsBigEndian) == CpuKind &&
                    Arch.cpuSubKind(IsBigEndian) == SubKind)
                {
                    const auto ArchRange = Arch.range(isBigEndian());
                    const auto ObjectMap = ADT::MemoryMap(Map, ArchRange);

                    return Objects::OpenFrom(ObjectMap, Kind::FatMachO);
                }
            }
        } else {
            for (const auto &Arch : archList()) {
                if (Arch.cpuKind(IsBigEndian) == CpuKind &&
                    Arch.cpuSubKind(IsBigEndian) == SubKind)
                {
                    const auto ArchRange = Arch.range(isBigEndian());
                    const auto ObjectMap = ADT::MemoryMap(Map, ArchRange);

                    return Objects::OpenFrom(ObjectMap, Kind::FatMachO);
                }
            }
        }

        return OpenErrorNone;
    }

    [[nodiscard]]
    auto FatMachO::getArchObjectAtIndex(const uint32_t Index) const noexcept
        -> Objects::OpenResult
    {
        assert(!Utils::IndexOutOfBounds(Index, archCount()));
        if (is64Bit()) {
            const auto ArchList = archs64();
            const auto ArchRange = ArchList[Index].range(isBigEndian());

            return Objects::OpenFrom(ADT::MemoryMap(Map, ArchRange),
                                     Kind::FatMachO);
        }

        const auto ArchList = archs();
        const auto ArchRange = ArchList[Index].range(isBigEndian());

        return Objects::OpenFrom(ADT::MemoryMap(Map, ArchRange),
                                 Kind::FatMachO);
    }
}
