//
//  Operations/Base.h
//  ktool
//
//  Created by suhaspai on 11/19/22.
//

#pragma once

#include "Objects/Base.h"
#include "Objects/FatMachO.h"

#include "Kind.h"

namespace Operations {
    struct RunResult {
        Objects::OpenError OpenError;
        uint32_t Error = 0;
        Objects::Kind Kind = Objects::Kind::None;

        constexpr explicit RunResult(const Objects::Kind Kind) noexcept
        : Kind(Kind) {}

        constexpr
        RunResult(const Objects::Kind Kind, const uint32_t Error) noexcept
        : Error(Error), Kind(Kind){}

        constexpr explicit
        RunResult(const uint32_t Error,
                  const Objects::OpenError OpenError) noexcept
        : OpenError(OpenError), Error(Error) {}

        [[nodiscard]] constexpr auto isUnsupportedError() const noexcept {
            return Kind == Objects::Kind::None && Error == 1;
        }

        template <typename T>
        constexpr auto set(const T Error) noexcept -> decltype(*this) {
            this->Error = static_cast<std::underlying_type_t<T>>(Error);
            return *this;
        }

        template <typename T>
        inline auto set(const T Error, Objects::OpenError OpenError) noexcept
            -> decltype(*this)
        {
            this->Error = static_cast<std::underlying_type_t<T>>(Error);
            this->OpenError = OpenError;

            return *this;
        }
    };

    constexpr auto RunResultUnsupported = RunResult(Objects::Kind::None, 1);

    struct Base {
    protected:
        explicit Base() noexcept = default;
    public:
        virtual ~Base() noexcept {}

        virtual
        auto supportsObjectKind(Objects::Kind Kind) const noexcept -> bool = 0;

        struct HandleFileOptions {
            int64_t ArchIndex = -1;
            std::string_view Path;
        };

        auto
        runAndHandleFile(HandleFileOptions Options) const noexcept -> RunResult;

        auto
        runAndHandleFile(const Objects::Base &Base,
                         HandleFileOptions Options) const noexcept
            -> RunResult;

        virtual
        auto run(const Objects::Base &Base) const noexcept -> RunResult = 0;
    };
}
