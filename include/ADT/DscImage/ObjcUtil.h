//
//  include/ADT/DscImage/ObjcUtil.h
//  ktool
//
//  Created by Suhas Pai on 9/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include "ADT/Mach-O/ObjcUtil.h"
#include "DeVirtualizer.h"

namespace DscImage {
    struct ObjcClassCategoryCollection;
    struct ObjcClassInfoCollection : public MachO::ObjcClassInfoCollection {
        friend struct ObjcClassCategoryCollection;
    public:
        ObjcClassInfoCollection &
        Parse(const uint8_t *Map,
              const MachO::SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const MachO::BindActionCollection &BindCollection,
              bool IsBigEndian,
              bool Is64Bit,
              Error *ErrorOut) noexcept;

        ObjcClassInfoCollection &
        Parse(const ConstMemoryMap &DscMap,
              const ConstMemoryMap &ImageMap,
              const MachO::SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const MachO::BindActionList *BindList,
              const MachO::LazyBindActionList *LazyBindList,
              const MachO::WeakBindActionList *WeakBindList,
              bool IsBigEndian,
              bool Is64Bit,
              Error *ErrorOut,
              MachO::BindOpcodeParseError *ParseErrorOut,
              MachO::BindActionCollection::Error *CollectionErrorOut) noexcept;

        [[nodiscard]] static inline ObjcClassInfoCollection
        Open(const uint8_t *Map,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const MachO::BindActionCollection &BindCollection,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut) noexcept
        {
            auto Result = ObjcClassInfoCollection();
            Result.Parse(Map,
                         SegmentCollection,
                         DeVirtualizer,
                         BindCollection,
                         IsBigEndian,
                         Is64Bit,
                         ErrorOut);

            return Result;
        }

        [[nodiscard]] static inline ObjcClassInfoCollection
        Open(const ConstMemoryMap &DscMap,
             const ConstMemoryMap &ImageMap,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const MachO::BindActionList *BindList,
             const MachO::LazyBindActionList *LazyBindList,
             const MachO::WeakBindActionList *WeakBindList,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut,
             MachO::BindOpcodeParseError *ParseErrorOut,
             MachO::BindActionCollection::Error *CollectionErrorOut) noexcept
        {
            auto Result = ObjcClassInfoCollection();
            Result.Parse(DscMap,
                         ImageMap,
                         SegmentCollection,
                         DeVirtualizer,
                         BindList,
                         LazyBindList,
                         WeakBindList,
                         IsBigEndian,
                         Is64Bit,
                         ErrorOut,
                         ParseErrorOut,
                         CollectionErrorOut);

            return Result;
        }
    };

    struct ObjcClassCategoryCollection :
        public MachO::ObjcClassCategoryCollection
    {
        ObjcClassCategoryCollection &
        CollectFrom(const uint8_t *Map,
                    const MachO::SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    const MachO::BindActionCollection *BindCollection,
                    ObjcClassInfoCollection *ClassInfoTree,
                    bool IsBigEndian,
                    bool Is64Bit,
                    Error *ErrorOut) noexcept;

        ObjcClassCategoryCollection &
        CollectFrom(
            const ConstMemoryMap &Map,
            const MachO::SegmentInfoCollection &SegmentCollection,
            const ConstDeVirtualizer &DeVirtualizer,
            ObjcClassInfoCollection *ClassInfoTree,
            const MachO::BindActionList *BindList,
            const MachO::LazyBindActionList *LazyBindList,
            const MachO::WeakBindActionList *WeakBindList,
            bool IsBigEndian,
            bool Is64Bit,
            Error *ErrorOut,
            MachO::BindOpcodeParseError *ParseErrorOut,
            MachO::BindActionCollection::Error *CollectionErrorOut) noexcept;

        [[nodiscard]] static inline ObjcClassCategoryCollection
        Open(const uint8_t *Map,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const MachO::BindActionCollection *BindCollection,
             ObjcClassInfoCollection *ClassInfoTree,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut) noexcept
        {
            auto Result = ObjcClassCategoryCollection();
            Result.CollectFrom(Map,
                               SegmentCollection,
                               DeVirtualizer,
                               BindCollection,
                               ClassInfoTree,
                               IsBigEndian,
                               Is64Bit,
                               ErrorOut);

            return Result;
        }

        [[nodiscard]] static inline ObjcClassCategoryCollection
        Open(const ConstMemoryMap &Map,
             const MachO::SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             ObjcClassInfoCollection *ClassInfoTree,
             const MachO::BindActionList *BindList,
             const MachO::LazyBindActionList *LazyBindList,
             const MachO::WeakBindActionList *WeakBindList,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut,
             MachO::BindOpcodeParseError *ParseErrorOut,
             MachO::BindActionCollection::Error *CollectionErrorOut) noexcept
        {
            auto Result = ObjcClassCategoryCollection();
            Result.CollectFrom(Map,
                               SegmentCollection,
                               DeVirtualizer,
                               ClassInfoTree,
                               BindList,
                               LazyBindList,
                               WeakBindList,
                               IsBigEndian,
                               Is64Bit,
                               ErrorOut,
                               ParseErrorOut,
                               CollectionErrorOut);

            return Result;
        }
    };
}
