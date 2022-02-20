//
//  src/ADT/DscImage/ExportTrieUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ExportTrieUtil.h"

namespace DscImage {
    const SegmentInfo *
    ExportTrieEntryCollection::LookupInfoForAddress(
        const MachO::SegmentInfoCollection *const Collection,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
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

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(
        const MachO::ConstExportTrieList &List,
        const SegmentInfoCollection *const Collection,
        Error *ErrorOut) noexcept
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(List, Collection, ErrorOut);

        return Result;
    }
}
