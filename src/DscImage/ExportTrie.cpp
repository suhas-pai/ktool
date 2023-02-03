/*
 * DscImage/ExportTrie.cpp
 * © suhas pai
 */

#include "DscImage/ExportTrie.h"
#include "MachO/ExportTrie.h"
#include "Utils/Overflow.h"

namespace DscImage {
    ExportTrieEntryCollection::ExportTrieEntryCollection(
        const uint64_t ImageAddress) noexcept
    : MachO::ExportTrieEntryCollection(), ImageAddress(ImageAddress) {}

    auto
    ExportTrieEntryCollection::Open(const MachO::ExportTrieMap &Trie,
                                    const MachO::SegmentList *SegList,
                                    uint64_t ImageAddress,
                                    Error *ErrorOut)
        -> ExportTrieEntryCollection
    {
        auto Result = ExportTrieEntryCollection(ImageAddress);
        Result.ParseFromTrie(Trie, SegList, ErrorOut);

        return Result;
    }

    const MachO::SegmentInfo *
    ExportTrieEntryCollection::LookupInfoForAddress(
        const MachO::SegmentList &SegList,
        const MachO::ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const MachO::SectionInfo **const SectionOut) const noexcept
    {
        const uint64_t FullAddress =
            Kind == MachO::ExportTrieFlags::Kind::Absolute ?
                Address :
                Utils::AddAndCheckOverflow(ImageAddress, Address).value();

        const auto Segment = SegList.findSegmentWithVmAddr(FullAddress);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(FullAddress);
        }

        return Segment;
    }

    ExportTrieExportCollection::ExportTrieExportCollection(
        const uint64_t ImageAddress) noexcept
    : MachO::ExportTrieExportCollection(), ImageAddress(ImageAddress) {}

    const MachO::SegmentInfo *
    ExportTrieExportCollection::LookupInfoForAddress(
        const MachO::SegmentList &SegList,
        const MachO::ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const MachO::SectionInfo **const SectionOut) const noexcept
    {
        const uint64_t FullAddress =
            Kind == MachO::ExportTrieFlags::Kind::Absolute ?
                Address :
                Utils::AddAndCheckOverflow(ImageAddress, Address).value();

        const auto Segment = SegList.findSegmentWithVmAddr(FullAddress);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(FullAddress);
        }

        return Segment;
    }

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(
        const MachO::ExportTrieMap::ExportMap &Trie,
        const MachO::SegmentList *SegList,
        uint64_t ImageAddress,
        Error *const ErrorOut) noexcept
    {
        auto Result = ExportTrieExportCollection(ImageAddress);
        Result.Parse(Trie, SegList, ErrorOut);

        return Result;
    }
}