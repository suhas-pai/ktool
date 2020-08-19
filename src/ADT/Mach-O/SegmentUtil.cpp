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
    [[nodiscard]] static bool
    ParseSectionInfo(const SectionType &Section,
                     SectionInfo &InfoIn,
                     bool IsBigEndian,
                     SegmentInfoCollection::Error *ErrorOut) noexcept
    {
        const auto Offset = SwitchEndianIf(Section.Offset, IsBigEndian);
        const auto Size = SwitchEndianIf(Section.Size, IsBigEndian);
        const auto VmAddr = SwitchEndianIf(Section.Addr, IsBigEndian);

        if (const auto FilRange = LocationRange::CreateWithSize(Offset, Size)) {
            InfoIn.setFileRange(FilRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        if (const auto MemRange = LocationRange::CreateWithSize(VmAddr, Size)) {
            InfoIn.setMemoryRange(MemRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        auto Name = std::string(Section.Name, strnlen(Section.Name, 16));

        InfoIn.setName(std::move(Name));
        InfoIn.setFlags(Section.getFlags(IsBigEndian));
        InfoIn.setReserved1(SwitchEndianIf(Section.Reserved1, IsBigEndian));
        InfoIn.setReserved2(SwitchEndianIf(Section.Reserved2, IsBigEndian));

        return true;
    }

    template <typename SegmentType>
    [[nodiscard]] static bool
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
            InfoIn.setMemoryRange(MemRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSegment;
        }

        auto Name = std::string(Segment.Name, strnlen(Segment.Name, 16));

        InfoIn.setFileRange(FileRange.value());
        InfoIn.setName(std::move(Name));

        InfoIn.setInitProt(Segment.getInitProt(IsBigEndian));
        InfoIn.setMaxProt(Segment.getMaxProt(IsBigEndian));
        InfoIn.setFlags(Segment.getFlags(IsBigEndian));

        auto SectionOutList = SegmentInfo::SectionListType();
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

            SectInfo->setSegment(&InfoIn);
            SectionOutList.emplace_back(std::move(SectInfo));
        }

        InfoIn.setSectionList(std::move(SectionOutList));
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

                default:
                    break;
            }
        }

        // Check for any overlaps after collecting a list.

        const auto ListEnd = List.cend();
        for (auto It = List.cbegin(); It != ListEnd; It++) {
            const auto &ItSegment = *It;
            for (auto Jt = List.cbegin(); Jt != It; Jt++) {
                const auto &JtSegment = *Jt;
                const auto JtSegFileRange = JtSegment->getFileRange();

                if (ItSegment->getFileRange().overlaps(JtSegFileRange)) {
                    Error = Error::OverlappingSegments;
                    goto done;
                }

                const auto JtSegMemoryRange = JtSegment->getMemoryRange();
                if (ItSegment->getMemoryRange().overlaps(JtSegMemoryRange)) {
                    Error = Error::OverlappingSegments;
                    goto done;
                }
            }

            const auto &SectionList = ItSegment->getSectionList();

            const auto SectionListBegin = SectionList.cbegin();
            const auto SectionListEnd = SectionList.cend();

            for (auto SectIter = SectionListBegin;
                 SectIter != SectionListEnd;
                 SectIter++)
            {
                const auto &ItSection = *SectIter;
                const auto &ItSectFileRange = ItSection->getFileRange();

                if (ItSectFileRange.getBegin() == 0) {
                    continue;
                }

                if (!ItSegment->getFileRange().contains(ItSectFileRange)) {
                    Error = Error::InvalidSection;
                    goto done;
                }

                const auto &ItSectMemoryRange = ItSection->getFileRange();
                for (auto SectJter = SectionListBegin;
                     SectJter != SectIter;
                     SectJter++)
                {
                    const auto &JtSection = *SectJter;
                    if (ItSectFileRange.overlaps(JtSection->getFileRange())) {
                        Error = Error::OverlappingSections;
                        goto done;
                    }

                    const auto &JtSectMemoryRange =
                        JtSection->getMemoryRange();

                    if (ItSectMemoryRange.overlaps(JtSectMemoryRange)) {
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

    std::unique_ptr<SegmentInfo>
    SegmentInfoCollection::OpenSegmentInfoWithName(
        const ConstLoadCommandStorage &LoadCmdStorage,
        bool Is64Bit,
        const std::string_view &Name,
        Error *ErrorOut) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.IsBigEndian();
        for (const auto &LoadCmd : LoadCmdStorage) {
            switch (LoadCmd.getKind(IsBigEndian)) {
                case LoadCommand::Kind::Segment: {
                    if (Is64Bit) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment>(IsBigEndian);

                    if (Name.compare(0, 16, Segment.Name) != 0) {
                        continue;
                    }

                    if (ParseSegmentInfo(Segment,
                                         *Info.get(),
                                         IsBigEndian,
                                         ErrorOut))
                    {
                        return nullptr;
                    }

                    return Info;
                }
                case LoadCommand::Kind::Segment64: {
                    if (!Is64Bit) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment64>(IsBigEndian);

                    if (Name.compare(0, 16, Segment.Name) != 0) {
                        continue;
                    }

                    if (ParseSegmentInfo(Segment,
                                         *Info.get(),
                                         IsBigEndian,
                                         ErrorOut))
                    {
                        return Info;
                    }

                    return nullptr;
                }

                default:
                    break;
            }
        }

        return nullptr;
    }

    std::unique_ptr<SectionInfo>
    SegmentInfoCollection::OpenSectionInfoWithName(
        const ConstLoadCommandStorage &LoadCmdStorage,
        bool Is64Bit,
        const std::string_view &SegmentName,
        const std::string_view &SectionName,
        std::unique_ptr<SegmentInfo> *SegmentOut,
        Error *ErrorOut) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.IsBigEndian();
        for (const auto &LoadCmd : LoadCmdStorage) {
            switch (LoadCmd.getKind(IsBigEndian)) {
                case LoadCommand::Kind::Segment: {
                    if (Is64Bit) {
                        continue;
                    }

                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment>(IsBigEndian);

                    const auto &SegName = Segment.Name;
                    if (SegmentName.compare(0, SegmentName.length(), SegName)) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    if (!ParseSegmentInfo(Segment,
                                          *Info.get(),
                                          IsBigEndian,
                                          ErrorOut))
                    {
                        return nullptr;
                    }

                    *SegmentOut = std::move(Info);
                    const auto SectionList =
                        Segment.GetConstSectionList(IsBigEndian);

                    for (const auto &Sect : SectionList) {
                        if (strncmp(SectionName.data(), Sect.Name, 16) != 0) {
                            continue;
                        }

                        auto SectInfo = std::make_unique<SectionInfo>();
                        const auto ParseSectResult =
                            ParseSectionInfo(Sect,
                                             *SectInfo.get(),
                                             IsBigEndian,
                                             ErrorOut);

                        if (!ParseSectResult) {
                            return nullptr;
                        }

                        return SectInfo;
                    }

                    return nullptr;
                }

                case LoadCommand::Kind::Segment64: {
                    if (!Is64Bit) {
                        continue;
                    }

                    const auto &Segment =
                        LoadCmd.cast<LoadCommand::Kind::Segment64>(IsBigEndian);

                    const auto &SegName = Segment.Name;
                    if (SegmentName.compare(0, SegmentName.length(), SegName)) {
                        continue;
                    }

                    auto Info = std::make_unique<SegmentInfo>();
                    if (!ParseSegmentInfo(Segment,
                                          *Info.get(),
                                          IsBigEndian,
                                          ErrorOut))
                    {
                        return nullptr;
                    }

                    *SegmentOut = std::move(Info);
                    const auto SectionList =
                        Segment.GetConstSectionList(IsBigEndian);

                    for (const auto &Sect : SectionList) {
                        if (strncmp(SectionName.data(), Sect.Name, 16) != 0) {
                            continue;
                        }

                        auto SectInfo = std::make_unique<SectionInfo>();
                        const auto ParseSectResult =
                            ParseSectionInfo(Sect,
                                             *SectInfo.get(),
                                             IsBigEndian,
                                             ErrorOut);

                        if (!ParseSectResult) {
                            return nullptr;
                        }

                        return SectInfo;
                    }

                    return nullptr;
                }

                default:
                    break;
            }
        }

        return nullptr;
    }

    const SegmentInfo *
    SegmentInfoCollection::GetInfoForName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &SegInfo : *this) {
            if (SegInfo->getName() == Name) {
                return SegInfo.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfo::FindSectionWithName(
        const std::string_view &Name) const noexcept
    {
        for (const auto &SectInfo : SectionList) {
            if (SectInfo->getName() == Name) {
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
            for (const auto &SectInfo : SegmentInfo->getSectionList()) {
                if (SectInfo->getName() == Name) {
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
            const auto &SectionList = Segment->getSectionList();
            const auto SectionListSize = SectionList.size();

            if (IndexOutOfBounds(SectionIndex, SectionListSize)) {
                SectionIndex -= SectionListSize;
                continue;
            }

            return SectionList.at(SectionIndex).get();
        }

        return nullptr;
    }

    const SegmentInfo *
    SegmentInfoCollection::FindSegmentContainingAddress(
        uint64_t Address) const noexcept
    {
        for (const auto &Segment : *this) {
            if (Segment->getMemoryRange().containsLocation(Address)) {
                return Segment.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfo::FindSectionContainingAddress(uint64_t Address) const noexcept {
        for (const auto &Section : SectionList) {
            if (Section->getMemoryRange().containsLocation(Address)) {
                return Section.get();
            }
        }

        return nullptr;
    }

    const SectionInfo *
    SegmentInfo::FindSectionContainingRelativeAddress(
        uint64_t Address) const noexcept
    {
        if (!getMemoryRange().containsRelativeLocation(Address)) {
            return nullptr;
        }

        const auto FullAddr = getMemoryRange().getBegin() + Address;
        return FindSectionContainingAddress(FullAddr);
    }

    [[nodiscard]]
    uint8_t *GetDataForSegment(uint8_t *Map, const SegmentInfo &Info) noexcept {
        return (Map + Info.getFileRange().getBegin());
    }

    [[nodiscard]] const uint8_t *
    GetDataForSegment(const uint8_t *Map, const SegmentInfo &Info) noexcept {
        return (Map + Info.getFileRange().getBegin());
    }

    [[nodiscard]]
    uint8_t *GetDataForSection(uint8_t *Map, const SectionInfo &Info) noexcept {
        return (Map + Info.getFileRange().getBegin());
    }

    [[nodiscard]] const uint8_t *
    GetDataForSection(const uint8_t *Map, const SectionInfo &Info) noexcept {
        return (Map + Info.getFileRange().getBegin());
    }

    template <typename T>
    [[nodiscard]] static inline T *
    GetDataForVirtualAddrImpl(const SegmentInfoCollection &Collection,
                              T *Map,
                              uint64_t Addr,
                              uint64_t Size,
                              T **EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = LocationRange::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->getMemoryRange().contains(DataRange)) {
                continue;
            }

            for (const auto &Section : Segment->getSectionList()) {
                const auto &SectMemoryRange = Section->getMemoryRange();
                if (!SectMemoryRange.contains(DataRange)) {
                    continue;
                }

                const auto Data = GetDataForSection(Map, *Section.get());
                const auto Offset = (Addr - SectMemoryRange.getBegin());
                const auto &SectFileRange = Section->getFileRange();

                if (!SectFileRange.containsRelativeLocation(Offset)) {
                    return nullptr;
                }

                if (EndOut != nullptr) {
                    const auto SectionSize = SectFileRange.size();
                    *EndOut = reinterpret_cast<T *>(Data + SectionSize);
                }

                return (Data + Offset);
            }
        }

        return nullptr;
    }

    template <typename T>
    [[nodiscard]] static inline T *
    GetDataForVirtualAddrImplNoSections(const SegmentInfoCollection &Collection,
                                        T *Map,
                                        uint64_t Addr,
                                        uint64_t Size,
                                        T **EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = LocationRange::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->getMemoryRange().contains(DataRange)) {
                continue;
            }

            const auto Data = GetDataForSegment(Map, *Segment.get());
            const auto Offset = (Addr - Segment->getMemoryRange().getBegin());

            if (EndOut != nullptr) {
                const auto SegmentSize = Segment->getFileRange().size();
                *EndOut = reinterpret_cast<T *>(Data + SegmentSize);
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
            GetDataForVirtualAddrImpl<uint8_t>(*this,
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
            GetDataForVirtualAddrImpl<const uint8_t>(*this,
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
            GetDataForVirtualAddrImplNoSections<uint8_t>(*this,
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
            GetDataForVirtualAddrImplNoSections<const uint8_t>(*this,
                                                               Map,
                                                               Addr,
                                                               Size,
                                                               EndOut);
        return Data;
    }
}
