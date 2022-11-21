//
//  Operations/Base.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "Objects/Base.h"
#include "Kind.h"

namespace Operations {
    struct RunResult {
        Objects::OpenError OpenError;
        uint32_t Error = 0;
        Objects::Kind Kind = Objects::Kind::None;

        constexpr explicit RunResult() noexcept = default;
        constexpr explicit RunResult(const Objects::Kind Kind) noexcept
        : Kind(Kind) {}

        constexpr
        RunResult(const Objects::Kind Kind, const uint32_t Error) noexcept
        : Kind(Kind), Error(Error) {}

        constexpr explicit
        RunResult(const uint32_t Error,
                  const Objects::OpenError OpenError) noexcept
        : Error(Error), OpenError(OpenError) {}

        [[nodiscard]] constexpr auto isUnsupportedError() const noexcept {
            return Kind == Objects::Kind::None && Error == 1;
        }

        template <typename T>
        constexpr auto &set(const T Error) noexcept {
            this->Error = static_cast<std::underlying_type_t<T>>(Error);
            return *this;
        }

        template <typename T>
        inline auto &
        set(const T Error, Objects::OpenError OpenError) noexcept {
            this->Error = static_cast<std::underlying_type_t<T>>(Error);
            this->OpenError = OpenError;
            
            return *this;
        }
    };

    constexpr static auto RunResultNone = RunResult();
    constexpr static auto RunResultUnsupported =
        RunResult(Objects::Kind::None, 1);

    struct Base {
    public:
        virtual ~Base() noexcept {}
        virtual RunResult run(const Objects::Base &Base) const noexcept = 0;


        template <typename T>
        auto
        runOnArchOfFatMachO(RunResult &Result,
                            const Objects::FatMachO &Fat,
                            const uint32_t ArchIndex) const noexcept
        {
            if (ArchIndex >= Fat.archCount()) {
                return Result.set(T::InvalidArchIndex);
            }

            const auto ArchObject =
                Fat.getArchObjectAtIndex(static_cast<int32_t>(ArchIndex));

            const auto Error = ArchObject.getError();
            if (!Error.isNone()) {
                if (Error.isUnrecognizedFormat()) {
                    return RunResultUnsupported;
                }

                return Result.set(T::ArchObjectOpenError, Error);
            }

            return run(*ArchObject.getPtr());
        }
    };
}
