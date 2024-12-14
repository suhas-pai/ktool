//
//  ADT/Mach-O/LoadCommandStorage.cpp
//  ktool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/LoadCommandStorage.h"

namespace MachO {
    static auto
    VerifyLoadCommands(const uint8_t *const Begin,
                       const uint8_t *const End,
                       const uint32_t Count,
                       const bool IsBigEndian,
                       const bool Is64Bit,
                       uint32_t *const SizeOut) noexcept
        -> ConstLoadCommandStorage::Error
    {
        if (Count == 0) {
            return ConstLoadCommandStorage::Error::NoLoadCommands;
        }

        const auto AlignMask = (Is64Bit) ? 7ull : 3ull;

        auto I = uint32_t();
        auto Iter = Begin;

        do {
            const auto LC = reinterpret_cast<const LoadCommand *>(Iter);
            const auto CmdSize = LC->getCmdSize(IsBigEndian);

            if (CmdSize < sizeof(*LC)) {
                return ConstLoadCommandStorage::Error::CmdSizeTooSmall;
            }

            if ((CmdSize & AlignMask) != 0) {
                return ConstLoadCommandStorage::Error::CmdSizeNotAligned;
            }

            Iter += CmdSize;
            if (Iter > End) {
                return ConstLoadCommandStorage::Error::StorageSizeTooSmall;
            }

            if (I == (Count - 1)) {
                break;
            }

            if (Iter == End) {
                return ConstLoadCommandStorage::Error::StorageSizeTooSmall;
            }

            I++;
        } while (true);

        *SizeOut = static_cast<uint32_t>(Iter - Begin);
        return ConstLoadCommandStorage::Error::None;
    }

    ConstLoadCommandStorage::ConstLoadCommandStorage(const Error Error) noexcept
    : BeginOrError(Error) {}

    ConstLoadCommandStorage::ConstLoadCommandStorage(
        const uint8_t *const Begin,
        const uint8_t *const End,
        const uint32_t Count,
        const bool IsBigEndian) noexcept
    : BeginOrError(Begin), End(End), Count(Count), sIsBigEndian(IsBigEndian) {}

    LoadCommandStorage::LoadCommandStorage(Error Error) noexcept
    : ConstLoadCommandStorage(Error) {}

    LoadCommandStorage::LoadCommandStorage(uint8_t *const Begin,
                                           uint8_t *const End,
                                           const uint32_t Count,
                                           const bool IsBigEndian) noexcept
    : ConstLoadCommandStorage(Begin, End, Count, IsBigEndian) {}

    auto
    ConstLoadCommandStorage::Open(const uint8_t *const Begin,
                                  const uint32_t Count,
                                  uint32_t Size,
                                  const bool IsBigEndian,
                                  const bool Is64Bit,
                                  const bool Verify) noexcept
        -> ConstLoadCommandStorage
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
