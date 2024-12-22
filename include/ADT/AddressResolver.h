//
//  ADT/AddressResolver.h
//  © suhas pai
//
//  Created by suhas pai on 12/16/24.
//

#pragma once

#include <expected>
#include <variant>

#include "ADT/MemoryMap.h"
#include "Dyld3/ChainedFixups.h"

#include "MachO/BindInfo.h"
#include "MachO/Header.h"
#include "MachO/LoadCommands.h"
#include "MachO/RebaseInfo.h"
#include "MachO/SegmentList.h"

namespace ADT {
    struct AddressResolver {
    protected:
        MachO::BindActionList::UnorderedMap BindMap;
        MachO::RebaseActionList::UnorderedMap RebaseMap;

        Dyld3::ChainedPointerKind ChainedFixupsKind;

        explicit
        AddressResolver(
            MachO::BindActionList::UnorderedMap &&BindMap,
            MachO::RebaseActionList::UnorderedMap &&RebaseMap,
            Dyld3::ChainedPointerKind ChainedFixupsKind) noexcept
        : BindMap(std::move(BindMap)), RebaseMap(std::move(RebaseMap)),
          ChainedFixupsKind(ChainedFixupsKind) {}
    public:
        using BindParseError =
            std::pair<MachO::BindInfoKind, MachO::BindOpcodeParseResult>;

        using RebaseParseError = MachO::RebaseOpcodeParseResult;
        using ParseErrorType = std::variant<BindParseError, RebaseParseError>;

        static auto
        FromLoadCommands(MemoryMap Map,
                         const MachO::Header &Header,
                         const MachO::DyldInfoCommand *DyldInfo,
                         const MachO::LinkeditDataCommand *ChainedFixups,
                         const MachO::SegmentList &SegmentList) noexcept
            -> std::expected<AddressResolver, ParseErrorType>;

        [[nodiscard]]
        auto resolveBind(uint64_t Address, uint64_t BaseAddress) const noexcept
            -> std::optional<const MachO::BindActionInfo *>;

        struct ResolvedFixupResult {
            enum class Kind {
                Bind,
                Rebase
            };

            Kind Kind;
            union {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wnested-anon-types"
                struct {
                    uint64_t Ordinal = 0;
                    uint64_t Addend = 0;
                } Bind;
                struct {
                    uint64_t TargetRuntimeOffset = 0;
                } Rebase;
            #pragma clang diagnostic pop
            };

            explicit ResolvedFixupResult(const enum Kind Kind) noexcept
            : Kind(Kind) {}

            ~ResolvedFixupResult() noexcept {}
        };

        [[nodiscard]] auto
        resolveChainedFixup(uint64_t Value, uint64_t BaseAddress) const noexcept
            -> std::optional<ResolvedFixupResult>;

        [[nodiscard]]
        auto resolve(uint64_t Address, uint64_t Value) const noexcept
            -> std::optional<uint64_t>;
    };
}
