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
    struct ObjcClassInfo : public BasicTreeNode {
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

        [[nodiscard]] constexpr inline bool isExternal() const noexcept {
            return sIsExternal;
        }

        [[nodiscard]] constexpr inline bool isNull() const noexcept {
            return sIsNull;
        }

        [[nodiscard]] constexpr inline bool isSwift() const noexcept {
            return sIsSwift;
        }

        [[nodiscard]]
        constexpr inline ObjcClassRoFlags getFlags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr inline uint64_t getAddress() const noexcept {
            assert(!this->isExternal());
            return Addr;
        }

        [[nodiscard]]
        constexpr inline uint64_t getBindAddress() const noexcept {
            assert(this->isExternal());
            return BindAddr;
        }

        [[nodiscard]] constexpr
        inline const CategoryListType &getCategoryList() const noexcept {
            return CategoryList;
        }

        [[nodiscard]]
        constexpr inline CategoryListType &getCategoryListRef() noexcept {
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
            assert(!this->isExternal());

            this->Addr = Value;
            return *this;
        }

        inline ObjcClassInfo &setBindAddr(uint64_t Value) noexcept {
            assert(this->isExternal());

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

        [[nodiscard]] constexpr inline bool isNull() const noexcept {
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
}
