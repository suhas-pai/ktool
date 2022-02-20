
//
//  include/ADT/Mach-O/ExportTrieUtil.h
//  ktool
//
//  Created by Suhas Pai on 6/5/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <memory>

#include "ADT/ByteVector.h"
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

        [[nodiscard]] inline ExportTrieChildNode *getParent() const noexcept {
            return get(Parent);
        }

        [[nodiscard]]
        inline ExportTrieChildNode *getPrevSibling() const noexcept {
            return get(PrevSibling);
        }

        [[nodiscard]]
        inline ExportTrieChildNode *getNextSibling() const noexcept {
            return get(NextSibling);
        }

        [[nodiscard]]
        inline ExportTrieChildNode *getFirstChild() const noexcept {
            return get(FirstChild);
        }

        [[nodiscard]]
        inline ExportTrieChildNode *getLastChild() const noexcept {
            return get(LastChild);
        }

        [[nodiscard]]
        static inline ExportTrieChildNode &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline
        const ExportTrieChildNode &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode &>(Node);
        }

        [[nodiscard]]
        static inline ExportTrieChildNode *get(TreeNode *Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] static inline
        const ExportTrieChildNode *get(const TreeNode *Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return String.length();
        }

        [[nodiscard]]
        constexpr ExportTrieExportKind getKind() const noexcept {
            return Kind;
        }

        [[nodiscard]]
        constexpr std::string_view getString() const noexcept {
            return String;
        }

        [[nodiscard]] constexpr bool isExport() const noexcept {
            return (getKind() != ExportTrieExportKind::None);
        }

        [[nodiscard]] constexpr bool isReexport() const noexcept {
            return (getKind() == ExportTrieExportKind::Reexport);
        }

        [[nodiscard]]
        inline ExportTrieExportChildNode *getIfExportNode() noexcept {
            if (this->isExport()) {
                return reinterpret_cast<ExportTrieExportChildNode *>(this);
            }

            return nullptr;
        }

        [[nodiscard]] inline
        const ExportTrieExportChildNode *getIfExportNode() const noexcept {
            if (!this->isExport()) {
                return nullptr;
            }

            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]]
        inline ExportTrieExportChildNode *getAsExportNode() noexcept {
            assert(this->isExport());
            return reinterpret_cast<ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline
        const ExportTrieExportChildNode *getAsExportNode() const noexcept {
            assert(this->isExport());
            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        inline
        ExportTrieChildNode &setParent(ExportTrieChildNode *Parent) noexcept {
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

        [[nodiscard]]
        constexpr const ExportTrieExportInfo &getInfo() const noexcept {
            return Info;
        }

        constexpr ExportTrieExportChildNode &
        setInfo(const ExportTrieExportInfo &Value) noexcept {
            this->Info = Value;
            return *this;
        }

        [[nodiscard]]
        inline TreeNode *createNew() const noexcept override {
            return new ExportTrieExportChildNode;
        }

        [[nodiscard]] inline const SegmentInfo *getSegment() const noexcept {
            assert(!this->isReexport());
            return Segment;
        }

        [[nodiscard]] inline const SectionInfo *getSection() const noexcept {
            assert(!this->isReexport());
            return Section;
        }

        constexpr
        ExportTrieChildNode &setSegment(const SegmentInfo *Value) noexcept {
            assert(!this->isReexport());

            this->Segment = Value;
            return *this;
        }

        constexpr
        ExportTrieChildNode &setSection(const SectionInfo *Value) noexcept {
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

        [[nodiscard]] inline ChildNode *getRoot() const noexcept {
            return reinterpret_cast<ChildNode *>(Root);
        }

        inline
        ChildNode *RemoveNode(ChildNode &Ref, bool RemoveParentLeafs) noexcept {
            auto &Node = reinterpret_cast<TreeNode &>(Ref);
            const auto Result = Tree::RemoveNode(Node, RemoveParentLeafs);

            return reinterpret_cast<ChildNode *>(Result);
        }

        using Iterator = TreeIterator<ChildNode>;
        using ConstIterator = TreeIterator<const ChildNode>;

        [[nodiscard]] inline Iterator begin() const noexcept {
            return Iterator(getRoot());
        }

        [[nodiscard]] inline Iterator end() const noexcept {
            return Iterator::Null();
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return ConstIterator(getRoot());
        }

        [[nodiscard]] inline ConstIterator cend() const noexcept {
            return ConstIterator::Null();
        }

        template <typename T>
        inline ExportTrieEntryCollection &forEach(const T &Callback) noexcept {
            forEachNode(Callback);
            return *this;
        }

        template <typename T>
        inline const ExportTrieEntryCollection &
        forEach(const T &Callback) const noexcept {
            forEachNode(Callback);
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
        constexpr const ExportTrieExportInfo &getInfo() const noexcept {
            return Export;
        }

        [[nodiscard]]
        constexpr ExportTrieExportInfo &getInfo() noexcept {
            return Export;
        }

        [[nodiscard]]
        constexpr const SegmentInfo *getSegment() const noexcept {
            return Segment;
        }

        [[nodiscard]]
        constexpr const SectionInfo *getSection() const noexcept {
            return Section;
        }

        constexpr ExportTrieExportCollectionEntryInfo &
        setSegment(const SegmentInfo *Value) noexcept {
            this->Segment = Value;
            return *this;
        }

        constexpr ExportTrieExportCollectionEntryInfo &
        setSection(const SectionInfo *Value) noexcept {
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

        [[nodiscard]] inline Iterator begin() noexcept {
            return EntryList.begin();
        }

        [[nodiscard]] inline Iterator end() noexcept {
            return EntryList.end();
        }

        [[nodiscard]] inline ConstIterator begin() const noexcept {
            return EntryList.cbegin();
        }

        [[nodiscard]] inline ConstIterator end() const noexcept {
            return EntryList.cend();
        }
    };
}
