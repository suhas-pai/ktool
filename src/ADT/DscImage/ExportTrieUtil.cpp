//
//  ADT/DscImage/ExportTrieUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/DscImage/ExportTrieUtil.h"

namespace DscImage {
    auto
    ExportTrieEntryCollection::LookupInfoForAddress(
        const MachO::SegmentInfoCollection *const Collection,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
            -> const SegmentInfo *
    {
        const auto SegCollection =
            reinterpret_cast<const SegmentInfoCollection *>(Collection);

        auto Segment = static_cast<const SegmentInfo *>(nullptr);
        const auto Section =
            SegCollection->FindSectionWithImageRelativeAddress(Address,
                                                               &Segment);

        if (Section != nullptr) {
            *SectionOut = Section;
            return Section->getSegment();
        }

        return nullptr;
    }

    auto
    ExportTrieEntryCollection::Open(
        const MachO::ConstExportTrieList &List,
        const SegmentInfoCollection *const Collection,
        Error *const ErrorOut) noexcept
            -> ExportTrieEntryCollection
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(List, Collection, ErrorOut);

        return Result;
    }
}
