//
//  ADT/Mach-O/SharedLibraryUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/LoadCommandStorage.h"
#include "ADT/Mach-O/SharedLibraryUtil.h"

namespace MachO {
    auto
    SharedLibraryInfoCollection::Open(
        const ConstLoadCommandStorage &LoadCmdStorage,
        Error *const ErrorOut) noexcept
            -> SharedLibraryInfoCollection
    {
        const auto IsBigEndian = LoadCmdStorage.isBigEndian();

        auto Error = Error::None;
        auto LCIndex = uint32_t();
        auto Result = SharedLibraryInfoCollection();

        for (const auto &LC : LoadCmdStorage) {
            if (!LC.isSharedLibraryKind(IsBigEndian)) {
                continue;
            }

            const auto &DylibCmd = cast<DylibCommand>(LC, IsBigEndian);
            const auto GetNameResult = DylibCmd.GetName(IsBigEndian);
            const auto &Name =
                (GetNameResult.hasError()) ?
                    std::string_view() :
                    GetNameResult.getString();

            if (GetNameResult.hasError()) {
                Error = Error::InvalidPath;
            }

            const auto &Info = DylibCmd.Info;
            auto LibInfo = std::make_unique<SharedLibraryInfo>();

            LibInfo->setKind(LC.getKind(IsBigEndian));
            LibInfo->setPath(std::string(Name));
            LibInfo->setIndex(LCIndex);
            LibInfo->setTimestamp(Info.getTimestamp(IsBigEndian));
            LibInfo->setCurrentVersion(Info.getCurrentVersion(IsBigEndian));
            LibInfo->setCompatVersion(Info.getCompatVersion(IsBigEndian));

            Result.List.emplace_back(std::move(LibInfo));
            LCIndex++;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error;
        }

        return Result;
    }
}
