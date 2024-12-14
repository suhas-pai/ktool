//
//  ADT/Mach-O/SegmentUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 5/16/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include <cstring>

#include "ADT/Mach-O/LoadCommandStorage.h"
#include "ADT/Mach-O/SegmentUtil.h"

namespace MachO {
    template <typename SectionType>
    [[nodiscard]] static auto
    ParseSectionInfo(const SectionType &Section,
                     SectionInfo &InfoIn,
                     const bool IsBigEndian,
                     SegmentInfoCollection::Error *const ErrorOut) noexcept
    {
        if (const auto FileRange = Section.getFileRange(IsBigEndian)) {
            InfoIn.setFileRange(FileRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        if (const auto MemoryRange = Section.getMemoryRange(IsBigEndian)) {
            InfoIn.setMemoryRange(MemoryRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSection;
        }

        auto Name = std::string(Section.Name, strnlen(Section.Name, 16));

        InfoIn.setName(std::move(Name));
        InfoIn.setFlags(Section.getFlags(IsBigEndian));
        InfoIn.setReserved1(Section.getReserved1(IsBigEndian));
        InfoIn.setReserved2(Section.getReserved2(IsBigEndian));

        return true;
    }

    template <typename SegmentType>
    [[nodiscard]] static auto
    ParseSegmentInfo(const SegmentType &Segment,
                     SegmentInfo &InfoIn,
                     const bool IsBigEndian,
                     SegmentInfoCollection::Error *const ErrorOut) noexcept
    {
        if (const auto FileRange = Segment.getFileRange(IsBigEndian)) {
            InfoIn.setFileRange(FileRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSegment;
        }

        if (const auto MemoryRange = Segment.getVmRange(IsBigEndian)) {
            InfoIn.setMemoryRange(MemoryRange.value());
        } else {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSegment;
        }

        auto Name = std::string(Segment.Name, strnlen(Segment.Name, 16));

        InfoIn.setName(std::move(Name));
        InfoIn.setInitProt(Segment.getInitProt(IsBigEndian));
        InfoIn.setMaxProt(Segment.getMaxProt(IsBigEndian));
        InfoIn.setFlags(Segment.getFlags(IsBigEndian));

        if (!Segment.isSectionListValid(IsBigEndian)) {
            *ErrorOut = SegmentInfoCollection::Error::InvalidSectionList;
            return true;
        }

        auto &SectionOutList = InfoIn.getSectionListRef();
        const auto SectionList =
            Segment.getConstSectionListUnsafe(IsBigEndian);

        for (const auto &Section : SectionList) {
            auto SectInfo = std::make_unique<SectionInfo>();
            const auto ParseSectResult =
                ParseSectionInfo(Section,
                                 *SectInfo.get(),
                                 IsBigEndian,
                                 ErrorOut);

            if (ParseSectResult) {
                SectInfo->setSegment(&InfoIn);
                SectionOutList.emplace_back(std::move(SectInfo));
            }
        }

        return true;
    }

    void
    SegmentInfoCollection::ParseFromLoadCommands(
        const ConstLoadCommandStorage &LoadCmdStorage,
        const bool Is64Bit,
        Error *const ErrorOut) noexcept
    {
        const auto IsBigEndian = LoadCmdStorage.isBigEndian();
        auto Error = Error::None;

        if (Is64Bit) {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment64>(LC, IsBigEndian);

                if (Segment == nullptr) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (ParseSegmentInfo(*Segment,
                                     *Info.get(),
                                     IsBigEndian,
                                     &Error))
                {
                    List.emplace_back(std::move(Info));
                }
            }
        } else {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment>(LC, IsBigEndian);

                if (Segment == nullptr) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (ParseSegmentInfo(*Segment,
                                     *Info.get(),
                                     IsBigEndian,
                                     &Error))
                {
                    List.emplace_back(std::move(Info));
                }
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

                    const auto &JtSectMemoryRange = JtSection->getMemoryRange();
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

    auto
    SegmentInfoCollection::Open(const ConstLoadCommandStorage &LoadCmdStorage,
                                const bool Is64Bit,
                                Error *const ErrorOut) noexcept
        -> SegmentInfoCollection
    {
        auto Result = SegmentInfoCollection();
        Result.ParseFromLoadCommands(LoadCmdStorage, Is64Bit, ErrorOut);

        return Result;
    }

    auto
    SegmentInfoCollection::OpenSegmentInfoWithName(
        const ConstLoadCommandStorage &LoadCmdStorage,
        const bool Is64Bit,
        const std::string_view Name,
        Error *const ErrorOut) noexcept
            -> std::unique_ptr<SegmentInfo>
    {
        const auto IsBigEndian = LoadCmdStorage.isBigEndian();
        if (Is64Bit) {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment64>(LC, IsBigEndian);

                if (Segment == nullptr) {
                    continue;
                }

                if (!Segment->nameEquals(Name)) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (ParseSegmentInfo(*Segment,
                                     *Info.get(),
                                     IsBigEndian,
                                     ErrorOut))
                {
                    return Info;
                }
            }
        } else {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment>(LC, IsBigEndian);

                if (!Segment->nameEquals(Name)) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (ParseSegmentInfo(*Segment,
                                     *Info.get(),
                                     IsBigEndian,
                                     ErrorOut))
                {
                    return Info;
                }
            }
        }

        return nullptr;
    }

    auto
    SegmentInfoCollection::OpenSectionInfoWithName(
        const ConstLoadCommandStorage &LoadCmdStorage,
        const bool Is64Bit,
        const std::string_view SegmentName,
        const std::string_view SectionName,
        std::unique_ptr<SegmentInfo> *const SegmentOut,
        Error *const ErrorOut) noexcept
            -> std::unique_ptr<SectionInfo>
    {
        const auto IsBigEndian = LoadCmdStorage.isBigEndian();
        if (Is64Bit) {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment64>(LC, IsBigEndian);

                if (Segment == nullptr) {
                    continue;
                }

                if (!Segment->nameEquals(SegmentName)) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (!ParseSegmentInfo(*Segment,
                                      *Info.get(),
                                      IsBigEndian,
                                      ErrorOut))
                {
                    return nullptr;
                }

                if (!Segment->isSectionListValid(IsBigEndian)) {
                    *SegmentOut = std::move(Info);
                    return nullptr;
                }

                *SegmentOut = std::move(Info);
                const auto SectionList =
                    Segment->getConstSectionListUnsafe(IsBigEndian);

                for (const auto &Sect : SectionList) {
                    if (!Sect.nameEquals(SectionName)) {
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
        } else {
            for (const auto &LC : LoadCmdStorage) {
                const auto *Segment =
                    dyn_cast<LoadCommand::Kind::Segment>(LC, IsBigEndian);

                if (Segment == nullptr) {
                    continue;
                }

                if (!Segment->nameEquals(SegmentName)) {
                    continue;
                }

                auto Info = std::make_unique<SegmentInfo>();
                if (!ParseSegmentInfo(*Segment,
                                      *Info.get(),
                                      IsBigEndian,
                                      ErrorOut))
                {
                    return nullptr;
                }

                if (!Segment->isSectionListValid(IsBigEndian)) {
                    *SegmentOut = std::move(Info);
                    return nullptr;
                }

                *SegmentOut = std::move(Info);
                const auto SectionList =
                    Segment->getConstSectionListUnsafe(IsBigEndian);

                for (const auto &Sect : SectionList) {
                    if (!Sect.nameEquals(SectionName)) {
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
        }

        return nullptr;
    }

    auto SegmentInfoCollection::GetInfoForName(
        const std::string_view Name) const noexcept
            -> const SegmentInfo *
    {
        for (const auto &SegInfo : *this) {
            if (SegInfo->getName() == Name) {
                return SegInfo.get();
            }
        }

        return nullptr;
    }

    auto
    SegmentInfo::FindSectionWithName(const std::string_view Name) const noexcept
        -> const SectionInfo *
    {
        for (const auto &SectInfo : getSectionList()) {
            if (SectInfo->getName() == Name) {
                return SectInfo.get();
            }
        }

        return nullptr;
    }

    auto SegmentInfoCollection::FindSectionWithName(
        const std::string_view SegmentName,
        const std::string_view Name) const noexcept
            ->  const SectionInfo *
    {
        const auto *SegmentInfo = GetInfoForName(SegmentName);
        if (SegmentInfo != nullptr) {
            return SegmentInfo->FindSectionWithName(Name);
        }

        return nullptr;
    }

    auto
    SegmentInfoCollection::FindSectionWithName(
        const std::initializer_list<SectionNamePair> &List) const noexcept
            -> const SectionInfo *
    {
        const auto NamePairBegin = List.begin();
        const auto NamePairEnd = List.end();

        for (auto It = NamePairBegin; It != NamePairEnd; It++) {
            auto Segment = GetInfoForName(It->SegmentName);
            if (Segment == nullptr) {
                continue;
            }

            for (const auto &SectionInfo : It->SectionList) {
                const auto SectInfo = Segment->FindSectionWithName(SectionInfo);
                if (SectInfo != nullptr) {
                    return SectInfo;
                }
            }
        }

        return nullptr;
    }

    auto
    SegmentInfoCollection::GetSectionWithIndex(
        uint64_t SectionIndex) const noexcept
            -> const SectionInfo *
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

    auto
    SegmentInfoCollection::FindSegmentContainingAddress(
        const uint64_t Address) const noexcept
            -> const SegmentInfo *
    {
        for (const auto &Segment : *this) {
            if (Segment->getMemoryRange().hasLocation(Address)) {
                return Segment.get();
            }
        }

        return nullptr;
    }

    auto SegmentInfo::FindSectionContainingAddress(
        const uint64_t Address) const noexcept
            -> const SectionInfo *
    {
        for (const auto &Section : getSectionList()) {
            if (Section->getMemoryRange().hasLocation(Address)) {
                return Section.get();
            }
        }

        return nullptr;
    }

    auto
    SegmentInfo::FindSectionContainingRelativeAddress(
        const uint64_t Address) const noexcept
            -> const SectionInfo *
    {
        if (!getMemoryRange().hasIndex(Address)) {
            return nullptr;
        }

        const auto FullAddr = this->getMemoryRange().getBegin() + Address;
        return FindSectionContainingAddress(FullAddr);
    }

    template <typename T>
    [[nodiscard]] static inline T *
    GetDataForVirtualAddrImpl(const SegmentInfoCollection &Collection,
                              T *const Map,
                              const uint64_t Addr,
                              const uint64_t Size,
                              T **const EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = Range::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->getMemoryRange().contains(DataRange)) {
                continue;
            }

            for (const auto &Section : Segment->getSectionList()) {
                const auto &SectMemoryRange = Section->getMemoryRange();
                if (!SectMemoryRange.contains(DataRange)) {
                    continue;
                }

                const auto Data = Section->getData(Map);
                const auto Offset = (Addr - SectMemoryRange.getBegin());
                const auto &SectFileRange = Section->getFileRange();

                if (!SectFileRange.hasIndex(Offset)) {
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
                                        T *const Map,
                                        const uint64_t Addr,
                                        const uint64_t Size,
                                        T **const EndOut) noexcept
    {
        if (Addr == 0) {
            return nullptr;
        }

        const auto DataRange = Range::CreateWithEnd(Addr, Addr + Size);
        for (const auto &Segment : Collection) {
            if (!Segment->getMemoryRange().contains(DataRange)) {
                continue;
            }

            const auto Data = Segment->getData(Map);
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
        uint8_t *const Map,
        const uint64_t Addr,
        const uint64_t Size,
        uint8_t **const EndOut) const noexcept
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

    auto
    SegmentInfoCollection::GetDataForVirtualAddrIgnoreSections(
        uint8_t *const Map,
        const uint64_t Addr,
        const uint64_t Size,
        uint8_t **const EndOut) const noexcept
            -> uint8_t *
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
        const uint8_t *const Map,
        const uint64_t Addr,
        const uint64_t Size,
        const uint8_t **const EndOut) const noexcept
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
