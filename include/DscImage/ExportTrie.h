/*
 * DscImage/ExportTrie.h
 * © suhas pai
 */

#pragma once
#include "MachO/ExportTrie.h"

namespace DscImage {
    struct ExportTrieEntryCollection : public MachO::ExportTrieEntryCollection {
    protected:
        explicit ExportTrieEntryCollection(uint64_t ImageAddress) noexcept;

        uint64_t ImageAddress = 0;
    public:
        using ChildNode = MachO::ExportTrieChildNode;
        using ExportChildNode = MachO::ExportTrieExportChildNode;
        using Error = MachO::ExportTrieParseError;

        static ExportTrieEntryCollection
        Open(const MachO::ExportTrieMap &Trie,
             const MachO::SegmentList *SegList,
             uint64_t ImageAddress,
             Error *ErrorOut = nullptr);

        [[nodiscard]] const MachO::SegmentInfo *
        LookupInfoForAddress(
            const MachO::SegmentList &SegList,
            MachO::ExportTrieFlags::Kind Kind,
            uint64_t Address,
            const MachO::SectionInfo **SectionOut) const noexcept override;
    };

    struct ExportTrieExportCollection :
        public MachO::ExportTrieExportCollection
    {
    protected:
        explicit ExportTrieExportCollection(uint64_t ImageAddress) noexcept;
        uint64_t ImageAddress = 0;
    public:
        using ChildNode = MachO::ExportTrieChildNode;
        using ExportChildNode = MachO::ExportTrieExportChildNode;
        using Error = MachO::ExportTrieParseError;

        static ExportTrieExportCollection
        Open(const MachO::ExportTrieMap::ExportMap &Trie,
             const MachO::SegmentList *SegList,
             uint64_t ImageAddress,
             Error *ErrorOut = nullptr) noexcept;

        [[nodiscard]] const MachO::SegmentInfo *
        LookupInfoForAddress(
            const MachO::SegmentList &SegList,
            MachO::ExportTrieFlags::Kind Kind,
            uint64_t Address,
            const MachO::SectionInfo **SectionOut) const noexcept override;
    };
}