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
                       uint32_t Ncmds,
                       bool IsBigEndian,
                       bool Is64Bit,
                       uint32_t *SizeOfCmdsOut) noexcept
    {
        if (Ncmds == 0) {
            return ConstLoadCommandStorage::Error::NoLoadCommands;
        }

        const auto AlignMask = (Is64Bit) ? 7ull : 3ull;

        auto I = uint32_t();
        auto Iter = Begin;

        do {
            const auto LoadCmd =
                reinterpret_cast<const MachO::LoadCommand *>(Iter);

            const auto CmdSize = SwitchEndianIf(LoadCmd->CmdSize, IsBigEndian);
            if (CmdSize < sizeof(*LoadCmd)) {
                return ConstLoadCommandStorage::Error::CmdSizeTooSmall;
            }

            if ((CmdSize & AlignMask) != 0) {
                return ConstLoadCommandStorage::Error::CmdSizeNotAligned;
            }

            Iter += CmdSize;
            if (Iter > End) {
                return ConstLoadCommandStorage::Error::SizeOfCmdsTooSmall;
            }

            if (I != (Ncmds - 1)) {
                if (Iter == End) {
                    return ConstLoadCommandStorage::Error::SizeOfCmdsTooSmall;
                }

                I++;
            } else {
                break;
            }
        } while (true);

        if (Iter != End) {
            return ConstLoadCommandStorage::Error::SizeOfCmdsTooSmall;
        }

        *SizeOfCmdsOut = static_cast<uint32_t>(Iter - Begin);
        return ConstLoadCommandStorage::Error::None;
    }

    ConstLoadCommandStorage::ConstLoadCommandStorage(Error Error) noexcept
    : ErrorStorage(Error) {}

    ConstLoadCommandStorage::ConstLoadCommandStorage(const uint8_t *Begin,
                                                     const uint8_t *End,
                                                     const uint32_t Ncmds,
                                                     bool IsBigEndian) noexcept
    : Begin(Begin), End(End), Ncmds(Ncmds), sIsBigEndian(IsBigEndian) {}

    LoadCommandStorage::LoadCommandStorage(Error Error) noexcept
    : ConstLoadCommandStorage(Error) {}

    LoadCommandStorage::LoadCommandStorage(uint8_t *Begin,
                                           uint8_t *End,
                                           uint32_t Ncmds,
                                           bool IsBigEndian) noexcept
    : ConstLoadCommandStorage(Begin, End, Ncmds, IsBigEndian) {}

    ConstLoadCommandStorage
    ConstLoadCommandStorage::Open(const uint8_t *Begin,
                                  uint32_t Ncmds,
                                  uint32_t SizeOfCmds,
                                  bool IsBigEndian,
                                  bool Is64Bit,
                                  bool Verify) noexcept
    {
        if (SizeOfCmds < sizeof(MachO::LoadCommand)) {
            return Error::SizeOfCmdsTooSmall;
        }

        auto End = Begin + SizeOfCmds;
        if (Verify) {
            const auto Error =
                VerifyLoadCommands(Begin,
                                   End,
                                   Ncmds,
                                   IsBigEndian,
                                   Is64Bit,
                                   &SizeOfCmds);

            if (Error != Error::None) {
                return Error;
            }

            End = Begin + SizeOfCmds;
        }

       return ConstLoadCommandStorage(Begin, End, Ncmds, IsBigEndian);
    }
}
