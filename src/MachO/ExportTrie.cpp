/*
 * MachO/ExportTrie.cpp
 * © suhas pai
 */

#include "MachO/ExportTrie.h"
#include "Utils/Leb128.h"

namespace MachO {
    auto
    ExportTrieExportInfo::ParseExportData(
        uint8_t *& Ptr,
        uint8_t *const NodeEnd,
        [[maybe_unused]] uint8_t *const TrieEnd,
        [[maybe_unused]] std::string_view String,
        [[maybe_unused]] std::vector<ADT::Range> &RangeList,
        [[maybe_unused]] std::vector<ADT::TrieStackInfo> &StackInfo) noexcept
            -> ADT::TrieParseError
    {
        using Error = ADT::TrieParseError;

        const auto Flags = Utils::ReadUleb128(Ptr, NodeEnd, &Ptr);
        if (Ptr == nullptr) {
            return Error::InvalidUleb128;
        }

        if (Ptr == NodeEnd) {
            return Error::InvalidFormat;
        }

        setFlags(Flags);
        if (isReexport()) {
            const auto DylibOrdinal =
                Utils::ReadUleb128<uint32_t>(Ptr, NodeEnd, &Ptr);

            if (Ptr == nullptr) {
                return Error::InvalidUleb128;
            }

            if (Ptr == NodeEnd) {
                return Error::InvalidFormat;
            }

            setReexportDylibOrdinal(DylibOrdinal);
            if (*Ptr != '\0') {
                const auto String = reinterpret_cast<const char *>(Ptr);
                const auto MaxLength = static_cast<uint64_t>(NodeEnd - Ptr);
                const auto Length = strnlen(String, MaxLength);

                if (Length == MaxLength) {
                    return Error::InvalidFormat;
                }

                auto ImportName = std::string(String, Length);
                setReexportImportName(std::move(ImportName));

                const auto StringSize = Length + 1;
                Ptr += StringSize;
            } else {
                Ptr += 1;
            }
        } else {
            const auto ImageOffset = Utils::ReadUleb128(Ptr, NodeEnd, &Ptr);
            if (Ptr == nullptr) {
                return Error::InvalidUleb128;
            }

            setImageOffset(ImageOffset);
            if (stubAndResolver()) {
                if (Ptr == NodeEnd) {
                    return Error::InvalidFormat;
                }

                const auto ResolverStubAddress =
                    Utils::ReadUleb128(Ptr, NodeEnd, &Ptr);

                if (Ptr == nullptr) {
                    return Error::InvalidUleb128;
                }

                setResolverStubAddress(ResolverStubAddress);
            }
        }

        if (Ptr != NodeEnd) {
            return Error::InvalidUleb128;
        }

        return Error::None;
    }

    const SegmentInfo *
    ExportTrieEntryCollection::LookupInfoForAddress(
        const SegmentList &SegList,
        [[maybe_unused]] const ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
    {
        const auto Segment = SegList.findSegmentWithVmAddr(Address);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(Address);
        }

        return Segment;
    }

    ExportTrieEntryCollection::ChildNode *
    ExportTrieEntryCollection::MakeNodeForEntryInfo(
        const ExportTrieMap::IterateInfo &Info,
        const SegmentList *const SegList) noexcept
    {
        auto Node = static_cast<ChildNode *>(nullptr);
        if (Info.isExport()) {
            Node = new ExportChildNode();
            Node->setKind(
                MachO::ExportTrieExportKindFromFlags(
                    Info.exportInfo().flags()));
        } else {
            Node = new ChildNode();
        }

        Node->setString(std::string(Info.string()));
        if (Info.isExport()) {
            auto ExportNode = Node->getAsExportNode();
            const auto &ExportInfo = Info.exportInfo();

            if (!ExportInfo.isReexport()) {
                if (SegList != nullptr) {
                    auto Section = static_cast<const SectionInfo *>(nullptr);

                    const auto Addr = ExportInfo.imageOffset();
                    const auto Segment =
                        LookupInfoForAddress(*SegList,
                                             ExportInfo.kind(),
                                             Addr,
                                             &Section);

                    ExportNode->setSegment(Segment);
                    ExportNode->setSection(Section);
                }
            }

            ExportNode->setInfo(ExportInfo);
        }

        return Node;
    }

    void
    ExportTrieEntryCollection::ParseFromTrie(const ExportTrieMap &Trie,
                                             const SegmentList *const SegList,
                                             Error *const Error) noexcept
    {
        auto Iter = Trie.begin();
        const auto End = Trie.end();

        if (Iter == End) {
            return;
        }

        setRoot(MakeNodeForEntryInfo(*Iter, SegList));

        auto Parent = root();
        auto PrevDepthLevel = uint64_t(1);

        const auto MoveUpParentHierarchy = [&](const uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                Parent = Parent->parent();
            }
        };

        for (Iter++; Iter != End; Iter++) {
            if (Iter.hasError()) {
                if (Error != nullptr) {
                    *Error = Iter.getError();
                }

                return;
            }

            const auto DepthLevel = Iter->depthLevel();
            if (PrevDepthLevel > DepthLevel) {
                MoveUpParentHierarchy(
                    static_cast<uint64_t>(PrevDepthLevel) - DepthLevel);
            }

            const auto Current = MakeNodeForEntryInfo(*Iter, SegList);

            Parent->addChild(*Current);
            if (Iter->node().childCount() != 0) {
                Parent = Current;
            }

            PrevDepthLevel = DepthLevel;
        }
    }

    ExportTrieEntryCollection
    ExportTrieEntryCollection::Open(const ExportTrieMap &Trie,
                                    const SegmentList *const SegList,
                                    Error *const ErrorOut)
    {
        auto Result = ExportTrieEntryCollection();
        Result.ParseFromTrie(Trie, SegList, ErrorOut);

        return Result;
    }

    auto
    ExportTrieExportCollection::LookupInfoForAddress(
        const SegmentList &SegList,
        [[maybe_unused]] const ExportTrieFlags::Kind Kind,
        const uint64_t Address,
        const SectionInfo **const SectionOut) const noexcept
        -> const SegmentInfo *
    {
        const auto Segment = SegList.findSegmentWithVmAddr(Address);
        if (Segment != nullptr) {
            *SectionOut = Segment->findSectionWithVmAddr(Address);
        }

        return Segment;
    }

    ExportTrieExportCollection
    ExportTrieExportCollection::Open(const ExportTrieMap::ExportMap &Trie,
                                     const SegmentList *const SegList,
                                     Error *const ErrorOut) noexcept
    {
        auto Result = ExportTrieExportCollection();
        Result.Parse(Trie, SegList, ErrorOut);

        return Result;
    }

    void
    ExportTrieExportCollection::Parse(const ExportTrieMap::ExportMap &Trie,
                                      const SegmentList *const SegList,
                                      Error *const ErrorOut) noexcept
    {
        for (auto Iter = Trie.begin(); Iter != Trie.end(); Iter++) {
            if (Iter.hasError()) {
                *ErrorOut = Iter.getError();
                return;
            }

            auto Entry = EntryInfo();
            const auto &ExportInfo = Iter->exportInfo();

            if (!ExportInfo.isReexport()) {
                if (SegList != nullptr) {
                    const auto Addr = ExportInfo.imageOffset();
                    auto Section = static_cast<const SectionInfo *>(nullptr);

                    Entry.setSegment(
                        LookupInfoForAddress(*SegList,
                                             ExportInfo.kind(),
                                             Addr,
                                             &Section));

                    Entry.setSection(Section);
                }
            }

            EntryList.emplace_back(std::make_unique<EntryInfo>(Entry));
        }
    }
}