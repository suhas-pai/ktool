
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
#include "SegmentUtil.h"

namespace MachO {
    struct ExportTrieEntryCollection;
    struct ExportTrieExportChildNode;
    struct ExportTrieChildNode : public BasicTreeNode {
        friend ExportTrieEntryCollection;
    public:
        ~ExportTrieChildNode() noexcept = default;
        using BasicTreeNode::BasicTreeNode;

        MachO::ExportTrieExportKind Kind;
        std::string String;

        [[nodiscard]]
        inline BasicTreeNode *createNew() const noexcept override {
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
        inline ExportTrieChildNode *getLongestChild() const noexcept {
            return get(LongestChild);
        }

        [[nodiscard]]
        static inline ExportTrieChildNode &get(BasicTreeNode &Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline
        const ExportTrieChildNode &get(const BasicTreeNode &Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode &>(Node);
        }

        [[nodiscard]]
        static inline ExportTrieChildNode *get(BasicTreeNode *Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] static inline
        const ExportTrieChildNode *get(const BasicTreeNode *Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode *>(Node);
        }

        [[nodiscard]] inline uint64_t GetLength() const noexcept override {
            return String.length();
        }

        [[nodiscard]] inline bool IsExport() const noexcept {
            return (Kind != MachO::ExportTrieExportKind::None);
        }

        [[nodiscard]]
        inline ExportTrieExportChildNode *getAsExportNode() noexcept {
            assert(IsExport());
            return reinterpret_cast<ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline
        const ExportTrieExportChildNode *getAsExportNode() const noexcept {
            assert(IsExport());
            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        inline
        ExportTrieChildNode &setParent(ExportTrieChildNode *Parent) noexcept {
            this->Parent = Parent;
            return *this;
        }
    };

    struct ExportTrieExportChildNode : public ExportTrieChildNode {
        friend ExportTrieEntryCollection;
    protected:
        const MachO::SegmentInfo *Segment = nullptr;
        const MachO::SectionInfo *Section = nullptr;
    public:
        using ExportTrieChildNode::ExportTrieChildNode;
        MachO::ExportTrieExportInfo Info;

        [[nodiscard]]
        inline const MachO::SegmentInfo *getSegment() const noexcept {
            assert(Kind != MachO::ExportTrieExportKind::Reexport);
            return Segment;
        }
        [[nodiscard]]
        inline const MachO::SectionInfo *getSection() const noexcept {
            assert(Kind != MachO::ExportTrieExportKind::Reexport);
            return Section;
        }

        [[nodiscard]]
        inline BasicTreeNode *createNew() const noexcept override {
            return new ExportTrieExportChildNode;
        }
    };

    struct ExportTrieEntryCollection : public BasicTree {
    public:
        using ChildNode = ExportTrieChildNode;
        using ExportChildNode = ExportTrieExportChildNode;
        using Error = ExportTrieParseError;
    protected:
        explicit ExportTrieEntryCollection() noexcept = default;

        virtual const SegmentInfo *
        LookupInfoForAddress(const MachO::SegmentInfoCollection *Collection,
                             uint64_t Address,
                             const SectionInfo **SectionOut) const noexcept;

        ChildNode *
        GetNodeForEntryInfo(const MachO::ExportTrieIterateInfo &Info,
                            const SegmentInfoCollection *Collection) noexcept;

        void
        ParseFromTrie(const MachO::ConstExportTrieList &Trie,
                      const MachO::SegmentInfoCollection *SegmentCollection,
                      Error *Error) noexcept;
    public:
        static ExportTrieEntryCollection
        Open(const MachO::ConstExportTrieList &Trie,
             const MachO::SegmentInfoCollection *SegmentCollection,
             Error *Error);

        [[nodiscard]] inline ChildNode *getRoot() const noexcept {
            return reinterpret_cast<ChildNode *>(Root);
        }

        inline ChildNode *
        RemoveNode(ChildNode &Ref, bool RemoveParentLeafs) noexcept {
            auto &Node = reinterpret_cast<BasicTreeNode &>(Ref);
            const auto Result = BasicTree::RemoveNode(Node, RemoveParentLeafs);

            return reinterpret_cast<ChildNode *>(Result);
        }

        using Iterator = TreeIterator<ChildNode>;
        using ConstIterator = TreeIterator<const ChildNode>;

        [[nodiscard]] Iterator begin() const noexcept;
        [[nodiscard]] Iterator end() const noexcept;

        [[nodiscard]] ConstIterator cbegin() const noexcept;
        [[nodiscard]] ConstIterator cend() const noexcept;
    };

    struct ExportTrieExportCollectionEntryInfo {
        MachO::ExportTrieExportInfo Export;

        const MachO::SegmentInfo *Segment = nullptr;
        const MachO::SectionInfo *Section = nullptr;
    };

    struct ExportTrieExportCollection : public BasicTree {
    public:
        using EntryInfo = ExportTrieExportCollectionEntryInfo;
        using Error = ExportTrieParseError;
    private:
        explicit ExportTrieExportCollection() noexcept = default;
    protected:
        std::vector<EntryInfo> EntryList;
    public:
        static ExportTrieExportCollection
        Open(const MachO::ConstExportTrieExportList &Trie,
             const MachO::SegmentInfoCollection *SegmentCollection,
             Error *Error) noexcept;

        using Iterator = std::vector<EntryInfo>::const_iterator;
        using ConstIterator = std::vector<EntryInfo>::const_iterator;

        [[nodiscard]] inline Iterator begin() const noexcept {
            return EntryList.begin();
        }

        [[nodiscard]] inline Iterator end() const noexcept {
            return EntryList.end();
        }

        [[nodiscard]] inline ConstIterator cbegin() const noexcept {
            return EntryList.cbegin();
        }

        [[nodiscard]] inline ConstIterator cend() const noexcept {
            return EntryList.cend();
        }
    };
}
