//
//  src/ADT/Mach-O/SharedLibraryUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "LoadCommandStorage.h"
#include "SharedLibraryUtil.h"

namespace MachO {
    SharedLibraryInfoCollection
    SharedLibraryInfoCollection::Open(
        const ConstLoadCommandStorage &LoadCmdStorage,
        Error *ErrorOut) noexcept
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
                    EmptyStringValue :
                    GetNameResult.getString();

            if (GetNameResult.hasError()) {
                Error = Error::InvalidPath;
            }

            const auto &Info = DylibCmd.Info;
            auto LibInfo =
                std::make_unique<SharedLibraryInfo>(SharedLibraryInfo {
                    .Kind = LC.getKind(IsBigEndian),
                    .Path = std::string(Name),
                    .Index = LCIndex,
                    .Timestamp = Info.getTimestamp(IsBigEndian),
                    .CurrentVersion = Info.getCurrentVersion(IsBigEndian),
                    .CompatibilityVersion = Info.getCompatVersion(IsBigEndian)
                });

            Result.List.emplace_back(std::move(LibInfo));
            LCIndex++;
        }

        if (ErrorOut != nullptr) {
            *ErrorOut = Error;
        }

        return Result;
    }
}
