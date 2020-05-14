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
HandleLoadCommandStorageError(FILE *ErrFile,
                              MachO::LoadCommandStorage::Error Error) {
    switch (Error) {
        case MachO::LoadCommandStorage::Error::None:
            break;
        case MachO::LoadCommandStorage::Error::NoLoadCommands:
            fprintf(ErrFile, "File has no load-commands!\n");
            break;
        case MachO::LoadCommandStorage::Error::CmdSizeTooSmall:
            fprintf(ErrFile,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Too Small)\n");
            break;
        case MachO::LoadCommandStorage::Error::CmdSizeNotAligned:
            fprintf(ErrFile,
                    "LoadCommands Buffer has an invalid LoadCommand (CmdSize "
                    "Not Aligned)\n");
            break;
        case MachO::LoadCommandStorage::Error::SizeOfCmdsTooSmall:
            fprintf(ErrFile, "LoadCommands Buffer is too small\n");
            break;
    }
}

MachO::LoadCommandStorage
OperationCommon::GetLoadCommandStorage(MachOMemoryObject &Object,
                                       FILE *ErrFile) noexcept
{
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(ErrFile, LoadCommandStorage.GetError());

    return LoadCommandStorage;
}

MachO::ConstLoadCommandStorage
OperationCommon::GetConstLoadCommandStorage(
    const ConstMachOMemoryObject &Object,
    FILE *ErrFile) noexcept
{
    const auto LoadCommandStorage = Object.GetLoadCommands();
    HandleLoadCommandStorageError(ErrFile, LoadCommandStorage.GetError());

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
