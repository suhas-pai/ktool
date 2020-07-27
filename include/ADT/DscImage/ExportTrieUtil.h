//
//  include/ADT/DscImage/ExportTrieUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/17/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ADT/Mach-O/ExportTrieUtil.h"
#include "SegmentUtil.h"

namespace DscImage {
    struct ExportTrieEntryCollection : public MachO::ExportTrieEntryCollection {
    protected:
        const SegmentInfo *
        LookupInfoForAddress(
            const MachO::SegmentInfoCollection *Collection,
            uint64_t Address,
            const SectionInfo **SectionOut) const noexcept override;

        using MachO::ExportTrieEntryCollection::ExportTrieEntryCollection;
    public:
        [[nodiscard]] static ExportTrieEntryCollection
        Open(const MachO::ConstExportTrieList &Info,
             const DscImage::SegmentInfoCollection *Collection,
             Error *ErrorOut) noexcept;
    };
}
