//
//  Objects/Base.cpp
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#include "Objects/Base.h"
#include "Objects/MachO.h"
#include "Objects/DyldSharedCache.h"
#include "Objects/FatMachO.h"
#include "Objects/Open.h"

namespace Objects {
    Base::~Base() noexcept {}

    auto
    Open(const ADT::MemoryMap &Map,
         const std::string_view Path,
         const ADT::FileMap::Prot Prot) noexcept
            -> std::expected<Base *, OpenError>
    {
        const auto Kind = Kind::None;
        switch (Kind) {
            case Kind::None:
            case Kind::DyldSharedCache: {
                const auto ObjectOrError =
                    Objects::DyldSharedCache::Open(Map,
                                                   std::string(Path),
                                                   Prot);

                if (ObjectOrError.has_value()) {
                    return ObjectOrError.value();
                }

                const auto Error = ObjectOrError.error();
                if (Error.Kind != DyldSharedCache::OpenError::WrongFormat) {
                    return std::unexpected(OpenError(Error));
                }

                [[fallthrough]];
            }
            case Kind::MachO: {
                const auto ObjectOrError = Objects::MachO::Open(Map);
                if (ObjectOrError.has_value()) {
                    return ObjectOrError.value();
                }

                const auto Error = ObjectOrError.error();
                if (Error.Kind != MachO::OpenError::WrongFormat) {
                    return std::unexpected(OpenError(Error));
                }

                [[fallthrough]];
            }
            case Kind::FatMachO: {
                const auto ObjectOrError = Objects::FatMachO::Open(Map);
                if (ObjectOrError.has_value()) {
                    return ObjectOrError.value();
                }

                const auto Error = ObjectOrError.error();
                if (Error.Kind != FatMachO::OpenError::WrongFormat) {
                    return std::unexpected(OpenError(Error));
                }
            }
            case Kind::DscImage:
                break;
        }

        return std::unexpected(OpenError::unrecognized());
    }

    auto OpenFrom(const ADT::MemoryMap &Map, const Kind FromKind) noexcept
        -> std::expected<Base *, OpenError>
    {
        const auto Kind = Kind::None;
        switch (Kind) {
            case Kind::None:
            case Kind::MachO: {
                const auto ObjectOrError = Objects::MachO::Open(Map);
                if (ObjectOrError.has_value()) {
                    return ObjectOrError.value();
                }

                const auto Error = ObjectOrError.error();
                if (Error.Kind != MachO::OpenError::WrongFormat) {
                    return std::unexpected(OpenError(Error));
                }

                [[fallthrough]];
            }
            case Kind::FatMachO: {
                // Fat Mach-O Files can't have Fat Mach-O Architectures
                if (FromKind == Kind::FatMachO) {
                    break;
                }

                const auto ObjectOrError = Objects::FatMachO::Open(Map);
                if (ObjectOrError.has_value()) {
                    return ObjectOrError.value();
                }

                const auto Error = ObjectOrError.error();
                if (Error.Kind != FatMachO::OpenError::WrongFormat) {
                    return std::unexpected(OpenError(Error));
                }

                [[fallthrough]];
            }
            case Kind::DyldSharedCache:
                break;
            case Kind::DscImage:
                break;
        }

        return std::unexpected(OpenError::unrecognized());
    }
}
