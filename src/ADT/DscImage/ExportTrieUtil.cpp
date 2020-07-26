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
        const MachO::SegmentInfoCollection *Collection,
        uint64_t Address,
        const SectionInfo **SectionOut) const noexcept
    {
        const auto SegCollection =
            reinterpret_cast<const SegmentInfoCollection *>(Collection);

        const auto Section =
            SegCollection->FindSectionWithImageRelativeAddress(Address);

        if (Section != nullptr) {
            return Section->Segment;
        }

        return nullptr;
    }

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(
        uint64_t BaseAddress,
        const MachO::ConstExportTrieList &List,
        const DscImage::SegmentInfoCollection *Collection,
        Error *ErrorOut) noexcept
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(List, Collection, ErrorOut);

        return Result;
    }
}
