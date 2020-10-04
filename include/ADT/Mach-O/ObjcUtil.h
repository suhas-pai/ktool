//
//  include/ADT/Mach-O/ObjcUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/13/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>
#include <unordered_map>
#include <vector>

#include "ADT/Tree.h"

#include "BindUtil.h"
#include "DeVirtualizer.h"
#include "LoadCommands.h"
#include "Objc.h"
#include "ObjcParse.h"
#include "SegmentUtil.h"

namespace MachO {
    struct ObjcClassCategoryCollection;
    struct ObjcClassInfoCollection : public BasicTree {
        friend struct ObjcClassCategoryCollection;
    public:
        using Info = ObjcClassInfo;
    protected:
        std::unordered_map<uint64_t, std::unique_ptr<Info>> List;
        void AdjustExternalAndRootClassList(std::vector<Info *> &List) noexcept;

        [[nodiscard]] static int
        GetBindCollection(
            const SegmentInfoCollection &SegmentCollection,
            const BindActionList *BindList,
            const LazyBindActionList *LazyBindList,
            const WeakBindActionList *WeakBindList,
            const LocationRange &Range,
            BindActionCollection &CollectionOut,
            BindOpcodeParseError *ParseErrorOut,
            BindActionCollection::Error *CollectionErrorOut) noexcept;
    public:
        using Error = ObjcParse::Error;
        ObjcClassInfoCollection() noexcept = default;

        ObjcClassInfoCollection &
        Parse(const uint8_t *Map,
              const SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const BindActionCollection &BindCollection,
              bool IsBigEndian,
              bool Is64Bit,
              Error *ErrorOut) noexcept;

        ObjcClassInfoCollection &
        Parse(const ConstMemoryMap &Map,
              const SegmentInfoCollection &SegmentCollection,
              const ConstDeVirtualizer &DeVirtualizer,
              const BindActionList *BindList,
              const LazyBindActionList *LazyBindList,
              const WeakBindActionList *WeakBindList,
              bool IsBigEndian,
              bool Is64Bit,
              Error *ErrorOut,
              BindOpcodeParseError *ParseErrorOut,
              BindActionCollection::Error *CollectionErrorOut) noexcept;

        [[nodiscard]] static inline ObjcClassInfoCollection
        Open(const uint8_t *Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const BindActionCollection &BindCollection,
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
        Open(const ConstMemoryMap &Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const BindActionList *BindList,
             const LazyBindActionList *LazyBindList,
             const WeakBindActionList *WeakBindList,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut,
             BindOpcodeParseError *ParseErrorOut,
             BindActionCollection::Error *CollectionErrorOut) noexcept
        {
            auto Result = ObjcClassInfoCollection();
            Result.Parse(Map,
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

        [[nodiscard]] inline Info *getRoot() const noexcept {
            return reinterpret_cast<Info *>(Root);
        }

        [[nodiscard]] inline
        std::unordered_map<uint64_t, std::unique_ptr<Info>> &getMap() noexcept {
            return List;
        }

        [[nodiscard]]
        inline const std::unordered_map<uint64_t, std::unique_ptr<Info>> &
        getMap() const noexcept {
            return List;
        }

        [[nodiscard]] std::vector<ObjcClassInfo *> GetAsList() const noexcept;
        ObjcClassInfo *AddNullClass(uint64_t BindAddress) noexcept;

        ObjcClassInfo *
        AddExternalClass(std::string_view Name,
                         uint64_t DylibOrdinal,
                         uint64_t BindAddress) noexcept;

        [[nodiscard]]
        ObjcClassInfo *GetInfoForAddress(uint64_t Address) const noexcept;

        [[nodiscard]] ObjcClassInfo *
        GetInfoForClassName(std::string_view Name) const noexcept;

        using Iterator = TreeIterator<ObjcClassInfo>;
        using ConstIterator = TreeIterator<const ObjcClassInfo>;

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(getRoot());
        }

        [[nodiscard]] inline Iterator end() const noexcept {
            return Iterator(getRoot());
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(getRoot());
        }
        
        [[nodiscard]] inline ConstIterator cend() const noexcept {
            return ConstIterator(nullptr);
        }

        [[nodiscard]] inline uint64_t size() const noexcept {
            return List.size();
        }
    };

    struct ObjcClassCategoryCollection {
    public:
        using Error = ObjcParse::Error;
        using Info = ObjcClassCategoryInfo;
    protected:
        std::vector<std::unique_ptr<Info>> List;
    public:
        ObjcClassCategoryCollection() noexcept = default;

        ObjcClassCategoryCollection &
        CollectFrom(const uint8_t *Map,
                    const SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    const BindActionCollection *BindCollection,
                    ObjcClassInfoCollection *ClassInfoTree,
                    bool IsBigEndian,
                    bool Is64Bit,
                    Error *ErrorOut) noexcept;

        ObjcClassCategoryCollection &
        CollectFrom(const ConstMemoryMap &Map,
                    const SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    ObjcClassInfoCollection *ClassInfoTree,
                    const BindActionList *BindList,
                    const LazyBindActionList *LazyBindList,
                    const WeakBindActionList *WeakBindList,
                    bool IsBigEndian,
                    bool Is64Bit,
                    Error *ErrorOut,
                    BindOpcodeParseError *ParseErrorOut,
                    BindActionCollection::Error *CollectionErrorOut) noexcept;

        [[nodiscard]] static inline ObjcClassCategoryCollection
        Open(const uint8_t *Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const BindActionCollection *BindCollection,
             ObjcClassInfoCollection *ClassInfoTree,
             bool Is64Bit,
             bool IsBigEndian,
             Error *ErrorOut) noexcept
        {
            auto Result = ObjcClassCategoryCollection();
            Result.CollectFrom(Map,
                               SegmentCollection,
                               DeVirtualizer,
                               BindCollection,
                               ClassInfoTree,
                               Is64Bit,
                               IsBigEndian,
                               ErrorOut);

            return Result;
        }

        [[nodiscard]] static inline ObjcClassCategoryCollection
        Open(const ConstMemoryMap &Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             ObjcClassInfoCollection *ClassInfoTree,
             const BindActionList *BindList,
             const LazyBindActionList *LazyBindList,
             const WeakBindActionList *WeakBindList,
             bool IsBigEndian,
             bool Is64Bit,
             Error *ErrorOut,
             BindOpcodeParseError *ParseErrorOut,
             BindActionCollection::Error *CollectionErrorOut) noexcept
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
