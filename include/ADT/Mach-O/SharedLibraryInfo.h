//
//  include/ADT/Mach-O/SharedLibraryInfo.h
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
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

        PackedVersion CurrentVersion;
        PackedVersion CompatVersion;
    public:
        [[nodiscard]]
        constexpr LoadCommand::Kind getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]]
        constexpr std::string_view getPath() const noexcept {
            return Path;
        }

        [[nodiscard]] constexpr uint32_t getIndex() const noexcept {
            return Index;
        }

        [[nodiscard]] constexpr uint32_t getTimestamp() const noexcept {
            return Timestamp;
        }

        [[nodiscard]]
        constexpr PackedVersion getCurrentVersion() const noexcept {
            return CurrentVersion;
        }

        [[nodiscard]]
        constexpr PackedVersion getCompatVersion() const noexcept {
            return CompatVersion;
        }

        constexpr
        inline SharedLibraryInfo &setKind(LoadCommandKind Value) noexcept {
            this->Kind = Value;
            return *this;
        }

        constexpr
        inline SharedLibraryInfo &setPath(const std::string &Value) noexcept {
            this->Path = Value;
            return *this;
        }

        constexpr SharedLibraryInfo &setIndex(uint32_t Value) noexcept {
            this->Index = Value;
            return *this;
        }

        constexpr
        inline SharedLibraryInfo &setTimestamp(uint32_t Value) noexcept {
            this->Timestamp = Value;
            return *this;
        }

        constexpr
        SharedLibraryInfo &setCurrentVersion(PackedVersion Value) noexcept {
            this->CurrentVersion = Value;
            return *this;
        }

        constexpr
        SharedLibraryInfo &setCompatVersion(PackedVersion Value) noexcept {
            this->CompatVersion = Value;
            return *this;
        }
    };
}
