//
//  SegmentUtil.cpp
//  stool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "SegmentUtil.h"

namespace MachO {
    template <typename SectionType>
    static bool
    ParseSectionInfo(const SectionType &Section,
                     bool IsBigEndian,
                     SegmentInfoCollectionError *ErrorOut,
                     SectionInfo *InfoOut) noexcept
    {
        const auto Offset = SwitchEndianIf(Section.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Section.Size, IsBigEndian);
        const auto VmAddr = SwitchEndianIf(Section.Addr, IsBigEndian);

        auto FileRange = LocationRange::CreateWithSize(Offset, Size);
        auto Info = SectionInfo();

        if (FileRange) {
            Info.File = FileRange.value();
        } else {
            *ErrorOut = SegmentInfoCollectionError::InvalidSection;
        }

        auto MemRange = LocationRange::CreateWithSize(VmAddr, Size);
        if (MemRange) {
            Info.Memory = MemRange.value();
        } else {
            *ErrorOut = SegmentInfoCollectionError::InvalidSection;
        }

        Info.Name = std::string_view(Section.Name, strnlen(Section.Name, 16));
        *InfoOut = Info;

        return true;
    }

    template <typename SegmentType>
    static bool
    ParseSegmentInfo(const SegmentType &Segment,
                     bool IsBigEndian,
                     SegmentInfoCollectionError *ErrorOut,
                     SegmentInfo *InfoOut) noexcept
    {
        const auto Offset = SwitchEndianIf(Segment.FileOff, IsBigEndian);
        const auto Size = SwitchEndianIf(Segment.FileSize, IsBigEndian);
        const auto VmAddr = SwitchEndianIf(Segment.VmAddr, IsBigEndian);

        auto FileRange = LocationRange::CreateWithSize(Offset, Size);
        if (!FileRange) {
            *ErrorOut = SegmentInfoCollectionError::InvalidSegment;
            return false;
        }

        auto Info = SegmentInfo();
        auto MemRange = LocationRange::CreateWithSize(VmAddr, Size);

        if (MemRange) {
            Info.Memory = MemRange.value();
        } else {
            *ErrorOut = SegmentInfoCollectionError::InvalidSegment;
        }

        Info.File = FileRange.value();
        Info.Name = std::string_view(Segment.Name, strnlen(Segment.Name, 16));

        const auto &SectionList = Segment.GetConstSectionList(IsBigEndian);
        for (const auto &Section : SectionList) {
            auto SectInfo = SectionInfo();
            const auto ParseSectResult =
                ParseSectionInfo(Section,
                                 IsBigEndian,
                                 ErrorOut,
                                 &SectInfo);

            if (!ParseSectResult) {
                continue;
            }

            Info.SectionList.emplace_back(SectInfo);
        }

        *InfoOut = Info;
        return true;
    }


    SegmentInfoCollectionResult
    CollectSegmentInfoList(const ConstLoadCommandStorage &LoadCmdStorage,
                           bool Is64Bit) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.IsBigEndian();

        auto Collection = std::vector<SegmentInfo>();
        auto Error = SegmentInfoCollectionError::None;

        for (const auto &LoadCmd : LoadCmdStorage) {
            switch (LoadCmd.GetKind(IsBigEndian)) {
                case LoadCommand::Kind::Segment: {
                    if (Is64Bit) {
                        continue;
                    }

                    auto Info = SegmentInfo();
                    const auto &Segment =
                        cast<LoadCommand::Kind::Segment>(LoadCmd, IsBigEndian);

                    if (!ParseSegmentInfo(Segment, IsBigEndian, &Error, &Info))
                    {
                        continue;
                    }

                    Collection.emplace_back(Info);
                    break;
                }
                case LoadCommand::Kind::Segment64: {
                    if (!Is64Bit) {
                        continue;
                    }

                    auto Info = SegmentInfo();
                    const auto &Segment =
                        cast<LoadCommand::Kind::Segment64>(LoadCmd,
                                                           IsBigEndian);

                    if (!ParseSegmentInfo(Segment, IsBigEndian, &Error, &Info))
                    {
                        continue;
                    }

                    Collection.emplace_back(Info);
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

        for (auto It = Collection.cbegin(); It != Collection.cend(); It++) {
            const auto &ItSegment = *It;
            for (auto Jt = Collection.cbegin(); Jt != It; Jt++) {
                const auto &JtSegment = *Jt;
                if (ItSegment.File.Overlaps(JtSegment.File)) {
                    Error = SegmentInfoCollectionError::OverlappingSegments;
                    goto done;
                }

                if (ItSegment.Memory.Overlaps(JtSegment.Memory)) {
                    Error = SegmentInfoCollectionError::OverlappingSegments;
                    goto done;
                }
            }

            for (auto SectIter = ItSegment.SectionList.cbegin();
                 SectIter != ItSegment.SectionList.cend();
                 SectIter++)
            {
                const auto &ItSection = *SectIter;
                if (ItSection.File.GetBegin() == 0) {
                    continue;
                }

                if (!ItSegment.File.Contains(ItSection.File)) {
                    Error = SegmentInfoCollectionError::InvalidSection;
                    goto done;
                }

                for (auto SectJter = ItSegment.SectionList.cbegin();
                     SectJter != SectIter;
                     SectJter++)
                {
                    const auto &JtSection = *SectJter;
                    if (ItSection.File.Overlaps(JtSection.File)) {
                        Error = SegmentInfoCollectionError::OverlappingSections;
                        goto done;
                    }

                    if (ItSection.Memory.Overlaps(JtSection.Memory)) {
                        Error = SegmentInfoCollectionError::OverlappingSections;
                        goto done;
                    }
                }
            }
        }

    done:
        return { .Collection = Collection, .Error = Error };
    }

    const SegmentInfo *
    FindSegmentWithName(const std::vector<SegmentInfo> &SegInfoList,
                        const std::string_view &Name) noexcept
    {
        for (const auto &SegInfo : SegInfoList) {
            if (SegInfo.Name == Name) {
                return &SegInfo;
            }
        }

        return nullptr;
    }

    const SectionInfo *
    FindSectionWithName(const std::vector<SegmentInfo> &SegInfoList,
                        const std::string_view &SegmentName,
                        const std::string_view &Name) noexcept
    {
        for (const auto &SegInfo : SegInfoList) {
            for (const auto &SectInfo : SegInfo.SectionList) {
                if (SectInfo.Name == Name) {
                    return &SectInfo;
                }
            }
        }

        return nullptr;
    }

    const SectionInfo *
    FindSectionWithName(
        const std::vector<SegmentInfo> &SegInfoList,
        const std::initializer_list<SectionNamePair> &SectNameList) noexcept
    {
        const auto NamePairBegin = SectNameList.begin();
        const auto NamePairEnd = SectNameList.end();

        for (auto It = NamePairBegin; It != NamePairEnd; It++) {
            for (const auto &SegmentInfo : SegInfoList) {
                if (It->SegmentName != SegmentInfo.Name) {
                    continue;
                }
                
                for (const auto &SectInfo : SegmentInfo.SectionList) {
                    if (It->SectionName == SectInfo.Name) {
                        return &SectInfo;
                    }
                }
            }
        }

        return nullptr;
    }

    uint8_t *
    GetDataForSection(uint8_t *Map, const SectionInfo *Info) noexcept {
        return (Map + Info->File.GetBegin());
    }

    const uint8_t *
    GetDataForSection(const uint8_t *Map, const SectionInfo *Info) noexcept {
         return (Map + Info->File.GetBegin());
    }

    template <typename PtrType>
    static inline PtrType
    GetDataForVirtualAddrImpl(const SegmentInfoCollection &Collection,
                              const PtrType &Map,
                              uint64_t Addr,
                              PtrType *EndOut) noexcept
    {
        for (const auto &Segment : Collection) {
            if (!Segment.Memory.ContainsLocation(Addr)) {
                continue;
            }

            for (const auto &Section : Segment.SectionList) {
                if (!Section.Memory.ContainsLocation(Addr)) {
                    continue;
                }

                const auto Data = GetDataForSection(Map, &Section);
                const auto Offset = (Addr - Section.Memory.GetBegin());

                if (!Section.Memory.ContainsRelativeLocation(Offset)) {
                    return nullptr;
                }

                if (EndOut != nullptr) {
                    const auto SectionSize = Section.File.GetSize();
                    *EndOut = reinterpret_cast<PtrType>(Data + SectionSize);
                }

                return (Data + Offset);
            }
        }

        return nullptr;
    }

    uint8_t *
    GetDataForVirtualAddr(const SegmentInfoCollection &Collection,
                          uint8_t *Map,
                          uint64_t Addr,
                          uint8_t **EndOut) noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImpl<uint8_t *>(Collection,
                                                 Map,
                                                 Addr,
                                                 EndOut);
        return Data;
    }

    const uint8_t *
    GetDataForVirtualAddr(const SegmentInfoCollection &Collection,
                          const uint8_t *Map,
                          uint64_t Addr,
                          const uint8_t **EndOut) noexcept
    {
        const auto Data =
            GetDataForVirtualAddrImpl<const uint8_t *>(Collection,
                                                       Map,
                                                       Addr,
                                                       EndOut);
        return Data;
    }
}
