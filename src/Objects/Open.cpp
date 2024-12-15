//
//  Objects/Open.cpp
//  ktool
//
//  Created by suhaspai on 12/14/24.
//

#include "Objects/DyldSharedCache.h"
#include "Objects/DscImage.h"
#include "Objects/FatMachO.h"

#include "Objects/Open.h"

namespace Objects {
    auto OpenArch(const FatMachO &Fat, const uint32_t Index) noexcept
        -> std::expected<Base *, OpenError>
    {
        assert(!Utils::IndexOutOfBounds(Index, Fat.archCount()));
        if (Fat.is64Bit()) {
            const auto ArchList = Fat.archs64();
            const auto ArchRange = ArchList[Index].range(Fat.isBigEndian());

            return Objects::OpenFrom(ADT::MemoryMap(Fat.map(), ArchRange),
                                     Kind::FatMachO);
        }

        const auto ArchList = Fat.archs();
        const auto ArchRange = ArchList[Index].range(Fat.isBigEndian());

        return Objects::OpenFrom(ADT::MemoryMap(Fat.map(), ArchRange),
                                 Kind::FatMachO);
    }

    auto
    OpenImage(const DyldSharedCache &Dsc, const uint32_t Index) noexcept
        -> std::expected<Base *, OpenError>
    {
        assert(!Utils::IndexOutOfBounds(Index, Dsc.imageCount()));

        const auto ImageInfo = Dsc.imageInfoList()[Index];
        const auto ObjectOrErr = DscImage::Open(Dsc, ImageInfo);

        if (ObjectOrErr.has_value()) {
            return ObjectOrErr.value();
        }

        return std::unexpected(OpenError(ObjectOrErr.error()));
    }
}