//
//  include/ADT/Mach-O/SharedLibraryUtil.h
//  ktool
//
//  Created by Suhas Pai on 5/21/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "SharedLibraryInfo.h"

namespace MachO {
    [[nodiscard]] inline
    bool IsSharedLibraryLoadCommand(const LoadCommand::Kind &Kind) noexcept;

    struct ConstLoadCommandStorage;
    struct SharedLibraryInfoCollection {
    public:
        enum class Error {
            None,
            InvalidPath
        };
    protected:
        std::vector<std::unique_ptr<SharedLibraryInfo>> List;
        SharedLibraryInfoCollection() noexcept = default;
    public:
        [[nodiscard]] static SharedLibraryInfoCollection
        Open(const ConstLoadCommandStorage &LoadCmdStorage,
             Error *ErrorOut) noexcept;

        [[nodiscard]]
        const SharedLibraryInfo &at(uint64_t Index) const noexcept {
            assert(!IndexOutOfBounds(Index, this->size()));
            return *List.at(Index).get();
        }

        [[nodiscard]]
        const SharedLibraryInfo &atOrdinal(uint64_t Ordinal) const noexcept {
            assert(Ordinal != 0);

            const auto Index = (Ordinal - 1);
            return at(Index);
        }

        [[nodiscard]] inline bool empty() const noexcept {
            return List.empty();
        }

        [[nodiscard]] inline uint64_t size() const noexcept {
            return List.size();
        }

        [[nodiscard]]
        inline decltype(List)::const_iterator begin() const noexcept {
            return List.cbegin();
        }

        [[nodiscard]]
        inline decltype(List)::const_iterator end() const noexcept {
            return List.cend();
        }
    };
}


