//
//  ADT/AddressResolver.h
//  ktool
//
//  Created by suhaspai on 12/16/24.
//

#include "ADT/AddressResolver.h"
#include "Dyld3/ChainedFixups.h"
#include "Mach/Machine.h"
#include "MachO/LoadCommands.h"

namespace ADT {
    static auto
    ParseBindAndRebaseMapFromDyldInfo(
        const ADT::MemoryMap &Map,
        const MachO::DyldInfoCommand &DyldInfo,
        const MachO::SegmentList &SegmentList,
        const bool IsBigEndian,
        const bool Is64Bit,
        MachO::BindActionList::UnorderedMap &BindMap,
        MachO::RebaseActionList::UnorderedMap &RebaseMap,
        AddressResolver::ParseErrorType &ErrorOut) noexcept -> bool
    {
        const auto BindRange = DyldInfo.bindRange(IsBigEndian);
        const auto LazyBindRange = DyldInfo.lazyBindRange(IsBigEndian);
        const auto WeakBindRange = DyldInfo.weakBindRange(IsBigEndian);
        const auto RebaseRange = DyldInfo.rebaseRange(IsBigEndian);

        if (Map.range().contains(BindRange)) {
            const auto BindList =
                MachO::BindActionList(Map, BindRange, SegmentList, Is64Bit);
            const auto ParseResult =
                BindList.getAsUnorderedMap(SegmentList, BindMap);

            if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                ErrorOut =
                    std::pair(MachO::BindInfoKind::Normal,
                              std::move(ParseResult));

                return false;
            }
        }

        if (Map.range().contains(LazyBindRange)) {
            const auto LazyBindList =
                MachO::LazyBindActionList(Map,
                                          LazyBindRange,
                                          SegmentList,
                                          Is64Bit);

            const auto ParseResult =
                LazyBindList.getAsUnorderedMap(SegmentList, BindMap);

            if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                ErrorOut =
                    std::pair(MachO::BindInfoKind::Lazy,
                              std::move(ParseResult));

                return false;
            }
        }

        if (Map.range().contains(WeakBindRange)) {
            const auto WeakBindList =
                MachO::WeakBindActionList(Map,
                                          WeakBindRange,
                                          SegmentList,
                                          Is64Bit);

            const auto ParseResult =
                WeakBindList.getAsUnorderedMap(SegmentList, BindMap);

            if (ParseResult.Error != MachO::BindOpcodeParseError::None) {
                ErrorOut =
                    std::pair(MachO::BindInfoKind::Weak,
                              std::move(ParseResult));

                return false;
            }
        }

        auto RebaseParseResult = MachO::RebaseOpcodeParseResult();
        auto RebaseActionInfoList = MachO::RebaseActionList::UnorderedMap();

        if (Map.range().contains(RebaseRange)) {
            const auto RebaseList = MachO::RebaseActionList(Map, Is64Bit);
            RebaseParseResult =
                RebaseList.getAsUnorderedMap(SegmentList, RebaseMap);

            if (RebaseParseResult.Error != MachO::RebaseOpcodeParseError::None)
            {
                ErrorOut = std::move(RebaseParseResult);
                return false;
            }
        }

        return true;
    }

    static auto
    GetChainedFixupsKind(
        const MemoryMap &Map,
        const MachO::Header &Header,
        const MachO::LinkeditDataCommand *const ChainedFixups) noexcept
            -> Dyld3::ChainedPointerKind
    {
        if (ChainedFixups == nullptr) {
            if (Header.cpuKind() == Mach::CpuKind::Arm64 &&
                static_cast<Mach::Arm64>(Header.cpuSubKind()) ==
                    Mach::Arm64::arm64e)
            {
                return Dyld3::ChainedPointerKind::Arm64e;
            }

            return Dyld3::ChainedPointerKind::None;
        }

        const auto IsBigEndian = Header.isBigEndian();

        const auto FixupsHeaderRange = ChainedFixups->dataRange(IsBigEndian);
        const auto ChainedFixupsHeader =
            Map.get<Dyld3::ChainedFixupsHeader>(FixupsHeaderRange);

        if (ChainedFixupsHeader == nullptr) {
            return Dyld3::ChainedPointerKind::None;
        }

        const auto StartsOffset =
            ChainedFixupsHeader->startsOffset(IsBigEndian);
        const auto Starts =
            Map.get<Dyld3::ChainedStartsInImage>(
                FixupsHeaderRange.front() + StartsOffset);

        if (Starts == nullptr) {
            return Dyld3::ChainedPointerKind::None;
        }

        const auto StartsRange = Starts->segmentOffsetsRange(IsBigEndian);
        const auto StartsSpanOpt =
            Map.getRange<Dyld3::ChainedStartsInImage>(
                ADT::Range::FromSize(StartsOffset,
                                     sizeof(*Starts) + StartsRange.size()));

        if (!StartsSpanOpt.has_value()) {
            return Dyld3::ChainedPointerKind::None;
        }

        const auto SegmentCount = Starts->segmentCount(IsBigEndian);
        for (auto I = uint32_t(); I != SegmentCount; I++) {
            const auto SegOffset = Starts->segmentOffset(I, IsBigEndian);
            if (SegOffset == 0) {
                continue;
            }

            const auto Segment =
                Map.get<Dyld3::ChainedStartsInSegment>(
                    FixupsHeaderRange.front() + StartsOffset + SegOffset);

            if (Segment == nullptr) {
                continue;
            }

            if (Segment->pageCount(IsBigEndian) != 0) {
                return Segment->pointerFormat(IsBigEndian);
            }
        }

        return Dyld3::ChainedPointerKind::None;
    }

    auto
    AddressResolver::FromLoadCommands(
        const MemoryMap Map,
        const MachO::Header &Header,
        const MachO::DyldInfoCommand *const DyldInfo,
        const MachO::LinkeditDataCommand *const ChainedFixups,
        const MachO::SegmentList &SegmentList) noexcept
            -> std::expected<AddressResolver, ParseErrorType>
    {
        const auto ChainedFixupsKind =
            GetChainedFixupsKind(Map, Header, ChainedFixups);

        auto BindMap = MachO::BindActionList::UnorderedMap();
        auto RebaseMap = MachO::RebaseActionList::UnorderedMap();

        const auto IsBigEndian = Header.isBigEndian();
        const auto Is64Bit = Header.is64Bit();

        if (DyldInfo != nullptr) {
            auto BindAndRebaseParseError = ParseErrorType();
            const auto ParseSuccess =
                ParseBindAndRebaseMapFromDyldInfo(Map,
                                                  *DyldInfo,
                                                  SegmentList,
                                                  IsBigEndian,
                                                  Is64Bit,
                                                  BindMap,
                                                  RebaseMap,
                                                  BindAndRebaseParseError);

            if (!ParseSuccess) {
                return std::unexpected(std::move(BindAndRebaseParseError));
            }
        }

        return AddressResolver(std::move(BindMap),
                               std::move(RebaseMap),
                               ChainedFixupsKind);
    }

    [[nodiscard]] auto
    AddressResolver::resolveBind(uint64_t Address,
                                 const uint64_t BaseAddress) const noexcept
        -> std::optional<const MachO::BindActionInfo *>
    {
        if (this->ChainedFixupsKind != Dyld3::ChainedPointerKind::None) {
            if (const auto ResolvedFixup =
                    this->resolveChainedFixup(Address, BaseAddress))
            {
                if (ResolvedFixup->Kind == ResolvedFixupResult::Kind::Rebase) {
                    Address =
                        BaseAddress + ResolvedFixup->Rebase.TargetRuntimeOffset;
                }
            } else {
                return std::nullopt;
            }
        }

        const auto Iter = BindMap.find(Address);
        if (Iter != BindMap.end()) {
            return &Iter->second;
        }

        return nullptr;
    }

    [[nodiscard]] auto
    AddressResolver::resolveChainedFixup(
        const uint64_t Value,
        const uint64_t BaseAddress) const noexcept
            -> std::optional<ResolvedFixupResult>
    {
        switch (this->ChainedFixupsKind) {
            case Dyld3::ChainedPointerKind::None:
                return std::nullopt;
            case Dyld3::ChainedPointerKind::Arm64e:
            case Dyld3::ChainedPointerKind::Arm64eKernel:
            case Dyld3::ChainedPointerKind::Arm64eUserland:
            case Dyld3::ChainedPointerKind::Arm64eFirmware:
            case Dyld3::ChainedPointerKind::Arm64eUserland24: {
                auto Result =
                    ResolvedFixupResult(ResolvedFixupResult::Kind::Bind);

                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                if (!ChainedPtrValue.arm64e.AuthBind.Bind) {
                    Result.Kind = ResolvedFixupResult::Kind::Rebase;
                    if (ChainedPtrValue.arm64e.AuthRebase.Auth) {
                        Result.Rebase.TargetRuntimeOffset =
                            ChainedPtrValue.arm64e.AuthRebase.Target;

                        return Result;
                    }

                    Result.Rebase.TargetRuntimeOffset =
                        ChainedPtrValue.arm64e.Rebase.unpackTarget();

                    if (ChainedFixupsKind ==
                            Dyld3::ChainedPointerKind::Arm64e ||
                        ChainedFixupsKind ==
                            Dyld3::ChainedPointerKind::Arm64eFirmware)
                    {
                        Result.Rebase.TargetRuntimeOffset -= BaseAddress;
                    }

                    return Result;
                }

                if (ChainedPtrValue.arm64e.AuthBind.Auth) {
                    if (ChainedFixupsKind ==
                            Dyld3::ChainedPointerKind::Arm64eUserland24)
                    {
                        Result.Bind.Ordinal =
                            ChainedPtrValue.arm64e.AuthBind24.Ordinal;
                    } else {
                        Result.Bind.Ordinal =
                            ChainedPtrValue.arm64e.AuthBind.Ordinal;
                    }

                    return Result;
                }

                if (ChainedFixupsKind ==
                        Dyld3::ChainedPointerKind::Arm64eUserland24)
                {
                    Result.Bind.Ordinal = ChainedPtrValue.arm64e.Bind24.Ordinal;
                    return Result;
                }

                Result.Bind.Ordinal = ChainedPtrValue.arm64e.Bind.Ordinal;
                return Result;
            }
            case Dyld3::ChainedPointerKind::Bits64:
            case Dyld3::ChainedPointerKind::Bits64Offset: {
                auto Result =
                    ResolvedFixupResult(ResolvedFixupResult::Kind::Bind);

                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                if (ChainedPtrValue.Bind.Bind) {
                    Result.Bind.Ordinal = ChainedPtrValue.Bind.Ordinal;
                    Result.Bind.Addend = ChainedPtrValue.Bind.Addend;

                    return Result;
                }

                Result.Kind = ResolvedFixupResult::Kind::Rebase;
                Result.Rebase.TargetRuntimeOffset =
                    ChainedPtrValue.Rebase.unpackedTarget();

                if (ChainedFixupsKind ==
                        Dyld3::ChainedPointerKind::Bits64Offset)
                {
                    if (Result.Rebase.TargetRuntimeOffset < BaseAddress) {
                        return std::nullopt;
                    }

                    Result.Rebase.TargetRuntimeOffset -= BaseAddress;
                }

                return Result;
            }
            case Dyld3::ChainedPointerKind::Bits32: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer32(Value);
                auto Result =
                    ResolvedFixupResult(ResolvedFixupResult::Kind::Bind);

                if (!ChainedPtrValue.Bind.Bind) {
                    Result.Kind = ResolvedFixupResult::Kind::Rebase;
                    Result.Rebase.TargetRuntimeOffset =
                        ChainedPtrValue.Rebase.Target - BaseAddress;

                    return Result;
                }

                Result.Bind.Ordinal = ChainedPtrValue.Bind.Ordinal;
                Result.Bind.Addend = ChainedPtrValue.Bind.Addend;

                return Result;
            }
            case Dyld3::ChainedPointerKind::Bits32Cache:
                return std::nullopt;
            case Dyld3::ChainedPointerKind::Bits32Firmware: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer32(Value);
                auto Result =
                    ResolvedFixupResult(ResolvedFixupResult::Kind::Bind);

                Result.Rebase.TargetRuntimeOffset =
                    ChainedPtrValue.FirmwareRebase.Target - BaseAddress;

                break;
            }
            case Dyld3::ChainedPointerKind::Bits64KernelCache:
            case Dyld3::ChainedPointerKind::X86_64KernelCache: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                if (!ChainedPtrValue.Bind.Bind) {
                    auto Result =
                        ResolvedFixupResult(ResolvedFixupResult::Kind::Rebase);

                    Result.Rebase.TargetRuntimeOffset =
                        ChainedPtrValue.KernelCache.Target;

                    return Result;
                }

                return std::nullopt;
            }
            case Dyld3::ChainedPointerKind::Arm64eSharedCache: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                auto Result =
                    ResolvedFixupResult(ResolvedFixupResult::Kind::Rebase);

                if (ChainedPtrValue.arm64e.SharedCacheRebase.Auth) {
                    Result.Rebase.TargetRuntimeOffset =
                        ChainedPtrValue.arm64e.SharedCacheAuthRebase
                            .RuntimeOffset;
                } else {
                    Result.Rebase.TargetRuntimeOffset =
                        ChainedPtrValue.arm64e.SharedCacheRebase.RuntimeOffset;
                }

                return Result;
            }
        }

        assert(false && "Unknown ChainedPointerKind");
    }
}