//
//  include/ADT/DscImage/ObjcUtil.h
//  ktool
//
//  Created by Suhas Pai on 7/19/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "DeVirtualizer.h"
#include "ADT/Mach-O/ObjcUtil.h"

namespace DscImage {
    struct ObjcClassInfoCollection : public MachO::ObjcClassInfoCollection {
    public:
        explicit ObjcClassInfoCollection() noexcept = default;

        ObjcClassInfoCollection &
        Parse(const uint8_t *Map,
              const MachO::SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const MachO::BindActionCollection &BindCollection,
              bool Is64Bit,
              bool IsBigEndian,
              Error *ErrorOut) noexcept;

        static inline ObjcClassInfoCollection
        Open(const uint8_t *Map,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const MachO::BindActionCollection &BindCollection,
             bool Is64Bit,
             bool IsBigEndian,
             Error *ErrorOut) noexcept
        {
            auto Result = ObjcClassInfoCollection();
            Result.Parse(Map,
                         SegmentCollection,
                         DeVirtualizer,
                         BindCollection,
                         Is64Bit,
                         IsBigEndian,
                         ErrorOut);

            return Result;
        }
    };

    struct ObjcClassCategoryCollection :
        public MachO::ObjcClassCategoryCollection
    {
        ObjcClassCategoryCollection() noexcept = default;

        ObjcClassCategoryCollection &
        Parse(const uint8_t *Map,
              const MachO::SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const MachO::BindActionCollection &BindCollection,
              ObjcClassInfoCollection *ClassInfoTree,
              bool Is64Bit,
              bool IsBigEndian,
              Error *ErrorOut) noexcept;

        static inline ObjcClassCategoryCollection
        Open(const uint8_t *Map,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const MachO::BindActionCollection &BindCollection,
             ObjcClassInfoCollection *ClassInfoTree,
             bool Is64Bit,
             bool IsBigEndian,
             Error *ErrorOut) noexcept
        {
            auto Result = ObjcClassCategoryCollection();
            Result.Parse(Map,
                         SegmentCollection,
                         DeVirtualizer,
                         BindCollection,
                         ClassInfoTree,
                         Is64Bit,
                         IsBigEndian,
                         ErrorOut);

            return Result;
        }
    };
}
