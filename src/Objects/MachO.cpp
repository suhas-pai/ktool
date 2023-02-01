//
//  Objects/MachO.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "Objects/MachO.h"
#include "ADT/MemoryMap.h"
#include "MachO/LoadCommands.h"

namespace Objects {
    auto MachO::VerifyMap(const ADT::MemoryMap &Map) noexcept -> OpenError {
        const auto Header = Map.base<::MachO::Header>();
        if (Header == nullptr) {
            const auto Magic = Map.base<::MachO::Magic>();
            if (Magic == nullptr) {
                return OpenError::SizeTooSmall;
            }

            const auto Result =
                ::MachO::MagicIsThin(*Magic) ?
                    OpenError::SizeTooSmall : OpenError::WrongFormat;

            return Result;
        }

        if (!::MachO::MagicIsThin(Header->Magic)) {
            return OpenError::WrongFormat;
        }

        return OpenError::None;
    }

    auto MachO::VerifyLoadCommands(const ADT::MemoryMap &Map) noexcept
        -> OpenError
    {
        const auto Header = Map.base<::MachO::Header, false>();

        const auto LoadCommandsRange = Header->loadCommandsRange();
        const auto MinLoadCommandSize =
            sizeof(::MachO::LoadCommand) * Header->ncmds();

        if (LoadCommandsRange.size() < MinLoadCommandSize) {
            return OpenError::TooManyLoadCommands;
        }

        if (!Map.range().contains(LoadCommandsRange)) {
            return OpenError::TooManyLoadCommands;
        }

        return OpenError::None;
    }

    auto MachO::Open(const ADT::MemoryMap &Map) noexcept
        -> ADT::PointerOrError<MachO, OpenError>
    {
        const auto Header = Map.base<::MachO::Header>();
        if (Header == nullptr) {
            const auto Magic = Map.base<::MachO::Magic>();
            if (Magic == nullptr) {
                return OpenError::SizeTooSmall;
            }

            const auto Result =
                ::MachO::MagicIsThin(*Magic) ?
                    OpenError::SizeTooSmall : OpenError::WrongFormat;

            return Result;
        }

        if (!::MachO::MagicIsThin(Header->Magic)) {
            return OpenError::WrongFormat;
        }

        return new MachO(Map);
    }
}
