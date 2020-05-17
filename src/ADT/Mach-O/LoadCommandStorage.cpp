//
//  src/ADT/LoadCommandStorage.cpp
//  stool
//
//  Created by Suhas Pai on 3/30/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/LoadCommandStorage.h"

namespace MachO {
    static LoadCommandStorage::Error
    VerifyLoadCommands(const uint8_t *Begin,
                       const uint8_t *End,
                       uint32_t Ncmds,
                       bool IsBigEndian,
                       bool Is64Bit,
                       uint32_t *SizeOfCmdsOut) noexcept
    {
        if (Ncmds == 0) {
            return LoadCommandStorage::Error::NoLoadCommands;
        }

        const auto AlignMask = (Is64Bit) ? 7ull : 3ull;

        auto I = uint32_t();
        auto Iter = Begin;

        do {
            const auto LoadCmd =
                reinterpret_cast<const MachO::LoadCommand *>(Iter);

            const auto CmdSize = SwitchEndianIf(LoadCmd->CmdSize, IsBigEndian);
            if (CmdSize < sizeof(*LoadCmd)) {
                return LoadCommandStorage::Error::CmdSizeTooSmall;
            }

            if (CmdSize & AlignMask) {
                return LoadCommandStorage::Error::CmdSizeNotAligned;
            }

            Iter += CmdSize;
            if (Iter > End) {
                return LoadCommandStorage::Error::SizeOfCmdsTooSmall;
            }

            if (I != (Ncmds - 1)) {
                if (Iter == End) {
                    return LoadCommandStorage::Error::SizeOfCmdsTooSmall;
                }

                I++;
            } else {
                break;
            }
        } while (true);

        if (Iter != End) {
            return LoadCommandStorage::Error::SizeOfCmdsTooSmall;
        }

        *SizeOfCmdsOut = static_cast<uint32_t>(Iter - Begin);
        return LoadCommandStorage::Error::None;
    }

    LoadCommandStorage::LoadCommandStorage(Error Error) noexcept
    : ErrorStorage(Error) {}

    ConstLoadCommandStorage::ConstLoadCommandStorage(Error Error) noexcept
    : ErrorStorage(Error) {}

    LoadCommandStorage::LoadCommandStorage(uint8_t *Begin,
                                           uint8_t *End,
                                           uint32_t Ncmds,
                                           bool IsBigEndian) noexcept
    : Begin(Begin), End(End), Ncmds(Ncmds), mIsBigEndian(IsBigEndian) {}

    ConstLoadCommandStorage::ConstLoadCommandStorage(const uint8_t *Begin,
                                                     const uint8_t *End,
                                                     const uint32_t Ncmds,
                                                     bool IsBigEndian) noexcept
    : Begin(Begin), End(End), Ncmds(Ncmds), mIsBigEndian(IsBigEndian) {}

    LoadCommandStorage
    LoadCommandStorage::Open(uint8_t *Begin,
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

        const auto Obj = LoadCommandStorage(Begin, End, Ncmds, IsBigEndian);
        return Obj;
    }

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

        const auto Obj =
            ConstLoadCommandStorage(Begin, End, Ncmds, IsBigEndian);

        return Obj;
    }
}
