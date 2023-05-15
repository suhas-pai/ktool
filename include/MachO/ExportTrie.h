/*
 * MachO/ExportTrie.h
 * © suhas pai
 */

#pragma once
#include <cassert>
#include <vector>

#include "ADT/Trie.h"
#include "MachO/SegmentList.h"

namespace MachO {
    struct ExportTrieFlags : ADT::FlagsBase<uint8_t> {
    public:
        using ADT::FlagsBase<uint8_t>::FlagsBase;

        enum class Kind : uint8_t {
            Regular,
            ThreadLocal,
            Absolute
        };

        [[nodiscard]]
        constexpr static auto KindIsValid(const Kind Kind) noexcept {
            switch (Kind) {
                case Kind::Regular:
                case Kind::ThreadLocal:
                case Kind::Absolute:
                    return true;
            }

            return false;
        }

        [[nodiscard]]
        constexpr static auto KindGetString(const Kind Kind) noexcept
            -> std::string_view
        {
            switch (Kind) {
                case Kind::Regular:
                    return "EXPORT_SYMBOL_FLAGS_KIND_REGULAR";
                case Kind::ThreadLocal:
                    return "EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL";
                case Kind::Absolute:
                    return "EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE";
            }

            assert(false &&
                   "MachO::ExportTrieFlags::KindGetString() got unrecognized "
                   "Kind");
        }

        [[nodiscard]]
        constexpr static auto KindGetDesc(const Kind Kind) noexcept
            -> std::string_view
        {
            switch (Kind) {
                case Kind::Regular:
                    return "Regular";
                case Kind::ThreadLocal:
                    return "Thread-Local";
                case Kind::Absolute:
                    return "Absolute";
            }

            assert(false &&
                   "MachO::ExportTrieFlags::KindGetDesc() got unrecognized "
                   "Kind");
        }

        [[nodiscard]] constexpr
        static auto KindGetFromString(const std::string_view S) noexcept
            -> std::optional<Kind>
        {
            const Kind Kind = Kind::Regular;
            switch (Kind) {
                STR_TO_ENUM_SWITCH_CASE(Kind::Regular, S, KindGetString);
                STR_TO_ENUM_SWITCH_CASE(Kind::ThreadLocal, S, KindGetString);
                STR_TO_ENUM_LAST_SWITCH_CASE(Kind::Absolute, S, KindGetString);
            }

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr static auto KindGetFromDesc(const std::string_view D) noexcept
            -> std::optional<Kind>
        {
            const Kind Kind = Kind::Regular;
            switch (Kind) {
                STR_TO_ENUM_SWITCH_CASE(Kind::Regular, D, KindGetDesc);
                STR_TO_ENUM_SWITCH_CASE(Kind::ThreadLocal, D, KindGetDesc);
                STR_TO_ENUM_LAST_SWITCH_CASE(Kind::Absolute, D, KindGetDesc);
            }

            return std::nullopt;
        }

        enum class Masks : uint8_t {
            Kind            = 0b11,
            WeakDefinition  = 0b100,
            Reexport        = 0b1000,
            StubAndResolver = 0b10000
        };

        [[nodiscard]] constexpr auto kind() const noexcept {
            return Kind(valueForMask(Masks::Kind));
        }

        [[nodiscard]] constexpr auto regular() const noexcept {
            return kind() == Kind::Regular;
        }

        [[nodiscard]] constexpr auto absolute() const noexcept {
            return kind() == Kind::Absolute;
        }

        [[nodiscard]] constexpr auto threadLocal() const noexcept {
            return kind() == Kind::ThreadLocal;
        }

        [[nodiscard]] constexpr auto isWeak() const noexcept {
            return has(Masks::WeakDefinition);
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return has(Masks::Reexport);
        }

        [[nodiscard]] constexpr auto stubAndResolver() const noexcept {
            return has(Masks::StubAndResolver);
        }

        constexpr auto setKind(const Kind Kind) noexcept -> decltype(*this) {
            setValueForMask(Masks::Kind, 0, Kind);
            return *this;
        }

        constexpr auto setWeak(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::WeakDefinition, 0, Value);
            return *this;
        }

        constexpr auto setReexport(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::Reexport, 0, Value);
            return *this;
        }

        constexpr auto setStubAndResolver(const bool Value = true) noexcept
            -> decltype(*this)
        {
            setValueForMask(Masks::StubAndResolver, 0, Value);
            return *this;
        }
    };

    enum class ExportTrieExportKind {
        None,
        Regular,
        Absolute,
        Reexport,
        WeakDefinition,
        StubAndResolver,
        ThreadLocal
    };

    [[nodiscard]] constexpr
    auto ExportTrieExportKindIsValid(const ExportTrieExportKind Kind) noexcept {
        switch (Kind) {
            case ExportTrieExportKind::None:
            case ExportTrieExportKind::Regular:
            case ExportTrieExportKind::Absolute:
            case ExportTrieExportKind::Reexport:
            case ExportTrieExportKind::WeakDefinition:
            case ExportTrieExportKind::StubAndResolver:
            case ExportTrieExportKind::ThreadLocal:
                return true;
        }

        return false;
    }

    [[nodiscard]] constexpr
    auto ExportTrieExportKindFromFlags(const ExportTrieFlags &Flags) noexcept {
        switch (Flags.kind()) {
            case ExportTrieFlags::Kind::Regular:
                return ExportTrieExportKind::Regular;
            case ExportTrieFlags::Kind::ThreadLocal:
                return ExportTrieExportKind::ThreadLocal;
            case ExportTrieFlags::Kind::Absolute:
                return ExportTrieExportKind::Absolute;
        }

        if (Flags.isReexport()) {
            return ExportTrieExportKind::Reexport;
        }

        if (Flags.isWeak()) {
            return ExportTrieExportKind::WeakDefinition;
        }

        if (Flags.stubAndResolver()) {
            return ExportTrieExportKind::StubAndResolver;
        }

        return ExportTrieExportKind::None;
    }


    [[nodiscard]] constexpr
    auto ExportTrieExportKindGetString(const ExportTrieExportKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case ExportTrieExportKind::None:
                assert(false &&
                       "MachO::ExportTrieExportKindGetString() got None");
            case ExportTrieExportKind::Regular:
                return "EXPORT_SYMBOL_FLAGS_KIND_REGULAR";
            case ExportTrieExportKind::Absolute:
                return "EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE";
            case ExportTrieExportKind::Reexport:
                return "EXPORT_SYMBOL_FLAGS_REEXPORT";
            case ExportTrieExportKind::WeakDefinition:
                return "EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION";
            case ExportTrieExportKind::StubAndResolver:
                return "EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER";
            case ExportTrieExportKind::ThreadLocal:
                return "EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL";
        }

        assert(false &&
               "MachO::ExportTrieExportKindGetString() got unrecognized "
               "ExportTrieExportKind");
    }

    [[nodiscard]] constexpr
    auto ExportTrieExportKindGetDesc(const ExportTrieExportKind Kind) noexcept
        -> std::string_view
    {
        switch (Kind) {
            case ExportTrieExportKind::None:
                assert(false &&
                       "MachO::ExportTrieExportKindGetDesc() got None");
            case ExportTrieExportKind::Regular:
                return "Regular";
            case ExportTrieExportKind::Absolute:
                return "Absolute";
            case ExportTrieExportKind::Reexport:
                return "Re-export";
            case ExportTrieExportKind::WeakDefinition:
                return "Weak-Def";
            case ExportTrieExportKind::StubAndResolver:
                return "Stub-and-Resolver";
            case ExportTrieExportKind::ThreadLocal:
                return "Thread-Local";
        }

        assert(false &&
               "MachO::ExportTrieExportKindGetDesc() got unrecognized "
               "ExportTrieExportKind");
    }

    [[nodiscard]] constexpr
    auto ExportTrieExportKindGetFromString(const std::string_view S) noexcept
        -> std::optional<ExportTrieExportKind>
    {
        using ExportKind = ExportTrieExportKind;
        const ExportKind Kind = ExportKind::Regular;

        switch (Kind) {
            case ExportTrieExportKind::None:
                [[fallthrough]];
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Regular,
                                    S,
                                    ExportTrieExportKindGetString);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Absolute,
                                    S,
                                    ExportTrieExportKindGetString);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Reexport,
                                    S,
                                    ExportTrieExportKindGetString);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::WeakDefinition,
                                    S,
                                    ExportTrieExportKindGetString);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::StubAndResolver,
                                    S,
                                    ExportTrieExportKindGetString);
            STR_TO_ENUM_LAST_SWITCH_CASE(ExportKind::ThreadLocal,
                                         S,
                                         ExportTrieExportKindGetString);
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr
    auto ExportTrieExportKindGetFromDesc(const std::string_view D) noexcept
        -> std::optional<ExportTrieExportKind>
    {
        using ExportKind = ExportTrieExportKind;
        const ExportKind Kind = ExportKind::Regular;

        switch (Kind) {
            case ExportTrieExportKind::None:
                [[fallthrough]];
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Regular,
                                    D,
                                    ExportTrieExportKindGetDesc);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Absolute,
                                    D,
                                    ExportTrieExportKindGetDesc);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::Reexport,
                                    D,
                                    ExportTrieExportKindGetDesc);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::WeakDefinition,
                                    D,
                                    ExportTrieExportKindGetDesc);
            STR_TO_ENUM_SWITCH_CASE(ExportKind::StubAndResolver,
                                    D,
                                    ExportTrieExportKindGetDesc);
            STR_TO_ENUM_LAST_SWITCH_CASE(ExportKind::ThreadLocal,
                                         D,
                                         ExportTrieExportKindGetDesc);
        }

        return std::nullopt;
    }

    struct ExportTrieExportInfo {
    public:
        using Kind = ExportTrieExportKind;
    protected:
        std::string ReexportImportName;

        union L {
            uint32_t ReexportDylibOrdinal;

            struct LocInfo {
                uint64_t ResolverStubAddress;
                uint64_t ImageOffset;
            } Loc;
        } Info;

        ExportTrieFlags Flags;
    public:
        [[nodiscard]] constexpr auto flags() const noexcept {
            return Flags;
        }

        [[nodiscard]] constexpr auto kind() const noexcept {
            return flags().kind();
        }

        [[nodiscard]] constexpr auto regular() const noexcept {
            return flags().regular();
        }

        [[nodiscard]] constexpr auto absolute() const noexcept {
            return flags().absolute();
        }

        [[nodiscard]] constexpr auto threadLocal() const noexcept {
            return flags().threadLocal();
        }

        [[nodiscard]] constexpr auto weak() const noexcept {
            return flags().isWeak();
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return flags().isReexport();
        }

        [[nodiscard]] constexpr auto stubAndResolver() const noexcept {
            return flags().stubAndResolver();
        }

        [[nodiscard]]
        constexpr auto reexportImportName() const noexcept -> std::string_view {
            assert(isReexport());
            return ReexportImportName;
        }

        [[nodiscard]] constexpr auto reexportDylibOrdinal() const noexcept {
            assert(isReexport());
            return Info.ReexportDylibOrdinal;
        }

        [[nodiscard]] constexpr auto resolverStubAddress() const noexcept {
            assert(stubAndResolver());
            return Info.Loc.ResolverStubAddress;
        }

        [[nodiscard]] constexpr auto imageOffset() const noexcept {
            assert(!isReexport());
            return Info.Loc.ImageOffset;
        }

        constexpr
        auto setReexportImportName(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->ReexportImportName = Value;
            return *this;
        }

        constexpr auto setReexportImportName(std::string &&Value) noexcept
            -> decltype(*this)
        {
            this->ReexportImportName = Value;
            return *this;
        }

        constexpr auto setFlags(const ExportTrieFlags &Flags) noexcept
            -> decltype(*this)
        {
            this->Flags = Flags;
            return *this;
        }

        constexpr auto setReexportDylibOrdinal(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            assert(isReexport());

            this->Info.ReexportDylibOrdinal = Value;
            return *this;
        }

        constexpr auto setResolverStubAddress(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            assert(stubAndResolver());

            this->Info.Loc.ResolverStubAddress = Value;
            return *this;
        }

        constexpr auto setImageOffset(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Info.Loc.ImageOffset = Value;
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
            Info.Loc.ImageOffset = 0;
            Info.ReexportDylibOrdinal = 0;
            Info.Loc.ResolverStubAddress = 0;

            Flags.clear();
            return *this;
        }
    };

    struct ExportTrieMap : public ADT::Trie<ExportTrieExportInfo> {
    protected:
        ExportTrieExportInfo ExportInfo;
    public:
        explicit
        ExportTrieMap(uint8_t *const Begin,
                      uint8_t *const End,
                      ADT::TrieParser &TrieParser) noexcept
        : ADT::Trie<ExportTrieExportInfo>(Begin, End, TrieParser, ExportInfo) {}

        explicit
        ExportTrieMap(const ADT::MemoryMap &Map,
                      ADT::TrieParser &TrieParser) noexcept
        : ADT::Trie<ExportTrieExportInfo>(Map, TrieParser, ExportInfo) {}

        [[nodiscard]] constexpr auto &exportInfo() const noexcept {
            return ExportInfo;
        }
    };

    struct ExportTrieExportChildNode;
    struct ExportTrieChildNode : public ADT::TreeNode {
    protected:
        ExportTrieExportKind Kind;
        std::string String;
    public:
        ~ExportTrieChildNode() noexcept = default;
        using TreeNode::TreeNode;

        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ExportTrieChildNode;
        }

        [[nodiscard]] static inline auto &get(TreeNode &Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline auto &get(const TreeNode &Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode &>(Node);
        }

        [[nodiscard]] static inline auto get(TreeNode *Node) noexcept {
            return reinterpret_cast<ExportTrieChildNode *>(Node);
        }

        [[nodiscard]]
        static inline auto get(const TreeNode *const Node) noexcept {
            return reinterpret_cast<const ExportTrieChildNode *>(Node);
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

        [[nodiscard]] inline uint64_t getLength() const noexcept override {
            return String.length();
        }

        [[nodiscard]] constexpr auto kind() const noexcept {
            return Kind;
        }

        [[nodiscard]]
        constexpr auto string() const noexcept -> std::string_view {
            return String;
        }

        constexpr auto setKind(const ExportTrieExportKind Kind) noexcept
            -> decltype(*this)
        {
            this->Kind = Kind;
            return *this;
        }

        constexpr auto setString(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return kind() != ExportTrieExportKind::None;
        }

        [[nodiscard]] constexpr auto isReexport() const noexcept {
            return kind() == ExportTrieExportKind::Reexport;
        }

        [[nodiscard]]
        inline auto getIfExportNode() noexcept -> ExportTrieExportChildNode * {
            if (this->isExport()) {
                return reinterpret_cast<ExportTrieExportChildNode *>(this);
            }

            return nullptr;
        }

        [[nodiscard]]
        inline auto getIfExportNode() const noexcept
            -> const ExportTrieExportChildNode *
        {
            if (!this->isExport()) {
                return nullptr;
            }

            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() noexcept {
            assert(this->isExport());
            return reinterpret_cast<ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() const noexcept {
            assert(this->isExport());
            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        inline auto setParent(ExportTrieChildNode *const Parent) noexcept
            -> decltype(*this)
        {
            this->Parent = Parent;
            return *this;
        }
    };

    struct ExportTrieExportChildNode : public ExportTrieChildNode {
    protected:
        ExportTrieExportInfo Info;

        const MachO::SegmentInfo *Segment = nullptr;
        const MachO::SectionInfo *Section = nullptr;
    public:
        using ExportTrieChildNode::ExportTrieChildNode;

        [[nodiscard]] constexpr const auto &info() const noexcept {
            return Info;
        }

        constexpr auto &setInfo(const ExportTrieExportInfo &Value) noexcept {
            this->Info = Value;
            return *this;
        }

        [[nodiscard]] inline TreeNode *createNew() const noexcept override {
            return new ExportTrieExportChildNode;
        }

        [[nodiscard]] inline auto segment() const noexcept {
            assert(!this->isReexport());
            return Segment;
        }

        [[nodiscard]] inline auto section() const noexcept {
            assert(!this->isReexport());
            return Section;
        }

        constexpr auto setSegment(const SegmentInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->isReexport());

            this->Segment = Value;
            return *this;
        }

        constexpr auto setSection(const SectionInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->isReexport());

            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieEntryCollectionNodeCreator {
    protected:
        const MachO::SegmentList *SegList = nullptr;
    public:
        using ChildNode = ExportTrieChildNode;
        using ExportChildNode = ExportTrieExportChildNode;
        using Error = ADT::TrieParseError;

        explicit
        ExportTrieEntryCollectionNodeCreator(
            const MachO::SegmentList *const SegList) noexcept
        : SegList(SegList) {}

        [[nodiscard]] virtual auto
        LookupInfoForAddress(ExportTrieFlags::Kind Kind,
                             uint64_t Address,
                             const SectionInfo **SectionOut) const noexcept
            -> const SegmentInfo *;

        virtual auto
        createChildNode(ExportTrieMap::IterateInfo &Info) const noexcept
            -> ChildNode *;
    };

    struct ExportTrieEntryCollection :
        public ADT::TrieNodeCollection<ExportTrieExportInfo,
                                       ExportTrieEntryCollectionNodeCreator>
    {
    public:
        using NodeCreator = ExportTrieEntryCollectionNodeCreator;
        using Base = ADT::TrieNodeCollection<ExportTrieExportInfo, NodeCreator>;
        using ChildNode = ExportTrieChildNode;
        using ExportChildNode = ExportTrieExportChildNode;
        using Error = ADT::TrieParseError;
    protected:
        ExportTrieChildNode *Root;
    public:
        explicit ExportTrieEntryCollection() noexcept = default;

        static auto
        Open(ExportTrieMap &Trie,
             const MachO::SegmentList *SegList,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ExportTrieEntryCollection;

        static auto
        Open(ExportTrieMap &Trie,
             ExportTrieEntryCollectionNodeCreator &Creator,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ExportTrieEntryCollection;

        virtual void
        ParseFromTrie(ExportTrieMap &Trie,
                      const MachO::SegmentList *SegList,
                      const ParseOptions &Options = ParseOptions(),
                      Error *ErrorOut = nullptr) noexcept;

        [[nodiscard]] inline ADT::TreeNode *root() const noexcept override {
            return Root;
        }

        inline ADT::Tree &setRoot(ADT::TreeNode *const Root) noexcept override {
            this->Root = static_cast<ExportTrieChildNode *>(Root);
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
            return ConstIterator(Root);
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

    struct ExportTrieExportCollectionEntryInfo {
    protected:
        ExportTrieExportInfo Export;

        const SegmentInfo *Segment = nullptr;
        const SectionInfo *Section = nullptr;
    public:
        [[nodiscard]] constexpr auto &getInfo() const noexcept {
            return Export;
        }

        [[nodiscard]] constexpr auto &info() noexcept {
            return Export;
        }

        [[nodiscard]] constexpr auto segment() const noexcept {
            return Segment;
        }

        [[nodiscard]] constexpr auto section() const noexcept {
            return Section;
        }

        constexpr auto setSegment(const SegmentInfo *const Value) noexcept
            -> decltype(*this)
        {
            this->Segment = Value;
            return *this;
        }

        constexpr auto setSection(const SectionInfo *const Value) noexcept
            -> decltype(*this)
        {
            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieExportCollection {
    public:
        using EntryInfo = ExportTrieExportCollectionEntryInfo;
        using NodeCreator = ExportTrieEntryCollectionNodeCreator;
        using Error = ADT::TrieParseError;
        using ParseOptions = ADT::TrieParseOptions;
        using EntryListType = std::vector<std::unique_ptr<EntryInfo>>;
    protected:
        EntryListType EntryList;
        explicit ExportTrieExportCollection() noexcept = default;

        void
        Parse(const ExportTrieMap::ExportMap &Trie,
              NodeCreator &Creator,
              const ParseOptions &Options,
              Error *const ErrorOut) noexcept;
    public:
        static auto
        Open(const ExportTrieMap::ExportMap &Trie,
             const SegmentList *SegList,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ExportTrieExportCollection;

        static auto
        Open(const ExportTrieMap::ExportMap &Trie,
             const NodeCreator &Creator,
             const ParseOptions &Options = ParseOptions(),
             Error *ErrorOut = nullptr) noexcept -> ExportTrieExportCollection;

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