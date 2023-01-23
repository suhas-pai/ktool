/*
 * MachO/LibraryList.h
 * © suhas pai
 */

#pragma once
#include <string>

#include "Dyld3/PackedVersion.h"
#include "MachO/LoadCommands.h"
#include "MachO/LoadCommandsMap.h"

namespace MachO {
    struct LibraryInfo {
        LoadCommandKind Kind;
        std::optional<std::string> Path;

        Dyld3::PackedVersion CurrentVersion;
        Dyld3::PackedVersion CompatVersion;

        uint32_t Index;
        uint32_t Timestamp;
    };

    struct LibraryList {
    protected:
        std::vector<LibraryInfo> List;
    public:
        LibraryList() noexcept = default;
        LibraryList(const MachO::LoadCommandsMap &Map,
                    bool IsBigEndian) noexcept;

        constexpr auto
        addLibrary(const MachO::DylibCommand &Dylib,
                   const bool IsBigEndian) noexcept -> decltype(*this)
        {
            const auto PathOpt = Dylib.name(IsBigEndian);
            List.emplace_back(LibraryInfo {
                .Kind = Dylib.kind(IsBigEndian),
                .Path =
                    PathOpt.has_value() ?
                        std::string(PathOpt.value()) :
                        std::optional<std::string>(std::nullopt),
                .CurrentVersion = Dylib.currentVersion(IsBigEndian),
                .CompatVersion = Dylib.compatVersion(IsBigEndian),
                .Index = static_cast<uint32_t>(List.size()),
                .Timestamp = Dylib.timestamp(IsBigEndian),
            });

            return *this;
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return List.empty();
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return List.size();
        }

        [[nodiscard]] constexpr
        auto &at(const decltype(List)::size_type Index) const noexcept {
            return List.at(Index);
        }
    };
}