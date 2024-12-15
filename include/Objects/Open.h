//
//  Objects/Open.h
//  ktool
//
//  Created by suhaspai on 12/14/24.
//

#pragma once

#include "DscImage.h"
#include "DyldSharedCache.h"
#include "FatMachO.h"
#include "MachO.h"

namespace Objects {
    struct OpenError {
    private:
        explicit OpenError(const Objects::Kind Kind) noexcept : Kind(Kind) {}
    public:
        Objects::Kind Kind = Kind::None;
        union {
            DyldSharedCache::Error DscError;
            DscImage::Error DscImageError;
            FatMachO::Error FatMachOError;
            MachO::Error MachOError;
        };

        constexpr OpenError(const DyldSharedCache::Error &Error) noexcept
        : Kind(Objects::Kind::DyldSharedCache), DscError(Error) {}

        constexpr OpenError(const DscImage::Error &Error) noexcept
        : Kind(Objects::Kind::DscImage), DscImageError(Error) {}

        constexpr OpenError(const FatMachO::Error &Error) noexcept
        : Kind(Objects::Kind::FatMachO), FatMachOError(Error) {}

        constexpr OpenError(const MachO::Error &Error) noexcept
        : Kind(Objects::Kind::MachO), MachOError(Error) {}

        static inline auto unrecognized() noexcept {
            return OpenError(Objects::Kind::None);
        }

        [[nodiscard]] constexpr auto isUnrecognizedFormat() const noexcept {
            return Kind == Objects::Kind::None;
        }

        [[nodiscard]] constexpr auto isWrongFormat() const noexcept {
            switch (Kind) {
                case Objects::Kind::None:
                    assert(0 && "isWrongFormat called on Object None");
                case Objects::Kind::DyldSharedCache:
                    return DscError.Kind ==
                        DyldSharedCache::OpenError::WrongFormat;
                case Objects::Kind::MachO:
                    return MachOError.Kind == MachO::OpenError::WrongFormat;
                case Objects::Kind::DscImage:
                    return DscImageError.Kind ==
                        DscImage::OpenError::WrongFormat;
                case Kind::FatMachO:
                    return FatMachOError.Kind ==
                        FatMachO::OpenError::WrongFormat;
            }

            assert(0 && "isWrongFormat called on Object with unknown Kind");
        }
    };

    auto
    Open(const ADT::MemoryMap &Map,
         std::string_view Path,
         ADT::FileMap::Prot Prot) noexcept
            -> std::expected<Base *, OpenError>;

    auto OpenFrom(const ADT::MemoryMap &Map, const Kind FromKind) noexcept
        -> std::expected<Base *, OpenError>;

    auto OpenArch(const FatMachO &MachO, const uint32_t Index) noexcept
        -> std::expected<Base *, OpenError>;

    auto OpenImage(const DyldSharedCache &Dsc, const uint32_t Index) noexcept
        -> std::expected<Base *, OpenError>;
}
