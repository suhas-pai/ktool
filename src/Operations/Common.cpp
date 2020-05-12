//
//  include/Operations/Common.cpp
//  stool
//
//  Created by Suhas Pai on 4/23/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <unistd.h>

#include "Common.h"
#include <string_view>

static void
HandleLoadCommandStorageError(MachO::LoadCommandStorage::Error Error) {
    switch (Error) {
        case MachO::LoadCommandStorage::Error::None:
            break;
        case MachO::LoadCommandStorage::Error::NoLoadCommands:
            fprintf(stderr, "File has no load-commands!\n");
            exit(1);
        case MachO::LoadCommandStorage::Error::CmdSizeTooSmall:
            fprintf(stderr,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Too Small)\n");
            exit(1);
        case MachO::LoadCommandStorage::Error::CmdSizeNotAligned:
            fprintf(stderr,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Not Aligned)\n", stderr);
            exit(1);
        case MachO::LoadCommandStorage::Error::SizeOfCmdsTooSmall:
            fprintf(stderr, "LoadCommands Buffer is too small\n");
            exit(1);
    }
}

MachO::LoadCommandStorage
OperationCommon::GetLoadCommandStorage(MachOMemoryObject &Object) noexcept {
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(LoadCommandStorage.GetError());

    return LoadCommandStorage;
}

MachO::ConstLoadCommandStorage
OperationCommon::GetConstLoadCommandStorage(
    const ConstMachOMemoryObject &Object) noexcept
{
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(LoadCommandStorage.GetError());

    return LoadCommandStorage;
}

const std::string_view &
OperationCommon::GetLoadCommandStringValue(
    const MachO::LoadCommandString::GetValueResult &Result) noexcept
{
    if (Result.HasError()) {
        constexpr static const auto InvalidString =
            std::string_view("(Invalid LoadCommand-String)");
        return InvalidString;
    }

    return Result.GetString();
}
