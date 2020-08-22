//
//  src/ADT/Mach-O/LoadCommandStorage.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/LoadCommandStorage.h"

namespace MachO {
    static ConstLoadCommandStorage::Error
    VerifyLoadCommands(const uint8_t *Begin,
                       const uint8_t *End,
                       uint32_t Count,
                       bool IsBigEndian,
                       bool Is64Bit,
                       uint32_t *SizeOut) noexcept
    {
        if (Count == 0) {
            return ConstLoadCommandStorage::Error::NoLoadCommands;
        }

        const auto AlignMask = (Is64Bit) ? 7ull : 3ull;

        auto I = uint32_t();
        auto Iter = Begin;

        do {
            const auto LoadCmd =
                reinterpret_cast<const LoadCommand *>(Iter);

            const auto CmdSize = LoadCmd->getCmdSize(IsBigEndian);
            if (CmdSize < sizeof(*LoadCmd)) {
                return ConstLoadCommandStorage::Error::CmdSizeTooSmall;
            }

            if ((CmdSize & AlignMask) != 0) {
                return ConstLoadCommandStorage::Error::CmdSizeNotAligned;
            }

            Iter += CmdSize;
            if (Iter > End) {
                return ConstLoadCommandStorage::Error::StorageSizeTooSmall;
            }

            if (I != (Count - 1)) {
                if (Iter == End) {
                    return ConstLoadCommandStorage::Error::StorageSizeTooSmall;
                }

                I++;
            } else {
                break;
            }
        } while (true);

        if (Iter != End) {
            return ConstLoadCommandStorage::Error::StorageSizeTooSmall;
        }

        *SizeOut = static_cast<uint32_t>(Iter - Begin);
        return ConstLoadCommandStorage::Error::None;
    }

    ConstLoadCommandStorage::ConstLoadCommandStorage(Error Error) noexcept
    : ErrorStorage(Error) {}

    ConstLoadCommandStorage::ConstLoadCommandStorage(const uint8_t *Begin,
                                                     const uint8_t *End,
                                                     const uint32_t Count,
                                                     bool IsBigEndian) noexcept
    : Begin(Begin), End(End), Count(Count), sIsBigEndian(IsBigEndian) {}

    LoadCommandStorage::LoadCommandStorage(Error Error) noexcept
    : ConstLoadCommandStorage(Error) {}

    LoadCommandStorage::LoadCommandStorage(uint8_t *Begin,
                                           uint8_t *End,
                                           uint32_t Count,
                                           bool IsBigEndian) noexcept
    : ConstLoadCommandStorage(Begin, End, Count, IsBigEndian) {}

    ConstLoadCommandStorage
    ConstLoadCommandStorage::Open(const uint8_t *Begin,
                                  uint32_t Count,
                                  uint32_t Size,
                                  bool IsBigEndian,
                                  bool Is64Bit,
                                  bool Verify) noexcept
    {
        if (Size < sizeof(LoadCommand)) {
            return Error::StorageSizeTooSmall;
        }

        auto End = Begin + Size;
        if (Verify) {
            const auto Error =
                VerifyLoadCommands(Begin,
                                   End,
                                   Count,
                                   IsBigEndian,
                                   Is64Bit,
                                   &Size);

            if (Error != Error::None) {
                return Error;
            }

            End = Begin + Size;
        }

       return ConstLoadCommandStorage(Begin, End, Count, IsBigEndian);
    }
}
