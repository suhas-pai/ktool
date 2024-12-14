//
//  ADT/Mach-O/SharedLibraryInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cstdint>
#include "LoadCommands.h"

namespace MachO {
    struct SharedLibraryInfo {
    protected:
        LoadCommand::Kind Kind;
        std::string Path;

        uint32_t Index;
        uint32_t Timestamp;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;
    public:
        [[nodiscard]] constexpr auto getKind() const noexcept {
            return this->Kind;
        }

        [[nodiscard]] constexpr auto getPath() const noexcept
            -> std::string_view
        {
            return this->Path;
        }

        [[nodiscard]] constexpr auto getIndex() const noexcept {
            return this->Index;
        }

        [[nodiscard]] constexpr auto getTimestamp() const noexcept {
            return this->Timestamp;
        }

        [[nodiscard]] constexpr auto getCurrentVersion() const noexcept {
            return this->CurrentVersion;
        }

        [[nodiscard]] constexpr auto getCompatVersion() const noexcept {
            return this->CompatVersion;
        }

        constexpr auto setKind(LoadCommandKind Value) noexcept
            -> decltype(*this)
        {
            this->Kind = Value;
            return *this;
        }

        constexpr auto setPath(const std::string_view Path) noexcept
            -> decltype(*this)
        {
            this->Path = Path;
            return *this;
        }

        constexpr auto setIndex(const uint32_t Index) noexcept
            -> decltype(*this)
        {
            this->Index = Index;
            return *this;
        }

        constexpr auto setTimestamp(const uint32_t Timestamp) noexcept
            -> decltype(*this)
        {
            this->Timestamp = Timestamp;
            return *this;
        }

        constexpr auto
        setCurrentVersion(const Dyld3::PackedVersion Value) noexcept
            -> decltype(*this)
        {
            this->CurrentVersion = Value;
            return *this;
        }

        constexpr auto
        setCompatVersion(const Dyld3::PackedVersion Value) noexcept
            -> decltype(*this)
        {
            this->CompatVersion = Value;
            return *this;
        }
    };
}
