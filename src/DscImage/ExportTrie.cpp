/*
 * DscImage/ExportTrie.cpp
 * © suhas pai
 */

#include "DscImage/ExportTrie.h"
#include "MachO/ExportTrie.h"
#include "Utils/Overflow.h"

namespace DscImage {
    ExportTrieEntryCollectionNodeCreator::ExportTrieEntryCollectionNodeCreator(
        const MachO::SegmentList *const SegList,
        const uint64_t ImageAddress) noexcept
    : MachO::ExportTrieEntryCollectionNodeCreator(SegList),
      ImageAddress(ImageAddress) {}

    auto
    ExportTrieEntryCollection::Open(MachO::ExportTrieMap &Trie,
                                    const MachO::SegmentList *const SegList,
                                    const uint64_t ImageAddress,
                                    const ParseOptions &Options,
                                    Error *const ErrorOut)
        -> ExportTrieEntryCollection
    {
        auto Result = ExportTrieEntryCollection();
        auto NodeParser =
            ExportTrieEntryCollectionNodeCreator(SegList, ImageAddress);

        Result.MachO::ExportTrieEntryCollection::Base::ParseFromTrie(Trie,
                                                                     NodeParser,
                                                                     Options,
                                                                     ErrorOut);
        return Result;
    }

    const MachO::SegmentInfo *
    ExportTrieEntryCollectionNodeCreator::LookupInfoForAddress(
        const MachO::ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const MachO::SectionInfo **const SectionOut) const noexcept
    {
        const uint64_t FullAddress =
            Kind == MachO::ExportTrieFlags::Kind::Absolute ?
                Address :
                Utils::AddAndCheckOverflow(ImageAddress, Address).value();

        const auto Segment = SegList->findSegmentWithVmAddr(FullAddress);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(FullAddress);
        }

        return Segment;
    }

    ExportTrieExportCollection::ExportTrieExportCollection(
        const uint64_t ImageAddress) noexcept
    : MachO::ExportTrieExportCollection(), ImageAddress(ImageAddress) {}

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(
        const MachO::ExportTrieMap::ExportMap &Trie,
        const MachO::SegmentList *const SegList,
        const uint64_t ImageAddress,
        const ParseOptions &Options,
        Error *const ErrorOut) noexcept
    {
        auto Result = ExportTrieExportCollection(ImageAddress);
        auto NodeCreator =
            ExportTrieEntryCollectionNodeCreator(SegList, ImageAddress);

        Result.Parse(Trie, NodeCreator, Options, ErrorOut);
        return Result;
    }
}