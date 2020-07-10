//
//  src/ADT/Mach-O/ExportTrieUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 6/5/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ExportTrieUtil.h"

namespace MachO {
    ExportTrieEntryCollection::ChildNode *
    ExportTrieEntryCollection::GetNodeForEntryInfo(
        const MachO::ExportTrieIterateInfo &Info,
        const SegmentInfoCollection *Collection) noexcept
    {
        auto Node = static_cast<ChildNode *>(nullptr);
        if (Info.IsExport()) {
            Node = new ExportChildNode();
        } else {
            Node = new ChildNode();
        }

        Node->ExportKind = Info.Export.getKind();
        Node->String = Info.String;

        if (Info.IsExport() && !Info.Export.getFlags().IsReexport()) {
            const auto Addr = Info.Export.getImageOffset();
            auto ExportNode = reinterpret_cast<ExportChildNode *>(Node);

            if (Collection != nullptr) {
                const auto Segment =
                    Collection->FindSegmentContainingAddress(Addr);

                if (Segment != nullptr) {
                    ExportNode->Segment = Segment;
                    ExportNode->Section =
                        Segment->FindSectionContainingAddress(Addr);
                }
            }

            ExportNode->Address = Addr;
        }

        return Node;
    }

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(
        const MachO::ConstExportTrieList &Trie,
        const MachO::SegmentInfoCollection *SegmentCollection,
        Error *Error)
    {
        auto Iter = Trie.begin();
        const auto End = Trie.end();

        auto Result = ExportTrieEntryCollection();
        Result.Root = GetNodeForEntryInfo(*Iter, SegmentCollection);

        auto Parent = reinterpret_cast<ChildNode *>(Result.Root);
        auto PrevDepthLevel = 1;

        const auto MoveUpParentHierarchy = [&](uint8_t Amt) noexcept {
            for (auto I = uint8_t(); I != Amt; I++) {
                Parent = Parent->getParent();
            }
        };

        for (Iter++; Iter != End; Iter++) {
            if (Iter.hasError()) {
                if (Error != nullptr) {
                    *Error = Iter.getError();
                }

                return Result;
            }

            const auto DepthLevel = Iter->getDepthLevel();
            if (PrevDepthLevel > DepthLevel) {
                MoveUpParentHierarchy(PrevDepthLevel - DepthLevel);
            }

            const auto Current = GetNodeForEntryInfo(*Iter, SegmentCollection);
            Parent->AddChild(*Current);

            if (Iter->getNode().ChildCount != 0) {
                Parent = Current;
            }

            PrevDepthLevel = DepthLevel;
        }

        return Result;
    }

    ExportTrieEntryCollection::Iterator
    ExportTrieEntryCollection::begin() const noexcept {
        return Iterator(getRoot());
    }

    ExportTrieEntryCollection::Iterator
    ExportTrieEntryCollection::end() const noexcept {
        return Iterator(nullptr);
    }

    ExportTrieEntryCollection::ConstIterator
    ExportTrieEntryCollection::cbegin() const noexcept {
        return ConstIterator(getRoot());
    }

    ExportTrieEntryCollection::ConstIterator
    ExportTrieEntryCollection::cend() const noexcept {
        return ConstIterator(nullptr);
    }

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(
        const MachO::ConstExportTrieExportList &Trie,
        const MachO::SegmentInfoCollection *SegmentCollection,
        Error *Error)
    {
        auto Result = ExportTrieExportCollection();
        for (auto &Iter : Trie) {
            auto Entry = EntryInfo();
            if (!Iter.Export.getFlags().IsReexport()) {
                if (SegmentCollection != nullptr) {
                    const auto Addr = Iter.Export.getImageOffset();
                    Entry.Segment =
                        SegmentCollection->FindSegmentContainingAddress(Addr);

                    if (Entry.Segment != nullptr) {
                        Entry.Section =
                            Entry.Segment->FindSectionContainingAddress(Addr);
                    }
                }
            }

            Result.EntryList.emplace_back(std::move(Entry));
        }

        return Result;
    }
}
