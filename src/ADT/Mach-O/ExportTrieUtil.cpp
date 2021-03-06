//
//  src/ADT/Mach-O/ExportTrieUtil.cpp
//  ktool
//
//  Created by Suhas Pai on 6/5/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include "ExportTrieUtil.h"

namespace MachO {
    const SegmentInfo *
    ExportTrieEntryCollection::LookupInfoForAddress(
        const SegmentInfoCollection *Collection,
        uint64_t Address,
        const SectionInfo **SectionOut) const noexcept
    {
        const auto Segment = Collection->FindSegmentContainingAddress(Address);
        if (Segment != nullptr) {
            *SectionOut = Segment->FindSectionContainingAddress(Address);
        }

        return Segment;
    }

    ExportTrieEntryCollection::ChildNode *
    ExportTrieEntryCollection::MakeNodeForEntryInfo(
        const ExportTrieIterateInfo &Info,
        const SegmentInfoCollection *Collection) noexcept
    {
        auto Node = static_cast<ChildNode *>(nullptr);
        if (Info.isExport()) {
            Node = new ExportChildNode();
        } else {
            Node = new ChildNode();
        }

        Node->Kind = Info.getKind();
        Node->String = Info.getString();

        if (Info.isExport()) {
            auto ExportNode = Node->getAsExportNode();
            const auto &ExportInfo = Info.getExportInfo();

            if (!ExportInfo.isReexport()) {
                const auto Addr = ExportInfo.getImageOffset();
                if (Collection != nullptr) {
                    auto Section = static_cast<const SectionInfo *>(nullptr);
                    const auto Segment =
                        LookupInfoForAddress(Collection, Addr, &Section);

                    ExportNode->setSegment(Segment);
                    ExportNode->setSection(Section);
                }
            }

            ExportNode->setInfo(ExportInfo);
        }

        return Node;
    }

    void
    ExportTrieEntryCollection::ParseFromTrie(
        const ConstExportTrieList &Trie,
        const SegmentInfoCollection *SegmentCollection,
        Error *Error) noexcept
    {
        auto Iter = Trie.begin();
        const auto End = Trie.end();

        if (Iter == End) {
            return;
        }

        setRoot(MakeNodeForEntryInfo(*Iter, SegmentCollection));

        auto Parent = getRoot();
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

                return;
            }

            const auto DepthLevel = Iter->getDepthLevel();
            if (PrevDepthLevel > DepthLevel) {
                MoveUpParentHierarchy(PrevDepthLevel - DepthLevel);
            }

            const auto Current = MakeNodeForEntryInfo(*Iter, SegmentCollection);

            Parent->AddChild(*Current);
            if (Iter->getNode().getChildCount() != 0) {
                Parent = Current;
            }

            PrevDepthLevel = DepthLevel;
        }
    }

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(
        const ConstExportTrieList &Trie,
        const SegmentInfoCollection *SegmentCollection,
        Error *Error)
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(Trie, SegmentCollection, Error);

        return Result;
    }

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(
        const ConstExportTrieExportList &Trie,
        const SegmentInfoCollection *SegmentCollection,
        Error *Error) noexcept
    {
        auto Result = ExportTrieExportCollection();
        for (auto &Iter : Trie) {
            auto Entry = EntryInfo();
            const auto &ExportInfo = Iter.getExportInfo();

            if (!ExportInfo.isReexport()) {
                if (SegmentCollection != nullptr) {
                    const auto Addr = ExportInfo.getImageOffset();
                    const auto Segment =
                        SegmentCollection->FindSegmentContainingAddress(Addr);

                    auto Section = static_cast<const SectionInfo *>(nullptr);
                    if (Segment != nullptr) {
                        Section = Segment->FindSectionContainingAddress(Addr);
                    }

                    Entry.setSegment(Segment);
                    Entry.setSection(Section);
                }
            }

            Result.EntryList.emplace_back(std::make_unique<EntryInfo>(Entry));
        }

        return Result;
    }
}
