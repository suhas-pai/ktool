//
//  ADT/Mach-O/ObjcUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/13/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <cassert>

#include "DeVirtualizer.h"
#include "ObjcParse.h"

namespace MachO {
    struct ObjcClassCategoryCollection;
    struct ObjcClassInfoCollection : public Tree {
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
            const Range &Range,
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
             const BindActionList *const BindList,
             const LazyBindActionList *const LazyBindList,
             const WeakBindActionList *const WeakBindList,
             const bool IsBigEndian,
             const bool Is64Bit,
             Error *const ErrorOut,
             BindOpcodeParseError *const ParseErrorOut,
             BindActionCollection::Error *const CollectionErrorOut) noexcept
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

        [[nodiscard]] inline auto getRoot() const noexcept {
            return reinterpret_cast<Info *>(Root);
        }

        [[nodiscard]] inline auto &getMap() noexcept {
            return List;
        }

        [[nodiscard]] inline auto &getMap() const noexcept {
            return List;
        }

        [[nodiscard]] auto GetAsList() const noexcept
            -> std::vector<ObjcClassInfo *>;

        auto AddNullClass(uint64_t BindAddress) noexcept -> ObjcClassInfo *;

        auto
        AddExternalClass(std::string_view Name,
                         uint64_t DylibOrdinal,
                         uint64_t BindAddress) noexcept
            -> ObjcClassInfo *;

        [[nodiscard]]
        auto GetInfoForAddress(uint64_t Address) const noexcept
            -> ObjcClassInfo *;

        [[nodiscard]] auto
        GetInfoForClassName(std::string_view Name) const noexcept
            -> ObjcClassInfo *;

        using Iterator = TreeIterator<ObjcClassInfo>;
        using ConstIterator = TreeIterator<const ObjcClassInfo>;

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(getRoot());
        }

        [[nodiscard]] inline auto end() const noexcept {
            return Iterator::Null();
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return ConstIterator(getRoot());
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ConstIterator::Null();
        }

        [[nodiscard]] inline auto size() const noexcept {
            return List.size();
        }

        template <typename T>
        inline auto forEach(const T &Callback) const noexcept
            -> decltype(*this)
        {
            this->forEachNode(Callback);
            return *this;
        }

        template <typename T>
        inline auto forEach(const T &Callback) noexcept
            -> decltype(*this)
        {
            this->forEachNode(Callback);
            return *this;
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

        auto
        CollectFrom(const uint8_t *Map,
                    const SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    const BindActionCollection *BindCollection,
                    ObjcClassInfoCollection *ClassInfoTree,
                    bool IsBigEndian,
                    bool Is64Bit,
                    Error *ErrorOut) noexcept
            -> decltype(*this);

        auto
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
                    BindActionCollection::Error *CollectionErrorOut) noexcept
            -> decltype(*this);

        [[nodiscard]] static inline auto
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

        [[nodiscard]] static inline auto
        Open(const ConstMemoryMap &Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             ObjcClassInfoCollection *const ClassInfoTree,
             const BindActionList *const BindList,
             const LazyBindActionList *const LazyBindList,
             const WeakBindActionList *const WeakBindList,
             const bool IsBigEndian,
             const bool Is64Bit,
             Error *const ErrorOut,
             BindOpcodeParseError *const ParseErrorOut,
             BindActionCollection::Error *const CollectionErrorOut) noexcept
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

        template <typename T>
        inline auto forEach(const T &Callback) const noexcept -> decltype(*this)
        {
            for (const auto &CategoryPtr : List) {
                Callback(*CategoryPtr);
            }

            return *this;
        }

        template <typename T>
        inline auto forEach(const T &Callback) noexcept -> decltype(*this) {
            for (const auto &CategoryPtr : List) {
                Callback(*CategoryPtr);
            }

            return *this;
        }
    };
}
