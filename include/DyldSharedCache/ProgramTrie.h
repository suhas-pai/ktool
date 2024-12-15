/*
 * DyldSharedCache/ProgramTrie.h
 * © suhas pai
 */

#pragma once

#include "ADT/MemoryMap.h"
#include "ADT/Trie.h"

#include "Utils/Overflow.h"

namespace DyldSharedCache {
    struct DylibIndexInfo {
    protected:
        uint32_t Index = 0;
    public:
        explicit DylibIndexInfo() noexcept = default;

        [[nodiscard]] constexpr auto index() const noexcept {
            return Index;
        }

        constexpr auto setIndex(const uint32_t Index) noexcept
            -> decltype(*this)
        {
            this->Index = Index;
            return *this;
        }

        auto
        ParseExportData(uint8_t *& Ptr,
                        uint8_t *NodeEnd,
                        uint8_t *TrieEnd,
                        std::string_view String,
                        std::vector<ADT::Range> &RangeList,
                        std::vector<ADT::TrieStackInfo> &StackInfo) noexcept
            -> ADT::TrieParseError;

        constexpr auto clearExclusiveInfo() noexcept -> decltype(*this) {
            Index = 0;
            return *this;
        }
    };

    struct ProgramTrieMap : public ADT::Trie<DylibIndexInfo> {
    protected:
        DylibIndexInfo Info;
    public:
        explicit
        ProgramTrieMap(uint8_t *const Begin,
                       uint8_t *const End,
                       ADT::TrieParser &TrieParser) noexcept
        : ADT::Trie<DylibIndexInfo>(Begin, End, TrieParser, Info) {}

        explicit
        ProgramTrieMap(const ADT::MemoryMap &Map,
                       ADT::TrieParser &TrieParser) noexcept
        : ADT::Trie<DylibIndexInfo>(Map, TrieParser, Info) {}

        [[nodiscard]] constexpr auto &info() const noexcept {
            return Info;
        }

        [[nodiscard]] constexpr auto &info() noexcept {
            return Info;
        }
    };

    struct ProgramTrieExportChildNode;
    struct ProgramTrieChildNode : public ADT::TreeNode {
    protected:
        std::string String;

        // Store Ordinal instead of index. Ordinal == 0 -> Not export
        uint32_t Ordinal;
    public:
        ~ProgramTrieChildNode() noexcept = default;
        using TreeNode::TreeNode;

        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ProgramTrieChildNode;
        }

        [[nodiscard]] static inline auto &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ProgramTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline auto &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ProgramTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline auto get(TreeNode *const Node) noexcept {
            return reinterpret_cast<ProgramTrieChildNode *>(Node);
        }

        [[nodiscard]]
        static inline auto get(const TreeNode *const Node) noexcept {
            return reinterpret_cast<const ProgramTrieChildNode *>(Node);
        }

        [[nodiscard]] inline auto parent() const noexcept {
            return get(Parent);
        }

        [[nodiscard]] inline auto prevSibling() const noexcept {
            return get(PrevSibling);
        }

        [[nodiscard]] inline auto nextSibling() const noexcept {
            return get(NextSibling);
        }

        [[nodiscard]] inline auto firstChild() const noexcept {
            return get(FirstChild);
        }

        [[nodiscard]] inline auto lastChild() const noexcept {
            return get(LastChild);
        }

        [[nodiscard]]
        constexpr auto string() const noexcept -> std::string_view {
            return this->String;
        }

        [[nodiscard]] inline uint64_t getLength() const noexcept override {
            return this->string().length();
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return this->Ordinal != 0;
        }

        [[nodiscard]]
        inline auto getIfExportNode() noexcept -> ProgramTrieExportChildNode * {
            if (this->isExport()) {
                return reinterpret_cast<ProgramTrieExportChildNode *>(this);
            }

            return nullptr;
        }

        [[nodiscard]]
        inline auto getIfExportNode() const noexcept
            -> const ProgramTrieExportChildNode *
        {
            if (!this->isExport()) {
                return nullptr;
            }

            return reinterpret_cast<const ProgramTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() noexcept {
            assert(this->isExport());
            return reinterpret_cast<ProgramTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() const noexcept {
            assert(this->isExport());
            return reinterpret_cast<const ProgramTrieExportChildNode *>(this);
        }

        constexpr auto setString(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
        }
    };

    struct ProgramTrieExportChildNode : public ProgramTrieChildNode {
        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ProgramTrieExportChildNode;
        }

        [[nodiscard]] auto index() const noexcept {
            assert(this->Ordinal != 0);
            return this->Ordinal - 1;
        }

        constexpr auto setIndex(const uint32_t Index) noexcept
            -> decltype(*this)
        {
            this->Ordinal =
                Utils::AddAndCheckOverflow<uint32_t>(Index, 1).value();

            return *this;
        }
    };

    struct ProgramTrieEntryCollectionNodeCreator {
        using ChildNode = ProgramTrieChildNode;
        using ExportChildNode = ProgramTrieExportChildNode;
        using ParseError = ADT::TrieParseError;

        explicit ProgramTrieEntryCollectionNodeCreator() noexcept = default;

        virtual auto
        createChildNode(ProgramTrieMap::IterateInfo &Info) const noexcept
            -> ChildNode *;
    };

    struct ProgramTrieEntryCollection :
        public ADT::TrieNodeCollection<DylibIndexInfo,
                                       ProgramTrieEntryCollectionNodeCreator>
    {
    public:
        using NodeCreator = ProgramTrieEntryCollectionNodeCreator;
        using Base = ADT::TrieNodeCollection<DylibIndexInfo, NodeCreator>;
        using ChildNode = ProgramTrieChildNode;
        using ProgramChildNode = ProgramTrieChildNode;
        using ParseError = ADT::TrieParseError;
    protected:
        ProgramTrieChildNode *Root;
    public:
        explicit ProgramTrieEntryCollection() noexcept = default;

        static auto
        Open(ProgramTrieMap &Trie,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ProgramTrieEntryCollection;

        static auto
        Open(ProgramTrieMap &Trie,
             ProgramTrieEntryCollectionNodeCreator &Creator,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ProgramTrieEntryCollection;

        virtual void
        ParseFromTrie(ProgramTrieMap &Trie,
                      const ParseOptions &Options = ParseOptions(),
                      Error *ErrorOut = nullptr) noexcept;

        [[nodiscard]] inline ADT::TreeNode *root() const noexcept override {
            return this->Root;
        }

        inline ADT::Tree &setRoot(ADT::TreeNode *const Root) noexcept override {
            this->Root = static_cast<ProgramTrieChildNode *>(Root);
            return *this;
        }

        inline
        auto RemoveNode(ChildNode &Ref, const bool RemoveParentLeafs) noexcept {
            auto &Node = reinterpret_cast<ADT::TreeNode &>(Ref);
            const auto Result = Tree::removeNode(Node, RemoveParentLeafs);

            return reinterpret_cast<ChildNode *>(Result);
        }

        using Iterator = ADT::TreeDFSIterator<ChildNode>;
        using ConstIterator = ADT::TreeDFSIterator<const ChildNode>;

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Root);
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Iterator(nullptr, nullptr);
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return ConstIterator(this->Root);
        }

        [[nodiscard]] constexpr auto cend() const noexcept {
            return ConstIterator(nullptr, nullptr);
        }

        template <typename T>
        inline auto forEach(const T &Callback) noexcept -> decltype(*this) {
            forEachNode(Callback);
            return *this;
        }

        template <typename T>
        inline auto forEach(const T &Callback) const noexcept -> decltype(*this)
        {
            forEachNode(Callback);
            return *this;
        }
    };

    struct ProgramTrieExportCollection {
    public:
        using EntryInfo = DylibIndexInfo;
        using NodeCreator = ProgramTrieEntryCollectionNodeCreator;
        using Error = ADT::TrieParseError;
        using ParseOptions = ADT::TrieParseOptions;
        using EntryListType = std::vector<std::unique_ptr<EntryInfo>>;
    protected:
        EntryListType EntryList;
        explicit ProgramTrieExportCollection() noexcept = default;

        void
        Parse(const ProgramTrieMap::ExportMap &Trie,
              const ParseOptions &Options,
              Error *const ErrorOut) noexcept;
    public:
        static auto
        Open(const ProgramTrieMap::ExportMap &Trie,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept
             -> ProgramTrieExportCollection;

        using Iterator = EntryListType::iterator;
        using ConstIterator = EntryListType::const_iterator;

        [[nodiscard]] inline auto begin() noexcept {
            return EntryList.begin();
        }

        [[nodiscard]] inline auto end() noexcept {
            return EntryList.end();
        }

        [[nodiscard]] inline auto begin() const noexcept {
            return EntryList.cbegin();
        }

        [[nodiscard]] inline auto end() const noexcept {
            return EntryList.cend();
        }
    };
}
