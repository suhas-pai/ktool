//
//  ADT/AddressResolver.h
//  ktool
//
//  Created by suhaspai on 12/16/24.
//

#include "ADT/AddressResolver.h"
#include "Dyld3/ChainedFixups.h"
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

        const auto StartsSpanOpt =
            Map.getSpan<Dyld3::ChainedStartsInImage>(
                StartsOffset, Starts->segmentCount(IsBigEndian));

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
        const ADT::MemoryMap &Map,
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

        auto PatchExportMap = PatchLocationMap();
        return AddressResolver(std::move(BindMap),
                               std::move(RebaseMap),
                               ChainedFixupsKind,
                               std::move(PatchExportMap),
                               SegmentList,
                               /*SlideInfoVersion=*/0,
                               /*SlideInfoBaseAddress=*/0,
                               /*BaseAddress=*/0);
    }

    auto
    AddressResolver::ForDscImage(
        const DyldSharedCache::DeVirtualizer &DeVirtualizer,
        const DyldSharedCache::SlideInfoBase *const SlideInfo,
        const Objects::DscImage &Image,
        const MachO::DyldInfoCommand *const DyldInfo,
        const MachO::LinkeditDataCommand *const ChainedFixups,
        const MachO::SegmentList &SegmentList) noexcept
            -> std::expected<AddressResolver, ParseErrorType>
    {
        using Error = enum PatchParseError::Error;

        const auto Map = DeVirtualizer.map();
        const auto &Header = Image.header();

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

        const auto BaseAddressOpt = Image.getBaseAddress();
        if (!BaseAddressOpt.has_value()) {
            return std::unexpected(Error::ImageBaseAddressNotFound);
        }

        const auto BaseAddress = BaseAddressOpt.value();
        const auto PatchInfoOrError =
            DyldSharedCache::PatchInfo::Create(DeVirtualizer,
                                               Image.index(),
                                               BaseAddress,
                                               Image.dsc().header());

        if (!PatchInfoOrError.has_value()) {
            return std::unexpected(PatchInfoOrError.error());
        }

        auto PatchExportMap = PatchLocationMap();

        const auto &PatchInfo = PatchInfoOrError.value();
        const auto PatchParseResult =
            PatchInfo.collectListOfExportPatchesForRange(
                PatchExportMap, Image.getVmRange());

        if (PatchParseResult.Error != PatchParseError::Error::None) {
            return std::unexpected(PatchParseResult);
        }

        auto SlideInfoVersion = static_cast<uint32_t>(0);
        if (SlideInfo != nullptr) {
            SlideInfoVersion = SlideInfo->Version;
        }

        auto SlideInfoBaseAddress = static_cast<uint64_t>(0);
        switch (SlideInfoVersion) {
            case 0:
                break;
            case 1:
            case 3:
            case 5:
                if (const auto BaseAddressOpt = Image.dsc().baseAddress()) {
                    SlideInfoBaseAddress = BaseAddressOpt.value();
                }

                break;
            case 2:
            case 4:
                SlideInfoBaseAddress =
                    static_cast<const ::DyldSharedCache::SlideInfoV2 *>(
                        SlideInfo)->ValueAdd;
                break;
        }

        return AddressResolver(std::move(BindMap),
                               std::move(RebaseMap),
                               ChainedFixupsKind,
                               std::move(PatchExportMap),
                               SegmentList,
                               SlideInfoVersion,
                               SlideInfoBaseAddress,
                               BaseAddress);
    }

    auto
    AddressResolver::resolve(uint64_t Address, uint64_t Value) const noexcept
        -> std::optional<Resolution>
    {
        if (const auto PatchIter = this->PatchExportMap.find(Address);
            PatchIter != this->PatchExportMap.end())
        {
            return Resolution(std::move(PatchIter->second));
        }

        if (this->SlideInfoVersion != 0) {
            const auto Version =
                ::DyldSharedCache::SlideInfoVersion(this->SlideInfoVersion);

            switch (Version) {
                case DyldSharedCache::SlideInfoVersion::V1:
                    break;
                case DyldSharedCache::SlideInfoVersion::V2: {
                    const auto DeltaMask = 0x00FFFF0000000000ull;
                    Value &= ~DeltaMask;

                    break;
                }
                case DyldSharedCache::SlideInfoVersion::V4: {
                    const auto DeltaMask = 0x00000000C0000000ull;
                    Value = static_cast<uint32_t>(Value) & ~DeltaMask;

                    break;
                }
                case DyldSharedCache::SlideInfoVersion::V3:
                    if (const auto ResolvedFixupOpt =
                            this->resolveChainedFixup(
                                Dyld3::ChainedPointerKind::Arm64e,
                                Value,
                                this->SlideInfoBaseAddress))
                    {
                        auto &Resolution = ResolvedFixupOpt.value();
                        if (Resolution.Kind == Resolution::Kind::Rebase) {
                            return std::move(Resolution);
                        }
                    } else {
                        return std::nullopt;
                    }

                    break;
                case DyldSharedCache::SlideInfoVersion::V5:
                    if (const auto ResolvedFixupOpt =
                            this->resolveChainedFixup(
                                Dyld3::ChainedPointerKind::Arm64eSharedCache,
                                Value,
                                this->SlideInfoBaseAddress))
                    {
                        auto &Resolution = ResolvedFixupOpt.value();
                        if (Resolution.Kind == Resolution::Kind::Rebase) {
                            return std::move(Resolution);
                        }
                    } else {
                        return std::nullopt;
                    }

                    break;
            }
        } else if (this->ChainedFixupsKind != Dyld3::ChainedPointerKind::None) {
            if (const auto ResolvedFixupOpt =
                    this->resolveChainedFixup(this->ChainedFixupsKind,
                                              Value,
                                              this->ImageBaseAddress))
            {
                auto &Resolution = ResolvedFixupOpt.value();
                if (Resolution.Kind == Resolution::Kind::Rebase) {
                    return std::move(Resolution);
                }
            } else {
                return std::nullopt;
            }
        }

        const auto Iter = this->BindMap.find(Address);
        if (Iter != this->BindMap.end()) {
            const auto &BindInfo = Iter->second;
            if (const auto FullAddressOpt =
                    BindInfo.getFullAddress(this->SegmentList))
            {
                return Resolution(BindInfo, FullAddressOpt.value());
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] auto
    AddressResolver::resolveChainedFixup(
        const Dyld3::ChainedPointerKind ChainedFixupsKind,
        const uint64_t Value,
        const uint64_t BaseAddress) const noexcept -> std::optional<Resolution>
    {
        switch (ChainedFixupsKind) {
            case Dyld3::ChainedPointerKind::None:
                return std::nullopt;
            case Dyld3::ChainedPointerKind::Arm64e:
            case Dyld3::ChainedPointerKind::Arm64eKernel:
            case Dyld3::ChainedPointerKind::Arm64eUserland:
            case Dyld3::ChainedPointerKind::Arm64eFirmware:
            case Dyld3::ChainedPointerKind::Arm64eUserland24: {
                auto Result = Resolution(Resolution::Kind::Bind);
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);

                if (!ChainedPtrValue.arm64e.AuthBind.Bind) {
                    Result.Kind = Resolution::Kind::Rebase;
                    if (ChainedPtrValue.arm64e.AuthRebase.Auth) {
                        Result.Rebase.FullAddress =
                            BaseAddress +
                            ChainedPtrValue.arm64e.AuthRebase.Target;

                        return std::move(Result);
                    }

                    Result.Rebase.FullAddress =
                        ChainedPtrValue.arm64e.Rebase.unpackTarget();

                    if (ChainedFixupsKind !=
                            Dyld3::ChainedPointerKind::Arm64e &&
                        ChainedFixupsKind !=
                            Dyld3::ChainedPointerKind::Arm64eFirmware)
                    {
                        Result.Rebase.FullAddress += BaseAddress;
                    }

                    return std::move(Result);
                }

                if (ChainedPtrValue.arm64e.AuthBind.Auth) {
                    if (ChainedFixupsKind ==
                            Dyld3::ChainedPointerKind::Arm64eUserland24)
                    {
                        Result.Bind.Info.DylibOrdinal =
                            ChainedPtrValue.arm64e.AuthBind24.Ordinal;
                    } else {
                        Result.Bind.Info.DylibOrdinal =
                            ChainedPtrValue.arm64e.AuthBind.Ordinal;
                    }

                    return std::move(Result);
                }

                if (ChainedFixupsKind ==
                        Dyld3::ChainedPointerKind::Arm64eUserland24)
                {
                    Result.Bind.Info.DylibOrdinal =
                        ChainedPtrValue.arm64e.Bind24.Ordinal;

                    return std::move(Result);
                }

                Result.Bind.Info.DylibOrdinal =
                    ChainedPtrValue.arm64e.Bind.Ordinal;

                return std::move(Result);
            }
            case Dyld3::ChainedPointerKind::Bits64:
            case Dyld3::ChainedPointerKind::Bits64Offset: {
                auto Result = Resolution(Resolution::Kind::Bind);
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);

                if (ChainedPtrValue.Bind.Bind) {
                    Result.Bind.Info.Addend = ChainedPtrValue.Bind.Addend;
                    Result.Bind.Info.DylibOrdinal =
                        ChainedPtrValue.Bind.Ordinal;

                    return std::move(Result);
                }

                Result.Kind = Resolution::Kind::Rebase;
                Result.Rebase.FullAddress =
                    ChainedPtrValue.Rebase.unpackedTarget();

                if (ChainedFixupsKind ==
                        Dyld3::ChainedPointerKind::Bits64Offset)
                {
                    Result.Rebase.FullAddress += BaseAddress;
                }

                return std::move(Result);
            }
            case Dyld3::ChainedPointerKind::Bits32: {
                auto Result = Resolution(Resolution::Kind::Bind);
                const auto ChainedPtrValue =
                    Dyld3::ChainedPointer32(static_cast<uint32_t>(Value));

                if (!ChainedPtrValue.Bind.Bind) {
                    Result.Kind = Resolution::Kind::Rebase;
                    Result.Rebase.FullAddress =
                        BaseAddress + ChainedPtrValue.Rebase.Target;

                    return std::move(Result);
                }

                Result.Bind.Info.DylibOrdinal = ChainedPtrValue.Bind.Ordinal;
                Result.Bind.Info.Addend = ChainedPtrValue.Bind.Addend;

                return std::move(Result);
            }
            case Dyld3::ChainedPointerKind::Bits32Cache:
                return std::nullopt;
            case Dyld3::ChainedPointerKind::Bits32Firmware: {
                auto Result = Resolution(Resolution::Kind::Bind);
                const auto ChainedPtrValue =
                    Dyld3::ChainedPointer32(static_cast<uint32_t>(Value));

                Result.Bind.FullAddress =
                    BaseAddress + ChainedPtrValue.FirmwareRebase.Target;

                break;
            }
            case Dyld3::ChainedPointerKind::Bits64KernelCache:
            case Dyld3::ChainedPointerKind::X86_64KernelCache: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                if (!ChainedPtrValue.Bind.Bind) {
                    const auto FullOffset =
                        BaseAddress + ChainedPtrValue.KernelCache.Target;

                    return Resolution(Resolution::Kind::Rebase, FullOffset);
                }

                return std::nullopt;
            }
            case Dyld3::ChainedPointerKind::Arm64eSharedCache: {
                const auto ChainedPtrValue = Dyld3::ChainedPointer64(Value);
                auto Result = Resolution(Resolution::Kind::Rebase);

                if (ChainedPtrValue.arm64e.SharedCacheRebase.Auth) {
                    Result.Rebase.FullAddress =
                        BaseAddress +
                        ChainedPtrValue.arm64e.SharedCacheAuthRebase
                            .RuntimeOffset;
                } else {
                    Result.Rebase.FullAddress =
                        BaseAddress +
                        ChainedPtrValue.arm64e.SharedCacheRebase.RuntimeOffset;
                }

                return std::move(Result);
            }
        }

        assert(false && "Unknown ChainedPointerKind");
    }

    auto AddressResolver::resolveRebase(uint64_t Value) const noexcept
        -> std::optional<Resolution>
    {
        auto Result = Resolution(Resolution::Kind::Rebase);
        Result.Rebase.FullAddress = Value + this->ImageBaseAddress;

        return Result;
    }
}
