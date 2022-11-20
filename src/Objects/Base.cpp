//
//  Objects/Base.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "Objects/Base.h"
#include "Objects/MachO.h"

namespace Objects {
    Base::~Base() noexcept {}

    auto Open(const ADT::MemoryMap &Map) noexcept -> Base * {
        const auto Kind = Kind::None;
        switch (Kind) {
            case Kind::None:
            case Kind::MachO: {
                const auto Object = Objects::MachO::Open(Map);
                using ErrorKind = Objects::MachO::OpenError;

                const auto Error = Object.getError();
                if (Error == ErrorKind::None) {
                    return Object.getPtr();
                }

                if (Object.getError() != ErrorKind::WrongFormat) {
                    return nullptr;
                }
            }
        }

        return nullptr;
    }
}
