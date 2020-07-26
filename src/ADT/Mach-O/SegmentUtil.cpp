//
//  src/ADT/Mach-O/SegmentUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "LoadCommandStorage.h"
#include "SegmentUtil.h"

namespace MachO {
    template <typename SectionType>
    static bool
    ParseSectionInfo(const SectionType &Section,
                     SectionInfo &InfoIn,
                     bool IsBigEndian,
                     SegmentInfoCollection::Error *ErrorOut) noexcept
    {
        const auto Offset = SwitchEndianIf(Section.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Section.Size, IsBigEndian);
        const auto VmAddr = SwitchEndianIf(Section.Addr, IsBigEndian);

        if (const auto FilRange = LocationRange::CreateWithSize(Offset, Size)) {
            InfoIn.File = FilRange.value();
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        if (const auto MemRange = LocationRange::CreateWithSize(VmAddr, Size)) {
            InfoIn.Memory = MemRange.value();
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        InfoIn.Name = std::string_view(Section.Name, strnlen(Section.Name, 16));
        InfoIn.Flags = Section.getFlags(IsBigEndian);
        InfoIn.Reserved1 = SwitchEndianIf(Section.Reserved1, IsBigEndian);
        InfoIn.Reserved2 = SwitchEndianIf(Section.Reserved2, IsBigEndian);

        return true;
    }

    template <typename SegmentType>
    static bool
    ParseSegmentInfo(const SegmentType &Segment,
                     SegmentInfo &InfoIn,
                     bool IsBigEndian,
                     SegmentInfoCollection::Error *ErrorOut) noexcept
    {
        const auto Offset = SwitchEndianIf(Segment.FileOff, IsBigEndian);
        const auto Size = SwitchEndianIf(Segment.FileSize, IsBigEndian);
        const auto VmAddr = SwitchEndianIf(Segment.VmAddr, IsBigEndian);

        auto FileRange = LocationRange::CreateWithSize(Offset, Size);
        if (!FileRange) {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSegment;
            return false;
        }

        if (const auto MemRange = LocationRange::CreateWithSize(VmAddr, Size)) {
            InfoIn.Memory = MemRange.value();
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSegment;
        }

        InfoIn.File = FileRange.value();
        InfoIn.Name = std::string_view(Segment.Name, strnlen(Segment.Name, 16));

        InfoIn.InitProt = Segment.getInitProt(IsBigEndian);
        InfoIn.MaxProt = Segment.getMaxProt(IsBigEndian);

        InfoIn.Flags = Segment.getFlags(IsBigEndian);

        const auto &SectionList = Segment.GetConstSectionList(IsBigEndian);
        for (const auto &Section : SectionList) {
            auto SectInfo = std::make_unique<SectionInfo>();
            const auto ParseSectResult =
                ParseSectionInfo(Section,
                                 *SectInfo.get(),
                                 IsBigEndian,
                                 ErrorOut);

            if (!ParseSectResult) {
                continue;
            }

            SectInfo->Segment = &InfoIn;
            InfoIn.SectionList.emplace_back(std::move(SectInfo));
        }

        return true;
    }

    void
    SegmentInfoCollection::ParseFromLoadCommands(
        const ConstLoadCommandStorage &LoadCmdStorage,
        bool Is64Bit,
        Error *ErrorOut) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.IsBigEndian();
        auto Error = Error::None;

        for (const auto &LoadCmd : LoadCmdStorage) {
            switch (LoadCmd.getKind(IsBigEndian)) {
                case LoadCommand::Kind::Segment: {
                    if (Is64Bit) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment>(IsBigEndian);

                    if (!ParseSegmentInfo(Segment,
                                          *Info.get(),
                                          IsBigEndian,
                                          &Error))
                    {
                        continue;
                    }

                    List.emplace_back(std::move(Info));
                    break;
                }
                case LoadCommand::Kind::Segment64: {
                    if (!Is64Bit) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment64>(IsBigEndian);

                    if (!ParseSegmentInfo(Segment,
                                          *Info.get(),
                                          IsBigEndian,
                                          &Error))
                    {
                        continue;
                    }

                    List.emplace_back(std::move(Info));
                    break;
                }

                case MachO::LoadCommand::Kind::SymbolTable:
                case MachO::LoadCommand::Kind::SymbolSegment:
                case MachO::LoadCommand::Kind::Thread:
                case MachO::LoadCommand::Kind::UnixThread:
                case MachO::LoadCommand::Kind::LoadFixedVMSharedLibrary:
                case MachO::LoadCommand::Kind::IdFixedVMSharedLibrary:
                case MachO::LoadCommand::Kind::Ident:
                case MachO::LoadCommand::Kind::FixedVMFile:
                case MachO::LoadCommand::Kind::PrePage:
                case MachO::LoadCommand::Kind::DynamicSymbolTable:
                case MachO::LoadCommand::Kind::LoadDylib:
                case MachO::LoadCommand::Kind::IdDylib:
                case MachO::LoadCommand::Kind::LoadDylinker:
                case MachO::LoadCommand::Kind::IdDylinker:
                case MachO::LoadCommand::Kind::PreBoundDylib:
                case MachO::LoadCommand::Kind::Routines:
                case MachO::LoadCommand::Kind::SubFramework:
                case MachO::LoadCommand::Kind::SubUmbrella:
                case MachO::LoadCommand::Kind::SubClient:
                case MachO::LoadCommand::Kind::SubLibrary:
                case MachO::LoadCommand::Kind::TwoLevelHints:
                case MachO::LoadCommand::Kind::PrebindChecksum:
                case MachO::LoadCommand::Kind::LoadWeakDylib:
                case MachO::LoadCommand::Kind::Routines64:
                case MachO::LoadCommand::Kind::Uuid:
                case MachO::LoadCommand::Kind::Rpath:
                case MachO::LoadCommand::Kind::CodeSignature:
                case MachO::LoadCommand::Kind::SegmentSplitInfo:
                case MachO::LoadCommand::Kind::ReexportDylib:
                case MachO::LoadCommand::Kind::LazyLoadDylib:
                case MachO::LoadCommand::Kind::EncryptionInfo:
                case MachO::LoadCommand::Kind::DyldInfo:
                case MachO::LoadCommand::Kind::DyldInfoOnly:
                case MachO::LoadCommand::Kind::LoadUpwardDylib:
                case MachO::LoadCommand::Kind::VersionMinimumMacOSX:
                case MachO::LoadCommand::Kind::VersionMinimumIPhoneOS:
                case MachO::LoadCommand::Kind::FunctionStarts:
                case MachO::LoadCommand::Kind::DyldEnvironment:
                case MachO::LoadCommand::Kind::Main:
                case MachO::LoadCommand::Kind::DataInCode:
                case MachO::LoadCommand::Kind::SourceVersion:
                case MachO::LoadCommand::Kind::DylibCodeSignDRS:
                case MachO::LoadCommand::Kind::EncryptionInfo64:
                case MachO::LoadCommand::Kind::LinkerOption:
                case MachO::LoadCommand::Kind::LinkerOptimizationHint:
                case MachO::LoadCommand::Kind::VersionMinimumTvOS:
                case MachO::LoadCommand::Kind::VersionMinimumWatchOS:
                case MachO::LoadCommand::Kind::Note:
                case MachO::LoadCommand::Kind::BuildVersion:
                case MachO::LoadCommand::Kind::DyldExportsTrie:
                case MachO::LoadCommand::Kind::DyldChainedFixups:
                    break;
            }
        }

        // Check for any overlaps after collecting a list.

        const auto ListEnd = List.cend();
        for (auto It = List.cbegin(); It != ListEnd; It++) {
            const auto &ItSegment = *It;
            for (auto Jt = List.cbegin(); Jt != It; Jt++) {
                const auto &JtSegment = *Jt;
                if (ItSegment->File.overlaps(JtSegment->File)) {
                    Error = Error::OverlappingSegments;
                    goto done;
                }

                if (ItSegment->Memory.overlaps(JtSegment->Memory)) {
                    Error = Error::OverlappingSegments;
                    goto done;
                }
            }

            const auto SectionListEnd = ItSegment->SectionList.cend();
            for (auto SectIter = ItSegment->SectionList.cbegin();
                 SectIter != SectionListEnd;
                 SectIter++)
            {
                const auto &ItSection = *SectIter;
                if (ItSection->File.getBegin() == 0) {
                    continue;
                }

                if (!ItSegment->File.contains(ItSection->File)) {
                    Error = Error::InvalidSection;
                    goto done;
                }

                for (auto SectJter = ItSegment->SectionList.cbegin();
                     SectJter != SectIter;
                     SectJter++)
                {
                    const auto &JtSection = *SectJter;
                    if (ItSection->File.overlaps(JtSection->File)) {
                        Error = Error::OverlappingSections;
                        goto done;
                    }

                    if (ItSection->Memory.overlaps(JtSection->Memory)) {
                        Error = Error::OverlappingSections;
                        goto done;
                    }
                }
            }
        }

    done:
        if (ErrorOut != nullptr) {
            *ErrorOut = Error;
        }
    }

    SegmentInfoCollection
    SegmentInfoCollection::Open(const ConstLoadCommandStorage &LoadCmdStorage,
                                bool Is64Bit,
                                Error *ErrorOut) noexcept
    {
        auto Result = SegmentInfoCollection();
        Result.ParseFromLoadCommands(LoadCmdStorage, Is64Bit, ErrorOut);

        return Result;
    }

    const SegmentInfo *
    SegmentInfoCollection::GetInfoForName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &SegInfo : *this) {
            if (SegInfo->Name == Name) {
                return SegInfo.get();
            }
        }

        return nullptr;
    }

    [[nodiscard]] const SectionInfo *
    SegmentInfo::FindSectionWithName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &SectInfo : SectionList) {
            if (SectInfo->Name == Name) {
                return SectInfo.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfoCollection::FindSectionWithName(
        const std::string_view &SegmentName,
        const std::string_view &Name) const noexcept
    {
        const auto *SegmentInfo = GetInfoForName(SegmentName);
        if (SegmentInfo != nullptr) {
            for (const auto &SectInfo : SegmentInfo->SectionList) {
                if (SectInfo->Name == Name) {
                    return SectInfo.get();
                }
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfoCollection::FindSectionWithName(
        const std::initializer_list<SectionNamePair> &List) const noexcept
    {
        const auto NamePairBegin = List.begin();
        const auto NamePairEnd = List.end();

        for (auto It = NamePairBegin; It != NamePairEnd; It++) {
            auto Segment = GetInfoForName(It->SegmentName);
            if (Segment == nullptr) {
                continue;
            }

            const auto &SectionList = It->SectionList;

            const auto SectionListBegin = SectionList.begin();
            const auto SectionListEnd = SectionList.end();

            for (auto SectionNameIter = SectionListBegin;
                 SectionNameIter != SectionListEnd;
                 SectionNameIter++)
            {
                const auto &SectionName = *SectionNameIter;
                const auto *SectInfo =
                    Segment->FindSectionWithName(SectionName);

                if (SectInfo != nullptr) {
                    return SectInfo;
                }
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfoCollection::GetSectionWithIndex(
        uint64_t SectionIndex) const noexcept
    {
        for (const auto &Segment : List) {
            if (IndexOutOfBounds(SectionIndex, Segment->SectionList.size())) {
                SectionIndex -= Segment->SectionList.size();
                continue;
            }

            return Segment->SectionList.at(SectionIndex).get();
        }

        return nullptr;
    }

    const SegmentInfo *
    SegmentInfoCollection::FindSegmentContainingAddress(
        uint64_t Address) const noexcept
    {
        for (const auto &Segment : *this) {
            if (Segment->Memory.containsLocation(Address)) {
                return Segment.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfo::FindSectionContainingAddress(uint64_t Address) const noexcept {
        for (const auto &Section : SectionList) {
            if (Section->Memory.containsLocation(Address)) {
                return Section.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfo::FindSectionContainingRelativeAddress(
        uint64_t Address) const noexcept
    {
        if (!Memory.containsRelativeLocation(Address)) {
            return nullptr;
        }

        const auto FullAddr = Memory.getBegin() + Address;
        return FindSectionContainingAddress(FullAddr);
    }

    uint8_t *GetDataForSegment(uint8_t *Map, const SegmentInfo &Info) noexcept {
        return (Map + Info.File.getBegin());
    }

    const uint8_t *
    GetDataForSegment(const uint8_t *Map, const SegmentInfo &Info) noexcept {
        return (Map + Info.File.getBegin());
    }

    uint8_t *GetDataForSection(uint8_t *Map, const SectionInfo &Info) noexcept {
        return (Map + Info.File.getBegin());
    }

    const uint8_t *
    GetDataForSection(const uint8_t *Map, const SectionInfo &Info) noexcept {
        return (Map + Info.File.getBegin());
    }

    template <typename PtrType>
    static inline PtrType
    GetDataForVirtualAddrImpl(const SegmentInfoCollection &Collection,
                              const PtrType &Map,
                              uint64_t Addr,
                              uint64_t Size,
                              PtrType *EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = LocationRange::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->Memory.contains(DataRange)) {
                continue;
            }

            for (const auto &Section : Segment->SectionList) {
                if (!Section->Memory.contains(DataRange)) {
                    continue;
                }

                const auto Data = GetDataForSection(Map, *Section.get());
                const auto Offset = (Addr - Section->Memory.getBegin());

                if (!Section->File.containsRelativeLocation(Offset)) {
                    return nullptr;
                }

                if (EndOut != nullptr) {
                    const auto SectionSize = Section->File.size();
                    *EndOut = reinterpret_cast<PtrType>(Data + SectionSize);
                }

                return (Data + Offset);
            }
        }

        return nullptr;
    }

    template <typename PtrType>
    static inline PtrType
    GetDataForVirtualAddrImplNoSections(
        const SegmentInfoCollection &Collection,
        const PtrType &Map,
        uint64_t Addr,
        uint64_t Size,
        PtrType *EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = LocationRange::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->Memory.contains(DataRange)) {
                continue;
            }

            const auto Data = GetDataForSegment(Map, *Segment.get());
            const auto Offset = (Addr - Segment->Memory.getBegin());

            if (EndOut != nullptr) {
                const auto SegmentSize = Segment->File.size();
                *EndOut = reinterpret_cast<PtrType>(Data + SegmentSize);
            }

            return (Data + Offset);
        }

        return nullptr;
    }

    uint8_t *
    SegmentInfoCollection::GetDataForVirtualAddr(
        uint8_t *Map,
        uint64_t Addr,
        uint64_t Size,
        uint8_t **EndOut) const noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImpl<uint8_t *>(*this,
                                                 Map,
                                                 Addr,
                                                 Size,
                                                 EndOut);
        return Data;
    }

    const uint8_t *
    SegmentInfoCollection::GetDataForVirtualAddr(
        const uint8_t *Map,
        uint64_t Addr,
        uint64_t Size,
        const uint8_t **EndOut) const noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImpl<const uint8_t *>(*this,
                                                       Map,
                                                       Addr,
                                                       Size,
                                                       EndOut);
        return Data;
    }

    uint8_t *
    SegmentInfoCollection::GetDataForVirtualAddrIgnoreSections(
        uint8_t *Map,
        uint64_t Addr,
        uint64_t Size,
        uint8_t **EndOut) const noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImplNoSections<uint8_t *>(*this,
                                                           Map,
                                                           Addr,
                                                           Size,
                                                           EndOut);
        return Data;
    }

    const uint8_t *
    SegmentInfoCollection::GetDataForVirtualAddrIgnoreSections(
        const uint8_t *Map,
        uint64_t Addr,
        uint64_t Size,
        const uint8_t **EndOut) const noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImplNoSections<const uint8_t *>(*this,
                                                                 Map,
                                                                 Addr,
                                                                 Size,
                                                                 EndOut);
        return Data;
    }
}
