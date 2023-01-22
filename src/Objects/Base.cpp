//
//  Objects/Base.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "Objects/Base.h"
#include "Objects/MachO.h"
#include "Objects/FatMachO.h"

namespace Objects {
    Base::~Base() noexcept {}

    auto Open(const ADT::MemoryMap &Map) noexcept -> OpenResult {
        const auto Kind = Kind::None;
        switch (Kind) {
            case Kind::None:
            case Kind::MachO: {
                const auto Object = Objects::MachO::Open(Map);
                using ErrorKind = Objects::MachO::OpenError;

                const auto Error = Object.error();
                if (Error == ErrorKind::None) {
                    return Object.ptr();
                }

                if (Error != ErrorKind::WrongFormat) {
                    return OpenError(Kind::MachO, static_cast<uint32_t>(Error));
                }

                [[fallthrough]];
            }
            case Kind::FatMachO: {
                const auto Object = Objects::FatMachO::Open(Map);
                using ErrorKind = Objects::FatMachO::OpenError;

                const auto Error = Object.error();
                if (Error == ErrorKind::None) {
                    return Object.ptr();
                }

                if (Error!= ErrorKind::WrongFormat) {
                    return OpenError(Kind::FatMachO,
                                     static_cast<uint32_t>(Error));
                }
            }
        }

        return OpenErrorUnrecognized;
    }

    auto
    OpenFrom(const ADT::MemoryMap &Map, const Kind FromKind) noexcept
        -> OpenResult
    {
        const auto Kind = Kind::None;
        switch (Kind) {
            case Kind::None:
            case Kind::MachO: {
                const auto Object = Objects::MachO::Open(Map);
                using ErrorKind = Objects::MachO::OpenError;

                const auto Error = Object.error();
                if (Error == ErrorKind::None) {
                    return Object.ptr();
                }

                if (Error != ErrorKind::WrongFormat) {
                    return OpenError(Kind::MachO, static_cast<uint32_t>(Error));
                }

                [[fallthrough]];
            }
            case Kind::FatMachO: {
                // Fat Mach-O Files can't have Fat Mach-O Architectures
                if (FromKind == Kind::FatMachO) {
                    break;
                }

                const auto Object = Objects::FatMachO::Open(Map);
                using ErrorKind = Objects::FatMachO::OpenError;

                const auto Error = Object.error();
                if (Error == ErrorKind::None) {
                    return Object.ptr();
                }

                if (Error != ErrorKind::WrongFormat) {
                    return OpenError(Kind::FatMachO,
                                     static_cast<uint32_t>(Error));
                }
            }
        }

        return OpenErrorUnrecognized;
    }
}
