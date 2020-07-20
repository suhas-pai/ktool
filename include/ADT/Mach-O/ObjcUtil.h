//
//  include/ADT/Mach-O/ObjcUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/13/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <unordered_map>
#include <vector>

#include "ADT/Tree.h"

#include "BindUtil.h"
#include "DeVirtualizer.h"
#include "Objc.h"
#include "SegmentUtil.h"

namespace MachO {
    struct ObjcClassInfoCollection;
    struct ObjcClassCategoryInfo;
    struct ObjcClassInfo : public BasicTreeNode {
        friend ObjcClassInfoCollection;
    public:
        union {
            uint64_t Addr = 0;
            uint64_t BindAddr;
        };

        std::string Name;
        uint64_t DylibOrdinal = 0;

        bool IsExternal : 1;
        bool IsNull : 1;
        bool IsSwift : 1;

        ObjcClassRoFlags Flags;
        std::vector<ObjcClassCategoryInfo *> CategoryList;

        ObjcClassInfo() noexcept : IsExternal(false), IsNull(false) {}

        explicit ObjcClassInfo(const std::string_view &Name) noexcept
        : Name(Name), IsExternal(false), IsNull(false) {}

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
        inline bool operator==(const ObjcClassInfo &Rhs) noexcept {
            return (Addr == Rhs.Addr);
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
              bool Is64Bit,
              bool IsBigEndian,
              Error *ErrorOut) noexcept;

        static inline ObjcClassInfoCollection
        Open(const uint8_t *Map,
             const SegmentInfoCollection &SegmentCollection,
             const ConstDeVirtualizer &DeVirtualizer,
             const BindActionCollection &BindCollection,
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

        Iterator begin() const noexcept;
        Iterator end() const noexcept;

        ConstIterator cbegin() const noexcept;
        ConstIterator cend() const noexcept;
    };

    struct ObjcClassCategoryInfo {
        std::string Name;
        ObjcClassInfo *Class = nullptr;

        union {
            uint64_t Address = 0;
            uint64_t BindAddr;
        };

        bool IsNull : 1;
    };

    struct ObjcClassCategoryCollection {
    public:
        using Info = ObjcClassCategoryInfo;
    protected:
        std::vector<Info *> List;
    public:
        ObjcClassCategoryCollection() noexcept = default;
        using Error = ObjcParseError;

        ObjcClassCategoryCollection &
        CollectFrom(const uint8_t *Map,
                    const SegmentInfoCollection &SegmentCollection,
                    const ConstDeVirtualizer &DeVirtualizer,
                    const BindActionCollection &BindCollection,
                    ObjcClassInfoCollection *ClassInfoTree,
                    bool Is64Bit,
                    bool IsBigEndian,
                    Error *ErrorOut) noexcept;

        static inline ObjcClassCategoryCollection
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
    };
}
