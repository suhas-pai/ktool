
//
//  ADT/Mach-O/ExportTrieUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/5/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>

#include "ADT/Tree.h"

#include "ExportTrie.h"
#include "SegmentInfo.h"
#include "SegmentUtil.h"

namespace MachO {
    struct ExportTrieEntryCollection;
    struct ExportTrieExportChildNode;
    struct ExportTrieChildNode : public TreeNode {
        friend ExportTrieEntryCollection;
    protected:
        ExportTrieExportKind Kind;
        std::string String;
    public:
        ~ExportTrieChildNode() noexcept = default;
        using TreeNode::TreeNode;

        [[nodiscard]]
        inline TreeNode *createNew() const noexcept override {
            return new ExportTrieChildNode;
        }

        [[nodiscard]]
        static inline auto &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode &>(Node);
        }

        [[nodiscard]]
        static inline auto &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline
        const ExportTrieChildNode *get(const TreeNode *Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] static inline auto get(TreeNode *Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] inline auto getParent() const noexcept {
            return this->get(this->Parent);
        }

        [[nodiscard]] inline auto getPrevSibling() const noexcept {
            return this->get(this->PrevSibling);
        }

        [[nodiscard]] inline auto getNextSibling() const noexcept {
            return this->get(this->NextSibling);
        }

        [[nodiscard]]
        inline auto getFirstChild() const noexcept {
            return this->get(this->FirstChild);
        }

        [[nodiscard]] inline auto getLastChild() const noexcept {
            return this->get(this->LastChild);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return this->String.length();
        }

        [[nodiscard]] constexpr auto getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]] constexpr auto getString() const noexcept {
            return std::string_view(String);
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return this->getKind() != ExportTrieExportKind::None;
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return this->getKind() == ExportTrieExportKind::Reexport;
        }

        [[nodiscard]] inline auto getIfExportNode() noexcept
            -> ExportTrieExportChildNode *
        {
            if (this->isExport()) {
                return reinterpret_cast<ExportTrieExportChildNode *>(this);
            }

            return nullptr;
        }

        [[nodiscard]] inline auto getIfExportNode() const noexcept
            -> const ExportTrieExportChildNode *
        {
            if (!this->isExport()) {
                return nullptr;
            }

            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() noexcept {
            assert(this->isExport());
            return reinterpret_cast<ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() const noexcept {
            assert(this->isExport());
            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        constexpr auto setParent(ExportTrieChildNode *Parent) noexcept
            -> decltype(*this)
        {
            this->Parent = Parent;
            return *this;
        }
    };

    struct ExportTrieExportChildNode : public ExportTrieChildNode {
    protected:
        ExportTrieExportInfo Info;

        const SegmentInfo *Segment = nullptr;
        const SectionInfo *Section = nullptr;
    public:
        using ExportTrieChildNode::ExportTrieChildNode;

        [[nodiscard]] constexpr auto &getInfo() const noexcept {
            return this->Info;
        }

        constexpr auto setInfo(const ExportTrieExportInfo &Value) noexcept
            -> decltype(*this)
        {
            this->Info = Value;
            return *this;
        }

        [[nodiscard]]
        inline TreeNode *createNew() const noexcept override {
            return new ExportTrieExportChildNode;
        }

        [[nodiscard]] inline auto getSegment() const noexcept {
            assert(!this->isReexport());
            return this->Segment;
        }

        [[nodiscard]] inline auto getSection() const noexcept {
            assert(!this->isReexport());
            return this->Section;
        }

        constexpr auto setSegment(const SegmentInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->isReexport());

            this->Segment = Value;
            return *this;
        }

        constexpr auto setSection(const SectionInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->isReexport());

            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieEntryCollection : public Tree {
    public:
        using ChildNode = ExportTrieChildNode;
        using ExportChildNode = ExportTrieExportChildNode;
        using Error = ExportTrieParseError;
    protected:
        explicit ExportTrieEntryCollection() noexcept = default;

        [[nodiscard]] virtual const SegmentInfo *
        LookupInfoForAddress(const SegmentInfoCollection *Collection,
                             uint64_t Address,
                             const SectionInfo **SectionOut) const noexcept;

        [[nodiscard]] ChildNode *
        MakeNodeForEntryInfo(const ExportTrieIterateInfo &Info,
                             const SegmentInfoCollection *Collection) noexcept;

        void
        ParseFromTrie(const ConstExportTrieList &Trie,
                      const SegmentInfoCollection *SegmentCollection,
                      Error *Error) noexcept;
    public:
        static ExportTrieEntryCollection
        Open(const ConstExportTrieList &Trie,
             const SegmentInfoCollection *SegmentCollection,
             Error *ErrorOut = nullptr);

        [[nodiscard]] inline auto getRoot() const noexcept {
            return reinterpret_cast<ChildNode *>(this->Root);
        }

        inline auto
        RemoveNode(ChildNode &Ref, const bool RemoveParentLeafs) noexcept {
            auto &Node = reinterpret_cast<TreeNode &>(Ref);
            const auto Result = Tree::RemoveNode(Node, RemoveParentLeafs);

            return reinterpret_cast<ChildNode *>(Result);
        }

        using Iterator = TreeIterator<ChildNode>;
        using ConstIterator = TreeIterator<const ChildNode>;

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(this->getRoot());
        }

        [[nodiscard]] inline auto end() const noexcept {
            return Iterator::Null();
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return ConstIterator(this->getRoot());
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ConstIterator::Null();
        }

        template <typename T>
        inline auto forEach(const T &Callback) noexcept
            -> decltype(*this)
        {
            this->forEachNode(Callback);
            return *this;
        }

        template <typename T>
        inline auto forEach(const T &Callback) const noexcept
            -> decltype(*this)
        {
            this->forEachNode(Callback);
            return *this;
        }
    };

    struct ExportTrieExportCollectionEntryInfo {
    protected:
        ExportTrieExportInfo Export;

        const SegmentInfo *Segment = nullptr;
        const SectionInfo *Section = nullptr;
    public:
        [[nodiscard]]
        constexpr auto &getInfo() const noexcept {
            return Export;
        }

        [[nodiscard]] constexpr auto getInfo() noexcept {
            return this->Export;
        }

        [[nodiscard]]
        constexpr auto getSegment() const noexcept {
            return this->Segment;
        }

        [[nodiscard]]
        constexpr auto getSection() const noexcept {
            return this->Section;
        }

        constexpr auto setSegment(const SegmentInfo *const Value) noexcept
            -> decltype(*this)
        {
            this->Segment = Value;
            return *this;
        }

        constexpr auto
        setSection(const SectionInfo *const Value) noexcept -> decltype(*this) {
            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieExportCollection : public Tree {
    public:
        using EntryInfo = ExportTrieExportCollectionEntryInfo;
        using Error = ExportTrieParseError;
        using EntryListType = std::vector<std::unique_ptr<EntryInfo>>;
    private:
        explicit ExportTrieExportCollection() noexcept = default;
    protected:
        EntryListType EntryList;
    public:
        static ExportTrieExportCollection
        Open(const ConstExportTrieExportList &Trie,
             const SegmentInfoCollection *SegmentCollection,
             Error *Error) noexcept;

        using Iterator = EntryListType::iterator;
        using ConstIterator = EntryListType::const_iterator;

        [[nodiscard]] inline auto begin() noexcept {
            return this->EntryList.begin();
        }

        [[nodiscard]] inline auto end() noexcept {
            return this->EntryList.end();
        }

        [[nodiscard]] inline auto begin() const noexcept {
            return this->EntryList.cbegin();
        }

        [[nodiscard]] inline auto end() const noexcept {
            return this->EntryList.cend();
        }
    };
}
