//
//  ADT/AddressResolver.h
//  © suhas pai
//
//  Created by suhas pai on 12/16/24.
//

#pragma once

#include <expected>
#include <variant>

#include "Dyld3/ChainedFixups.h"

#include "DyldSharedCache/DeVirtualizer.h"
#include "DyldSharedCache/PatchInfo.h"

#include "MachO/BindInfo.h"
#include "MachO/Header.h"
#include "MachO/LoadCommands.h"
#include "MachO/RebaseInfo.h"
#include "MachO/SegmentList.h"

#include "Objects/DscImage.h"

namespace ADT {
    struct AddressResolver {
    public:
        struct Resolution {
            enum class Kind {
                None,
                Bind,
                Rebase,
                Patch
            };

            Kind Kind;
            union {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wnested-anon-types"
                struct {
                    MachO::BindActionInfo Info;
                    uint64_t FullAddress;
                } Bind;
                struct {
                    uint64_t FullAddress = 0;
                } Rebase;

                struct {
                    DyldSharedCache::PatchInfo::PatchLocation PatchLoc;
                } Patch;
            #pragma clang diagnostic pop
            };

            explicit Resolution(enum Kind Kind) noexcept : Kind(Kind) {}

            explicit
            Resolution(
                const DyldSharedCache::PatchInfo::PatchLocation &Loc) noexcept
            : Kind(Kind::Patch), Patch(Loc) {}

            constexpr explicit
            Resolution(const MachO::BindActionInfo &BindInfo,
                       const uint64_t FullAddress) noexcept
            : Kind(Kind::Bind), Bind(BindInfo, FullAddress) {}

            constexpr explicit
            Resolution(const enum Kind Kind,
                       const uint64_t FullAddress) noexcept
            : Kind(Kind), Rebase(FullAddress) {
                assert(Kind == Kind::Rebase);
            }

            ~Resolution() noexcept {}
        };
    protected:
        using PatchLocationMap = DyldSharedCache::PatchInfo::PatchLocationMap;

        MachO::BindActionList::UnorderedMap BindMap;
        MachO::RebaseActionList::UnorderedMap RebaseMap;
        Dyld3::ChainedPointerKind ChainedFixupsKind;

        uint32_t SlideInfoVersion;

        PatchLocationMap PatchExportMap;
        MachO::SegmentList SegmentList;

        uint64_t SlideInfoBaseAddress;
        uint64_t ImageBaseAddress;

        explicit
        AddressResolver(
            MachO::BindActionList::UnorderedMap &&BindMap,
            MachO::RebaseActionList::UnorderedMap &&RebaseMap,
            Dyld3::ChainedPointerKind ChainedFixupsKind,
            PatchLocationMap &&PatchExportMap,
            const MachO::SegmentList &SegmentList,
            const uint32_t SlideInfoVersion,
            const uint64_t SlideInfoBaseAddress,
            const uint64_t BaseAddress) noexcept
        : BindMap(std::move(BindMap)), RebaseMap(std::move(RebaseMap)),
          ChainedFixupsKind(ChainedFixupsKind),
          SlideInfoVersion(SlideInfoVersion),
          PatchExportMap(std::move(PatchExportMap)), SegmentList(SegmentList),
          SlideInfoBaseAddress(SlideInfoBaseAddress),
          ImageBaseAddress(BaseAddress) {}

        [[nodiscard]] auto
        resolveChainedFixup(Dyld3::ChainedPointerKind ChainedFixupsKind,
                            uint64_t Value,
                            uint64_t BaseAddress) const noexcept
            -> std::optional<Resolution>;

        [[nodiscard]] auto resolveRebase(uint64_t Value) const noexcept
            -> std::optional<Resolution>;
    public:
        using BindParseError =
            std::pair<MachO::BindInfoKind, MachO::BindOpcodeParseResult>;

        using RebaseParseError = MachO::RebaseOpcodeParseResult;
        using PatchParseError = DyldSharedCache::PatchInfo::ParseResult;

        using ParseErrorType =
            std::variant<BindParseError, RebaseParseError, PatchParseError>;

        static auto
        FromLoadCommands(const ADT::MemoryMap Map,
                         const MachO::Header &Header,
                         const MachO::DyldInfoCommand *DyldInfo,
                         const MachO::LinkeditDataCommand *ChainedFixups,
                         const MachO::SegmentList &SegmentList) noexcept
            -> std::expected<AddressResolver, ParseErrorType>;

        static auto
        ForDscImage(const DyldSharedCache::DeVirtualizer &DeVirtualizer,
                    const DyldSharedCache::SlideInfoBase *SlideInfo,
                    const Objects::DscImage &DscImage,
                    const MachO::DyldInfoCommand *DyldInfo,
                    const MachO::LinkeditDataCommand *ChainedFixups,
                    const MachO::SegmentList &SegmentList) noexcept
            -> std::expected<AddressResolver, ParseErrorType>;

        [[nodiscard]]
        auto resolve(uint64_t Address, uint64_t Value) const noexcept
            -> std::optional<Resolution>;
    };
}
