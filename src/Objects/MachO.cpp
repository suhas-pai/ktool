//
//  Objects/MachO.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "MachO/Header.h"
#include "Objects/MachO.h"

namespace Objects {
    auto MachO::Open(const ADT::MemoryMap &Map) noexcept ->
        ADT::PointerOrError<MachO, OpenError>
    {
        const auto Header = Map.base<::MachO::Header>();
        if (Header == nullptr) {
            const auto Magic = Map.base<::MachO::Magic>();
            const auto Result =
                ::MachO::MagicIsValid(*Magic) ?
                    OpenError::SizeTooSmall : OpenError::WrongFormat;

            return Result;
        }

        if (!::MachO::MagicIsValid(Header->Magic)) {
            return OpenError::WrongFormat;
        }

        return new MachO(Map);
    }
}
