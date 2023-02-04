/*
 * DyldSharedCache/ProgramTrie.cpp
 * © suhas pai
 */

#include "DyldSharedCache/ProgramTrie.h"
#include "Utils/Leb128.h"

namespace DyldSharedCache {
    auto
    DylibIndexInfo::ParseExportData(
        uint8_t *& Ptr,
        uint8_t *const NodeEnd,
        [[maybe_unused]] uint8_t *const TrieEnd,
        [[maybe_unused]] const std::string_view String,
        [[maybe_unused]] std::vector<ADT::Range> &RangeList,
        [[maybe_unused]] std::vector<ADT::TrieStackInfo> &StackInfo) noexcept
            -> ADT::TrieParseError
    {
        if (const auto IndexOpt = Utils::ReadUleb128(Ptr, NodeEnd, &Ptr)) {
            Index = IndexOpt.value();
            return ADT::TrieParseError::None;
        }

        return ADT::TrieParseError::InvalidUleb128;
    }

    auto
    ProgramTrieEntryCollectionNodeCreator::createChildNode(
        ProgramTrieMap::IterateInfo &Info) const noexcept -> ChildNode *
    {
        auto Node = static_cast<ChildNode *>(nullptr);
        if (Info.isExport()) {
            Node = new ExportChildNode();
            static_cast<ExportChildNode *>(Node)->setIndex(
                Info.exportInfo().index());
        } else {
            Node = new ChildNode();
        }

        Node->setString(std::string(Info.string()));
        return Node;
    }

    auto
    ProgramTrieEntryCollection::Open(ProgramTrieMap &Trie,
                                     const ParseOptions &Options,
                                     Error *const ErrorOut) noexcept
        -> ProgramTrieEntryCollection
    {
        auto Result = ProgramTrieEntryCollection();
        Result.ParseFromTrie(Trie, Options, ErrorOut);

        return Result;
    }

    auto
    ProgramTrieEntryCollection::Open(
        ProgramTrieMap &Trie,
        ProgramTrieEntryCollectionNodeCreator &Creator,
        const ParseOptions &Options,
        Error *const ErrorOut) noexcept
        -> ProgramTrieEntryCollection
    {
        auto Result = ProgramTrieEntryCollection();
        Result.Base::ParseFromTrie(Trie, Creator, Options, ErrorOut);

        return Result;
    }

    void
    ProgramTrieEntryCollection::ParseFromTrie(ProgramTrieMap &Trie,
                                              const ParseOptions &Options,
                                              Error *const ErrorOut) noexcept
    {
        auto NodeCreator = ProgramTrieEntryCollection::NodeCreator();
        Base::ParseFromTrie(Trie, NodeCreator, Options, ErrorOut);
    }

    ProgramTrieExportCollection
    ProgramTrieExportCollection::Open(const ProgramTrieMap::ExportMap &Trie,
                                      const ParseOptions &Options,
                                      Error *const ErrorOut) noexcept
    {
        auto Result = ProgramTrieExportCollection();
        Result.Parse(Trie, Options, ErrorOut);

        return Result;
    }

    void
    ProgramTrieExportCollection::Parse(const ProgramTrieMap::ExportMap &Trie,
                                       const ParseOptions &Options,
                                       Error *const ErrorOut) noexcept
    {
        for (auto Iter = Trie.begin(Options); Iter != Trie.end(); Iter++) {
            if (Iter.hasError()) {
                *ErrorOut = Iter.getError();
                return;
            }

            const auto &ExportInfo = Iter->exportInfo();
            EntryList.emplace_back(std::make_unique<EntryInfo>(ExportInfo));
        }
    }
}