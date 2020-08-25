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
#include "SegmentUtil.h"

namespace MachO {
    struct ObjcClassInfoCollection;
    struct ObjcClassCategoryInfo;
    struct ObjcClassInfo : public BasicTreeNode {
        friend ObjcClassInfoCollection;
    public:
        using CategoryListType = std::vector<ObjcClassCategoryInfo *>;
    protected:
        union {
            uint64_t Addr = 0;
            uint64_t BindAddr;
        };

        std::string Name;
        uint64_t DylibOrdinal = 0;

        bool sIsExternal : 1;
        bool sIsNull : 1;
        bool sIsSwift : 1;

        ObjcClassRoFlags Flags;
        CategoryListType CategoryList;
    public:
        ObjcClassInfo() noexcept : sIsExternal(false), sIsNull(false) {}

        explicit ObjcClassInfo(const std::string_view &Name) noexcept
        : Name(Name), sIsExternal(false), sIsNull(false) {}

        [[nodiscard]]
        inline BasicTreeNode *createNew() const noexcept override {
            return new ObjcClassInfo;
        }

        [[nodiscard]] inline ObjcClassInfo *getParent() const noexcept {
            return get(Parent);
        }

        [[nodiscard]] inline ObjcClassInfo *getPrevSibling() const noexcept {
            return get(PrevSibling);
        }

        [[nodiscard]] inline ObjcClassInfo *getNextSibling() const noexcept {
            return get(NextSibling);
        }

        [[nodiscard]] inline ObjcClassInfo *getFirstChild() const noexcept {
            return get(FirstChild);
        }

        [[nodiscard]] inline ObjcClassInfo *getLongestChild() const noexcept {
            return get(LongestChild);
        }

        [[nodiscard]]
        static inline ObjcClassInfo &get(BasicTreeNode &Node) noexcept {
            return reinterpret_cast<ObjcClassInfo &>(Node);
        }

        [[nodiscard]] static
        inline const ObjcClassInfo &get(const BasicTreeNode &Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo &>(Node);
        }

        [[nodiscard]]
        static inline ObjcClassInfo *get(BasicTreeNode *Node) noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Node);
        }

        [[nodiscard]] static
        inline const ObjcClassInfo *get(const BasicTreeNode *Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo *>(Node);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return Name.length();
        }

        [[nodiscard]] inline ObjcClassInfo *getSuper() const noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Parent);
        }

        inline ObjcClassInfo &setSuper(ObjcClassInfo *Parent) noexcept {
            this->Parent = reinterpret_cast<BasicTreeNode *>(Parent);
            return *this;
        }

        inline ObjcClassInfo &
        setLongestChild(const ObjcClassInfo *LongestChild) noexcept {
            this->LongestChild = const_cast<ObjcClassInfo *>(LongestChild);
            return *this;
        }

        [[nodiscard]]
        constexpr inline std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr inline uint64_t getDylibOrdinal() const noexcept {
            return DylibOrdinal;
        }

        [[nodiscard]] constexpr inline bool IsExternal() const noexcept {
            return sIsExternal;
        }

        [[nodiscard]] constexpr inline bool IsNull() const noexcept {
            return sIsNull;
        }

        [[nodiscard]] constexpr inline bool IsSwift() const noexcept {
            return sIsSwift;
        }

        [[nodiscard]]
        constexpr inline const ObjcClassRoFlags &getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr inline uint64_t getAddr() const noexcept {
            assert(!this->IsExternal());
            return Addr;
        }

        [[nodiscard]] constexpr inline uint64_t getBindAddr() const noexcept {
            assert(this->IsExternal());
            return BindAddr;
        }

        [[nodiscard]] constexpr
        inline const CategoryListType &getCategoryList() const noexcept {
            return CategoryList;
        }

        [[nodiscard]]
        constexpr inline CategoryListType &getCategoryList() noexcept {
            return CategoryList;
        }

        inline ObjcClassInfo &setName(const std::string &Value) noexcept {
            this->Name = Value;
            return *this;
        }

        inline ObjcClassInfo &setDylibOrdinal(uint64_t Value) noexcept {
            this->DylibOrdinal = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsExternal(bool Value = true) noexcept {
            this->sIsExternal = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsNull(bool Value = true) noexcept {
            this->sIsNull = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsSwift(bool Value = true) noexcept {
            this->sIsSwift = Value;
            return *this;
        }

        constexpr
        inline ObjcClassInfo &setFlags(const ObjcClassRoFlags &Value) noexcept {
            this->Flags = Value;
            return *this;
        }

        inline ObjcClassInfo &setAddr(uint64_t Value) noexcept {
            assert(!this->IsExternal());

            this->Addr = Value;
            return *this;
        }

        inline ObjcClassInfo &setBindAddr(uint64_t Value) noexcept {
            assert(this->IsExternal());

            this->BindAddr = Value;
            return *this;
        }

        [[nodiscard]]
        constexpr inline bool operator==(const ObjcClassInfo &Rhs) noexcept {
            return (Addr == Rhs.Addr);
        }

        [[nodiscard]]
        constexpr inline bool operator!=(const ObjcClassInfo &Rhs) noexcept {
            return !(*this == Rhs);
        }
    };

    enum class ObjcParseError {
        None,
        NoObjcData,
        UnalignedSection,
        InvalidAddress
    };

    struct ObjcClassInfoCollection : public BasicTree {
    public:
        using Info = ObjcClassInfo;
    protected:
        std::unordered_map<uint64_t, std::unique_ptr<Info>> List;
    public:
        using Error = ObjcParseError;
        explicit ObjcClassInfoCollection() noexcept = default;

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
        AddExternalClass(const std::string_view &Name,
                         uint64_t DylibOrdinal,
                         uint64_t BindAddress) noexcept;

        [[nodiscard]]
        ObjcClassInfo *GetInfoForAddress(uint64_t Address) const noexcept;

        [[nodiscard]] ObjcClassInfo *
        GetInfoForClassName(const std::string_view &Name) const noexcept;

        using Iterator = TreeIterator<ObjcClassInfo>;
        using ConstIterator = TreeIterator<const ObjcClassInfo>;

        [[nodiscard]] Iterator begin() const noexcept;
        [[nodiscard]] Iterator end() const noexcept;

        [[nodiscard]] ConstIterator cbegin() const noexcept;
        [[nodiscard]] ConstIterator cend() const noexcept;

        [[nodiscard]] inline uint64_t size() const noexcept {
            return List.size();
        }
    };

    struct ObjcClassCategoryInfo {
    protected:
        std::string Name;
        const ObjcClassInfo *Class = nullptr;
        uint64_t Address = 0;
        bool sIsNull : 1;
    public:
        ObjcClassCategoryInfo() : sIsNull(false) {}

        [[nodiscard]]
        constexpr inline std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr inline const ObjcClassInfo *getClass() const noexcept {
            return Class;
        }

        [[nodiscard]] constexpr inline uint64_t getAddress() const noexcept {
            return Address;
        }

        [[nodiscard]] constexpr inline bool IsNull() const noexcept {
            return sIsNull;
        }

        constexpr inline
        ObjcClassCategoryInfo &setName(const std::string &Value) noexcept {
            this->Name = Value;
            return *this;
        }

        constexpr inline
        ObjcClassCategoryInfo &setClass(const ObjcClassInfo *Value) noexcept {
            this->Class = Value;
            return *this;
        }

        constexpr
        inline ObjcClassCategoryInfo &setAddress(uint64_t Value) noexcept {
            this->Address = Value;
            return *this;
        }

        constexpr
        inline ObjcClassCategoryInfo &setIsNull(bool Value = true) noexcept {
            this->sIsNull = Value;
            return *this;
        }
    };

    struct ObjcClassCategoryCollection {
    public:
        using Error = ObjcParseError;
        using Info = ObjcClassCategoryInfo;
    protected:
        std::vector<std::unique_ptr<Info>> List;
    public:
        ObjcClassCategoryCollection() noexcept = default;

        ObjcClassCategoryCollection &
        CollectFrom(const uint8_t *Map,
                    const SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    const BindActionCollection &BindCollection,
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
             const BindActionCollection &BindCollection,
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
