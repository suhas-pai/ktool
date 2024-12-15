//
//  Objects/FatMachO.cpp
//  ktool
//
//  Created by suhaspai on 11/20/22.
//

#include "Objects/FatMachO.h"
#include "Utils/Overflow.h"

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

        const auto TotalArchListSizeOpt =
            Utils::MulAndCheckOverflow(ArchSize, ArchCount);

        if (!TotalArchListSizeOpt.has_value()) {
            return FatMachO::OpenError::TooManyArchitectures;
        }

        const auto TotalArchListSize = TotalArchListSizeOpt.value();
        if (!Map.range().canContainSize(TotalArchListSize)) {
            if (ArchCount == 1) {
                return FatMachO::OpenError::SizeTooSmall;
            }

            return FatMachO::OpenError::TooManyArchitectures;
        }

        return FatMachO::OpenError::None;
    }

    auto FatMachO::Open(const ADT::MemoryMap &Map) noexcept
        -> std::expected<FatMachO *, Error>
    {
        if (const auto Err = ValidateHeader(Map); Err != OpenError::None) {
            return std::unexpected(Error(Err));
        }

        return new FatMachO(Map);
    }

    auto FatMachO::OpenAndValidateArchs(const ADT::MemoryMap &Map) noexcept
        -> std::expected<FatMachO *, Error>
    {
        if (const auto Err = ValidateHeader(Map); Err != OpenError::None) {
            return std::unexpected(Error(Err));
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
                    return std::unexpected(Error(OpenError::ArchOutOfBounds));
                }

                for (auto J = uint32_t(); J != I; J++) {
                    const auto &Inner = ArchList[J];
                    const auto InnerRange = Inner.range(Header.isBigEndian());

                    if (InnerRange.overlaps(ArchRange)) {
                        return std::unexpected(
                            Error(OpenError::OverlappingArchs));
                    }

                    if (Inner.CpuKind == Arch.CpuKind &&
                        Inner.CpuSubKind == Arch.CpuSubKind)
                    {
                        return std::unexpected(
                            Error(OpenError::ArchsForSameCpu));
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
                    return std::unexpected(Error(OpenError::ArchOutOfBounds));
                }

                for (auto J = uint32_t(); J != I; J++) {
                    const auto &Inner = ArchList[J];
                    const auto InnerRange = Inner.range(Header.isBigEndian());

                    if (InnerRange.overlaps(ArchRange)) {
                        return std::unexpected(
                            Error(OpenError::OverlappingArchs));
                    }

                    if (Inner.CpuKind == Arch.CpuKind &&
                        Inner.CpuSubKind == Arch.CpuSubKind)
                    {
                        return std::unexpected(
                            Error(OpenError::ArchsForSameCpu));
                    }
                }
            }
        }

        return new FatMachO(Map);
    }
}
