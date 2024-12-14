//
//  ADT/Mach-O/ObjcInfo.h
//  ktool
//
//  Created by Suhas Pai on 9/4/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
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
        explicit ObjcClassInfo() noexcept {}
        explicit ObjcClassInfo(const std::string_view Name) noexcept
        : Name(Name) {}

        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ObjcClassInfo;
        }

        [[nodiscard]] static inline auto get(TreeNode &Node) noexcept {
            return reinterpret_cast<ObjcClassInfo &>(Node);
        }

        [[nodiscard]]
        static inline auto get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo &>(Node);
        }

        [[nodiscard]] static inline auto get(TreeNode *const Node) noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Node);
        }

        [[nodiscard]]
        static inline auto get(const TreeNode *const Node) noexcept {
            return reinterpret_cast<const ObjcClassInfo *>(Node);
        }

        [[nodiscard]] inline auto getParent() const noexcept {
            return this->get(Parent);
        }

        [[nodiscard]] inline auto getPrevSibling() const noexcept {
            return this->get(PrevSibling);
        }

        [[nodiscard]] inline auto getNextSibling() const noexcept {
            return this->get(NextSibling);
        }

        [[nodiscard]] inline auto getFirstChild() const noexcept {
            return this->get(FirstChild);
        }

        [[nodiscard]] inline auto getLastChild() const noexcept {
            return this->get(LastChild);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return this->Name.length();
        }

        [[nodiscard]] inline auto getSuper() const noexcept {
            return reinterpret_cast<ObjcClassInfo *>(Parent);
        }

        inline auto setSuper(ObjcClassInfo *const Parent) noexcept
            -> decltype(*this)
        {
            this->Parent = reinterpret_cast<TreeNode *>(Parent);
            return *this;
        }

        inline auto
        setLastChild(const ObjcClassInfo *const LastChild) noexcept
            -> decltype(*this)
        {
            this->LastChild = const_cast<ObjcClassInfo *>(LastChild);
            return *this;
        }

        [[nodiscard]] constexpr auto getName() const noexcept
            -> std::string_view
        {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getDylibOrdinal() const noexcept {
            return this->DylibOrdinal;
        }

        [[nodiscard]] constexpr auto isExternal() const noexcept {
            return this->sIsExternal;
        }

        [[nodiscard]] constexpr auto isNull() const noexcept {
            return this->sIsNull;
        }

        [[nodiscard]] constexpr auto isSwift() const noexcept {
            return this->sIsSwift;
        }

        [[nodiscard]] constexpr auto getFlags() const noexcept {
            return this->Flags;
        }

        [[nodiscard]] constexpr auto getAddress() const noexcept {
            assert(!this->isExternal());
            return this->Addr;
        }

        [[nodiscard]] constexpr auto getBindAddress() const noexcept {
            assert(this->isExternal());
            return this->BindAddr;
        }

        [[nodiscard]] constexpr auto &getCategoryList() const noexcept {
            return this->CategoryList;
        }

        [[nodiscard]] constexpr auto &getCategoryListRef() noexcept {
            return this->CategoryList;
        }

        inline auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        inline auto setName(std::string &&Name) noexcept
            -> decltype(*this)
        {
            this->Name = std::move(Name);
            return *this;
        }

        inline auto setDylibOrdinal(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->DylibOrdinal = Value;
            return *this;
        }

        inline auto setIsExternal(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsExternal = Value;
            return *this;
        }

        inline auto setIsNull(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsNull = Value;
            return *this;
        }

        inline auto setIsSwift(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsSwift = Value;
            return *this;
        }

        constexpr auto setFlags(const ObjcClassRoFlags &Value) noexcept
            -> decltype(*this)
        {
            this->Flags = Value;
            return *this;
        }

        inline auto setAddr(const uint64_t Value) noexcept -> decltype(*this) {
            assert(!this->isExternal());

            this->Addr = Value;
            return *this;
        }

        inline auto setBindAddr(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            assert(this->isExternal());

            this->BindAddr = Value;
            return *this;
        }

        [[nodiscard]]
        constexpr auto operator==(const ObjcClassInfo &Rhs) noexcept {
            return Addr == Rhs.Addr;
        }

        [[nodiscard]]
        constexpr auto operator!=(const ObjcClassInfo &Rhs) noexcept {
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

        [[nodiscard]] constexpr auto getName() const noexcept
            -> std::string_view
        {
            return this->Name;
        }

        [[nodiscard]] constexpr auto getClass() const noexcept {
            return this->Class;
        }

        [[nodiscard]] constexpr auto getAddress() const noexcept {
            return this->Address;
        }

        [[nodiscard]] constexpr auto isNull() const noexcept {
            return this->sIsNull;
        }

        constexpr auto setName(const std::string_view Name) noexcept
            -> decltype(*this)
        {
            this->Name = Name;
            return *this;
        }

        constexpr auto setName(std::string &&Name) noexcept -> decltype(*this) {
            this->Name = Name;
            return *this;
        }

        constexpr auto setClass(const ObjcClassInfo *const Value) noexcept
            -> decltype(*this)
        {
            this->Class = Value;
            return *this;
        }

        constexpr auto setAddress(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Address = Value;
            return *this;
        }

        constexpr auto setIsNull(const bool Value = true) noexcept
            -> decltype(*this)
        {
            this->sIsNull = Value;
            return *this;
        }
    };
}
