//
//  include/ADT/Mach-O/ObjcInfo.h
//  ktool
//
//  Created by Suhas Pai on 9/4/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>

#include "ADT/Tree.h"
#include "Objc.h"

namespace MachO {
    struct ObjcClassCategoryInfo;
    struct ObjcClassInfo : public TreeNode {
    public:
        using CategoryListType = std::vector<ObjcClassCategoryInfo *>;
    protected:
        union {
            uint64_t Addr = 0;
            uint64_t BindAddr;
        };

        std::string Name;
        uint64_t DylibOrdinal = 0;

        bool sIsExternal : 1 = false;
        bool sIsNull : 1 = false;
        bool sIsSwift : 1 = false;

        ObjcClassRoFlags Flags;
        CategoryListType CategoryList;
    public:
        ObjcClassInfo() noexcept {}

        explicit ObjcClassInfo(const std::string_view Name) noexcept
        : Name(Name) {}

        [[nodiscard]]
        inline TreeNode *createNew() const noexcept override {
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

        [[nodiscard]] inline ObjcClassInfo *getLastChild() const noexcept {
            return get(LastChild);
        }

        [[nodiscard]]
        static inline ObjcClassInfo &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ObjcClassInfo &>(Node);
        }

        [[nodiscard]] static
        inline const ObjcClassInfo &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo &>(Node);
        }

        [[nodiscard]]
        static inline ObjcClassInfo *get(TreeNode *const Node) noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Node);
        }

        [[nodiscard]] static
        inline const ObjcClassInfo *get(const TreeNode *const Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo *>(Node);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return Name.length();
        }

        [[nodiscard]] inline ObjcClassInfo *getSuper() const noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Parent);
        }

        inline ObjcClassInfo &setSuper(ObjcClassInfo *const Parent) noexcept {
            this->Parent = reinterpret_cast<TreeNode *>(Parent);
            return *this;
        }

        inline ObjcClassInfo &
        setLastChild(const ObjcClassInfo *const LastChild) noexcept {
            this->LastChild = const_cast<ObjcClassInfo *>(LastChild);
            return *this;
        }

        [[nodiscard]]
        constexpr std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]]
        constexpr uint64_t getDylibOrdinal() const noexcept {
            return DylibOrdinal;
        }

        [[nodiscard]] constexpr bool isExternal() const noexcept {
            return sIsExternal;
        }

        [[nodiscard]] constexpr bool isNull() const noexcept {
            return sIsNull;
        }

        [[nodiscard]] constexpr bool isSwift() const noexcept {
            return sIsSwift;
        }

        [[nodiscard]]
        constexpr ObjcClassRoFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr uint64_t getAddress() const noexcept {
            assert(!this->isExternal());
            return Addr;
        }

        [[nodiscard]]
        constexpr uint64_t getBindAddress() const noexcept {
            assert(this->isExternal());
            return BindAddr;
        }

        [[nodiscard]]
        constexpr const CategoryListType &getCategoryList() const noexcept {
            return CategoryList;
        }

        [[nodiscard]]
        constexpr CategoryListType &getCategoryListRef() noexcept {
            return CategoryList;
        }

        inline ObjcClassInfo &setName(const std::string &Value) noexcept {
            this->Name = Value;
            return *this;
        }

        inline ObjcClassInfo &setDylibOrdinal(const uint64_t Value) noexcept {
            this->DylibOrdinal = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsExternal(const bool Value = true) noexcept {
            this->sIsExternal = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsNull(const bool Value = true) noexcept {
            this->sIsNull = Value;
            return *this;
        }

        inline ObjcClassInfo &setIsSwift(const bool Value = true) noexcept {
            this->sIsSwift = Value;
            return *this;
        }

        constexpr
        ObjcClassInfo &setFlags(const ObjcClassRoFlags &Value) noexcept {
            this->Flags = Value;
            return *this;
        }

        inline ObjcClassInfo &setAddr(const uint64_t Value) noexcept {
            assert(!this->isExternal());

            this->Addr = Value;
            return *this;
        }

        inline ObjcClassInfo &setBindAddr(const uint64_t Value) noexcept {
            assert(this->isExternal());

            this->BindAddr = Value;
            return *this;
        }

        [[nodiscard]]
        constexpr bool operator==(const ObjcClassInfo &Rhs) noexcept {
            return (Addr == Rhs.Addr);
        }

        [[nodiscard]]
        constexpr bool operator!=(const ObjcClassInfo &Rhs) noexcept {
            return !(*this == Rhs);
        }
    };

    struct ObjcClassCategoryInfo {
    protected:
        std::string Name;
        const ObjcClassInfo *Class = nullptr;
        uint64_t Address = 0;
        bool sIsNull : 1 = false;
    public:
        ObjcClassCategoryInfo() {}

        [[nodiscard]] constexpr std::string_view getName() const noexcept {
            return Name;
        }

        [[nodiscard]] constexpr const ObjcClassInfo *getClass() const noexcept {
            return Class;
        }

        [[nodiscard]] constexpr uint64_t getAddress() const noexcept {
            return Address;
        }

        [[nodiscard]] constexpr bool isNull() const noexcept {
            return sIsNull;
        }

        constexpr
        ObjcClassCategoryInfo &setName(const std::string &Value) noexcept {
            this->Name = Value;
            return *this;
        }

        constexpr ObjcClassCategoryInfo &
        setClass(const ObjcClassInfo *const Value) noexcept {
            this->Class = Value;
            return *this;
        }

        constexpr
        ObjcClassCategoryInfo &setAddress(const uint64_t Value) noexcept {
            this->Address = Value;
            return *this;
        }

        constexpr
        ObjcClassCategoryInfo &setIsNull(const bool Value = true) noexcept {
            this->sIsNull = Value;
            return *this;
        }
    };
}
