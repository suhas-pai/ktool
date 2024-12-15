//
//  Objects/MachO.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "Objects/MachO.h"
#include "Utils/Overflow.h"

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
        const auto MinLoadCommandSizeOpt =
            Utils::MulAndCheckOverflow(
                sizeof(::MachO::LoadCommand), Header->ncmds());

        if (!MinLoadCommandSizeOpt.has_value()) {
            return OpenError::TooManyLoadCommands;
        }

        if (LoadCommandsRange.size() < MinLoadCommandSizeOpt.value()) {
            return OpenError::TooManyLoadCommands;
        }

        if (!Map.range().contains(LoadCommandsRange)) {
            return OpenError::TooManyLoadCommands;
        }

        return OpenError::None;
    }

    auto MachO::Open(const ADT::MemoryMap &Map) noexcept
        -> std::expected<MachO *, Error>
    {
        const auto Header = Map.base<::MachO::Header>();
        if (Header == nullptr) {
            const auto Magic = Map.base<::MachO::Magic>();
            if (Magic == nullptr) {
                return std::unexpected(Error(OpenError::SizeTooSmall));
            }

            const auto Result =
                ::MachO::MagicIsThin(*Magic) ?
                    OpenError::SizeTooSmall : OpenError::WrongFormat;

            return std::unexpected(Error(Result));
        }

        if (!::MachO::MagicIsThin(Header->Magic)) {
            return std::unexpected(Error(OpenError::WrongFormat));
        }

        return new MachO(Map);
    }

    auto MachO::getMapForFileOffsets() const noexcept -> ADT::MemoryMap {
        return this->map();
    }
}
