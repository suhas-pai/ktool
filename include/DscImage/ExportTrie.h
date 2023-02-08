/*
 * DscImage/ExportTrie.h
 * © suhas pai
 */

#pragma once
#include "MachO/ExportTrie.h"

namespace DscImage {
    struct ExportTrieEntryCollectionNodeCreator :
        public MachO::ExportTrieEntryCollectionNodeCreator
    {
    protected:
        uint64_t ImageAddress = 0;
    public:
        explicit
        ExportTrieEntryCollectionNodeCreator(const MachO::SegmentList *SegList,
                                             uint64_t ImageAddress) noexcept;

        [[nodiscard]] const MachO::SegmentInfo *
        LookupInfoForAddress(
            MachO::ExportTrieFlags::Kind Kind,
            uint64_t Address,
            const MachO::SectionInfo **SectionOut) const noexcept override;
    };

    struct ExportTrieEntryCollection : public MachO::ExportTrieEntryCollection {
    protected:
        explicit ExportTrieEntryCollection() noexcept = default;
    public:
        using ChildNode = MachO::ExportTrieChildNode;
        using ExportChildNode = MachO::ExportTrieExportChildNode;
        using ParseOptions = MachO::ExportTrieEntryCollection::ParseOptions;
        using Error = ADT::TrieParseError;

        static auto
        Open(MachO::ExportTrieMap &Trie,
             const MachO::SegmentList *SegList,
             uint64_t ImageAddress,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) -> ExportTrieEntryCollection;
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
        using ParseOptions = MachO::ExportTrieExportCollection::ParseOptions;
        using Error = ADT::TrieParseError;

        static auto
        Open(const MachO::ExportTrieMap::ExportMap &Trie,
             const MachO::SegmentList *SegList,
             uint64_t ImageAddress,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ExportTrieExportCollection;
    };
}