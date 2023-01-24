/*
 * MachO/ExportTrie.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include "ADT/FlagsBase.h"
#include "ADT/Tree.h"

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
            return kind() == Kind::Absolute;
        }

        [[nodiscard]] constexpr auto weak() const noexcept {
            return has(Masks::WeakDefinition);
        }

        [[nodiscard]] constexpr auto reexport() const noexcept {
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

    struct ExportTrieExportInfo {
    public:
        using Kind = ExportTrieExportKind;
    protected:
        std::string String;
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
            return flags().weak();
        }

        [[nodiscard]] constexpr auto reexport() const noexcept {
            return flags().reexport();
        }

        [[nodiscard]] constexpr auto stubAndResolver() const noexcept {
            return flags().stubAndResolver();
        }

        [[nodiscard]]
        constexpr auto string() const noexcept -> std::string_view {
            return String;
        }

        [[nodiscard]]
        constexpr auto reexportImportName() const noexcept -> std::string_view {
            assert(reexport());
            return String;
        }

        [[nodiscard]] constexpr auto reexportDylibOrdinal() const noexcept {
            assert(reexport());
            return Info.ReexportDylibOrdinal;
        }

        [[nodiscard]] constexpr auto resolverStubAddress() const noexcept {
            assert(stubAndResolver());
            return Info.Loc.ResolverStubAddress;
        }

        [[nodiscard]] constexpr auto imageOffset() const noexcept {
            assert(!reexport());
            return Info.Loc.ImageOffset;
        }

        constexpr auto setString(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
        }

        constexpr auto setString(std::string &&Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
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
            this->Info.ReexportDylibOrdinal = Value;
            return *this;
        }

        constexpr auto setResolverStubAddress(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            this->Info.Loc.ResolverStubAddress = Value;
            return *this;
        }

        constexpr auto setImageOffset(const uint32_t Value) noexcept
            -> decltype(*this)
        {
            this->Info.Loc.ImageOffset = Value;
            return *this;
        }

        constexpr auto clearExclusiveInfo() noexcept -> decltype(*this) {
            Info.Loc.ImageOffset = 0;
            Info.ReexportDylibOrdinal = 0;
            Info.Loc.ResolverStubAddress = 0;

            Flags.clear();
            return *this;
        }
    };

    struct ExportTrieNodeInfo {
    protected:
        uint64_t Offset = 0;
        uint64_t Size = 0;
        uint8_t ChildCount = 0;

        std::string_view Prefix;
    public:
        [[nodiscard]] constexpr auto offset() const noexcept {
            return Offset;
        }

        [[nodiscard]] constexpr auto &offsetRef() noexcept {
            return Offset;
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return Size;
        }

        [[nodiscard]] constexpr auto childCount() const noexcept {
            return ChildCount;
        }

        [[nodiscard]]
        constexpr auto prefix() const noexcept -> std::string_view {
            return Prefix;
        }

        constexpr auto setOffset(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Offset = Value;
            return *this;
        }

        constexpr auto setSize(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->Size = Value;
            return *this;
        }

        constexpr auto setChildCount(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->ChildCount = Value;
            return *this;
        }

        constexpr auto setPrefix(const std::string_view Value) noexcept
            -> decltype(*this)
        {
            this->Prefix = Value;
            return *this;
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return Size != 0;
        }
    };

    struct ExportTrieStackInfo {
    public:
        using NodeInfo = ExportTrieNodeInfo;
    protected:
        NodeInfo Node;
        uint16_t ChildOrdinal = 0;
        std::vector<ADT::Range>::size_type RangeListSize = 0;
    public:
        ExportTrieStackInfo() noexcept = default;
        ExportTrieStackInfo(const NodeInfo &Node) noexcept : Node(Node) {}

        [[nodiscard]] constexpr const auto &node() const noexcept {
            return Node;
        }

        [[nodiscard]] constexpr auto &node() noexcept {
            return Node;
        }

        [[nodiscard]] constexpr auto childOrdinal() const noexcept {
            return ChildOrdinal;
        }

        [[nodiscard]] constexpr auto &childOrdinalRef() noexcept {
            return ChildOrdinal;
        }

        [[nodiscard]] constexpr auto rangeListSize() const noexcept {
            return RangeListSize;
        }

        constexpr auto setChildOrdinal(const uint16_t Value) noexcept
            -> decltype(*this)
        {
            this->ChildOrdinal = Value;
            return *this;
        }

        constexpr
        auto setRangeListSize(const decltype(RangeListSize) Value) noexcept
            -> decltype(*this)
        {
            this->RangeListSize = Value;
            return *this;
        }
    };

    struct ExportTrieIterateInfo {
        using NodeInfo = ExportTrieNodeInfo;
        using StackInfo = ExportTrieStackInfo;
    protected:
        uint64_t MaxDepth;
        std::string String;

        std::vector<ADT::Range> RangeList;
        std::vector<StackInfo> StackList;

        ExportTrieExportKind Kind;
        ExportTrieExportInfo Export;
    public:
        [[nodiscard]] constexpr auto maxDepth() const noexcept {
            return MaxDepth;
        }

        [[nodiscard]]
        constexpr auto string() const noexcept -> std::string_view {
            return String;
        }

        [[nodiscard]] constexpr auto &stringRef() noexcept {
            return String;
        }

        [[nodiscard]] constexpr auto &rangeList() const noexcept {
            return RangeList;
        }

        [[nodiscard]] constexpr auto &rangeListRef() noexcept {
            return RangeList;
        }

        [[nodiscard]] constexpr auto &stackList() const noexcept {
            return StackList;
        }

        [[nodiscard]] constexpr auto &stackListRef() noexcept {
            return StackList;
        }

        [[nodiscard]] constexpr auto kind() const noexcept {
            return Kind;
        }

        [[nodiscard]] constexpr auto &exportInfo() const noexcept {
            return Export;
        }

        [[nodiscard]] constexpr auto &exportInfoRef() noexcept {
            return Export;
        }

        constexpr auto setMaxDepth(const uint64_t Value) noexcept
            -> decltype(*this)
        {
            this->MaxDepth = Value;
            return *this;
        }

        constexpr auto setKind(const ExportTrieExportKind Value) noexcept
            -> decltype(*this)
        {
            this->Kind = Value;
            return *this;
        }

        [[nodiscard]] inline auto &stack() noexcept {
            assert(!stackListRef().empty());
            return stackListRef().back();
        }

        [[nodiscard]] inline const StackInfo &stack() const noexcept {
            assert(!stackList().empty());
            return stackList().back();
        }

        [[nodiscard]] inline auto &node() noexcept {
            return stack().node();
        }

        [[nodiscard]] inline auto &node() const noexcept {
            return stack().node();
        }

        [[nodiscard]] constexpr bool isExport() const noexcept {
            return (kind() != ExportTrieExportKind::None);
        }

        [[nodiscard]] constexpr bool absolute() const noexcept {
            return (kind() == ExportTrieExportKind::Absolute);
        }

        [[nodiscard]] constexpr bool reexport() const noexcept {
            return (kind() == ExportTrieExportKind::Reexport);
        }

        [[nodiscard]] constexpr bool regular() const noexcept {
            return (kind() == ExportTrieExportKind::Regular);
        }

        [[nodiscard]] constexpr bool stubAndResolver() const noexcept {
            return (kind() == ExportTrieExportKind::StubAndResolver);
        }

        [[nodiscard]] constexpr bool weak() const noexcept {
            return (kind() == ExportTrieExportKind::WeakDefinition);
        }

        [[nodiscard]] constexpr bool threadLocal() const noexcept {
            return (kind() == ExportTrieExportKind::ThreadLocal);
        }

        constexpr auto setIsAbsolute() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::Absolute);
            return *this;
        }

        constexpr auto setIsReexport() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::Reexport);
            return *this;
        }

        constexpr auto setIsRegular() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::Regular);
            return *this;
        }

        constexpr auto setIsStubAndResolver() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::StubAndResolver);
            return *this;
        }

        constexpr auto setIsWeak() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::WeakDefinition);
            return *this;
        }

        constexpr auto setIsThreadLocal() noexcept -> decltype(*this) {
            setKind(ExportTrieExportKind::ThreadLocal);
            return *this;
        }

        [[nodiscard]] inline auto depthLevel() const noexcept {
            assert(!stackList().empty());
            return stackList().size();
        }
    };

    enum class ExportTrieParseError {
        None,
        InvalidUleb128,
        InvalidFormat,
        OverlappingRanges,
        TooDeep,
        EmptyExport
    };

    struct ExportTrieParseOptions {
        uint64_t RangeListReserveSize = 128;
        uint64_t StackListReserveSize = 16;
        uint64_t StringReserveSize = 128;
        uint64_t MaxDepth = 128;
    };

    struct ExportTrieIteratorEnd {};
    struct ExportTrieIterator {
    public:
        using Error = ExportTrieParseError;

        using NodeInfo = ExportTrieNodeInfo;
        using StackInfo = ExportTrieStackInfo;
        using IterateInfo = ExportTrieIterateInfo;

        using ParseOptions = ExportTrieParseOptions;
    protected:
        Error ParseError;

        const uint8_t *Begin;
        const uint8_t *End;

        std::unique_ptr<ExportTrieIterateInfo> Info;
        std::unique_ptr<StackInfo> NextStack;

        void SetupInfoForNewStack() noexcept;
        [[nodiscard]] bool MoveUptoParentNode() noexcept;

        Error ParseNode(const uint8_t *Begin, NodeInfo *InfoOut) noexcept;
        Error ParseNextNode(const uint8_t *& Ptr, NodeInfo *InfoOut) noexcept;
        Error Advance() noexcept;
    public:
        explicit
        ExportTrieIterator(
            const uint8_t *Begin,
            const uint8_t *End,
            const ParseOptions &Options = ParseOptions()) noexcept;

        [[nodiscard]] inline auto &info() noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto &info() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline bool isAtEnd() const noexcept {
            return info().stackListRef().empty();
        }

        [[nodiscard]] constexpr auto hasError() const noexcept {
            return true;
        }

        [[nodiscard]] inline auto getError() const noexcept {
            return Error::None;
        }

        inline auto operator++() noexcept -> decltype(*this) {
            assert(!hasError());
            ParseError = Advance();

            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return operator++();
        }

        inline auto operator+=(const uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                operator++();
            }

            return *this;
        }

        [[nodiscard]] inline auto &operator*() noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto&operator*() const noexcept {
            return *Info;
        }

        [[nodiscard]] inline auto *operator->() noexcept {
            return Info.get();
        }

        [[nodiscard]] inline const auto *operator->() const noexcept {
            return Info.get();
        }

        [[nodiscard]] inline
        auto operator==([[maybe_unused]] const ExportTrieIteratorEnd &End) {
            return isAtEnd();
        }

        [[nodiscard]] inline
        auto operator!=([[maybe_unused]] const ExportTrieIteratorEnd &End) {
            return !operator==(End);
        }
    };

    struct ExportTrieExportIterator {
    public:
        using Error = ExportTrieParseError;
        using ParseOptions = ExportTrieParseOptions;
    protected:
        ExportTrieIterator Iterator;
        void Advance() noexcept;
    public:
        explicit
        ExportTrieExportIterator(const uint8_t *Begin,
                                 const uint8_t *End) noexcept
        : Iterator(Begin, End) {
            Advance();
        }

        explicit
        ExportTrieExportIterator(const uint8_t *Begin,
                                 const uint8_t *End,
                                 const ParseOptions &Options) noexcept
        : Iterator(Begin, End, Options) {
            Advance();
        }

        [[nodiscard]] inline ExportTrieIterateInfo &getInfo() noexcept {
            return Iterator.info();
        }

        [[nodiscard]]
        inline const auto &info() const noexcept {
            return Iterator.info();
        }

        [[nodiscard]] inline bool isAtEnd() const noexcept {
            return Iterator.isAtEnd();
        }

        [[nodiscard]] inline bool hasError() const noexcept {
            return Iterator.hasError();
        }

        [[nodiscard]] inline Error getError() const noexcept {
            return Iterator.getError();
        }

        inline auto operator++() noexcept -> decltype(*this) {
            Advance();
            return *this;
        }

        inline auto operator++(int) noexcept -> decltype(*this) {
            return operator++();
        }

        inline auto operator+=(const uint64_t Amt) noexcept -> decltype(*this) {
            for (auto I = uint64_t(); I != Amt; I++) {
                operator++();
            }

            return *this;
        }

        [[nodiscard]] inline auto &operator*() noexcept {
            return Iterator.info();
        }

        [[nodiscard]]
        inline const auto &operator*() const noexcept {
            return Iterator.info();
        }

        [[nodiscard]] inline auto operator->() noexcept {
            return &Iterator.info();
        }

        [[nodiscard]] inline auto operator->() const noexcept {
            return &Iterator.info();
        }

        [[nodiscard]] inline
        auto operator==([[maybe_unused]] const ExportTrieIteratorEnd &End) {
            return (Iterator == End);
        }

        [[nodiscard]] inline
        auto operator!=([[maybe_unused]] const ExportTrieIteratorEnd &End) {
            return !operator==(End);
        }
    };

    struct ExportTrieMap {
    public:
        using ParseOptions = ExportTrieParseOptions;
    protected:
        const uint8_t *Begin;
        const uint8_t *End;
    public:
        constexpr
        ExportTrieMap(const uint8_t *const Begin,
                      const uint8_t *const End) noexcept
        : Begin(Begin), End(End) {}

        constexpr ExportTrieMap(const ADT::MemoryMap &Map) noexcept
        : Begin(Map.base<uint8_t>()), End(Map.end<uint8_t>()) {}

        using Iterator = ExportTrieIterator;

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return Iterator(Begin, End);
        }

        [[nodiscard]]
        inline auto begin(const ParseOptions &Options) const noexcept {
            return Iterator(Begin, End, Options);
        }

        [[nodiscard]]
        inline auto cbegin(const ParseOptions &Options) const noexcept {
            return Iterator(Begin, End, Options);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return ExportTrieIteratorEnd();
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return ExportTrieIteratorEnd();
        }

        struct ExportMap {
        protected:
            const uint8_t *Begin;
            const uint8_t *End;
        public:
            constexpr
            ExportMap(const uint8_t *const Begin,
                      const uint8_t *const End) noexcept
            : Begin(Begin), End(End) {}

            constexpr ExportMap(const ADT::MemoryMap &Map) noexcept
            : Begin(Map.base<uint8_t>()), End(Map.end<uint8_t>()) {}

            using Iterator = ExportTrieExportIterator;

            [[nodiscard]] inline auto begin() const noexcept {
                return Iterator(Begin, End);
            }

            [[nodiscard]] inline auto cbegin() const noexcept {
                return Iterator(Begin, End);
            }

            [[nodiscard]]
            inline auto begin(const ParseOptions &Options) const noexcept {
                return Iterator(Begin, End, Options);
            }

            [[nodiscard]]
            inline auto cbegin(const ParseOptions &Options) const noexcept {
                return Iterator(Begin, End, Options);
            }

            [[nodiscard]] inline auto end() const noexcept {
                return ExportTrieIteratorEnd();
            }

            [[nodiscard]] inline auto cend() const noexcept {
                return ExportTrieIteratorEnd();
            }
        };

        [[nodiscard]] constexpr auto exportMap() const noexcept {
            return ExportMap(Begin, End);
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

        [[nodiscard]] static inline auto get(const TreeNode *Node) noexcept {
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

        [[nodiscard]] constexpr auto string() const noexcept {
            return String;
        }

        constexpr auto setKind(const ExportTrieExportKind Kind) noexcept
            -> decltype(*this)
        {
            this->Kind = Kind;
            return *this;
        }

        constexpr auto setString(const std::string &Value) noexcept
            -> decltype(*this)
        {
            this->String = Value;
            return *this;
        }

        [[nodiscard]] constexpr auto isExport() const noexcept {
            return (kind() != ExportTrieExportKind::None);
        }

        [[nodiscard]] constexpr auto reexport() const noexcept {
            return (kind() == ExportTrieExportKind::Reexport);
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

        [[nodiscard]] auto getAsExportNode() noexcept {
            assert(this->isExport());
            return reinterpret_cast<ExportTrieExportChildNode *>(this);
        }

        [[nodiscard]] inline auto getAsExportNode() const noexcept {
            assert(this->isExport());
            return reinterpret_cast<const ExportTrieExportChildNode *>(this);
        }

        inline auto setParent(ExportTrieChildNode *Parent) noexcept
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
            assert(!this->reexport());
            return Segment;
        }

        [[nodiscard]] inline auto section() const noexcept {
            assert(!this->reexport());
            return Section;
        }

        constexpr auto setSegment(const SegmentInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->reexport());

            this->Segment = Value;
            return *this;
        }

        constexpr auto setSection(const SectionInfo *const Value) noexcept
            -> decltype(*this)
        {
            assert(!this->reexport());

            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieEntryCollection : public ADT::Tree {
    public:
        using ChildNode = ExportTrieChildNode;
        using ExportChildNode = ExportTrieExportChildNode;
        using Error = ExportTrieParseError;
    protected:
        explicit ExportTrieEntryCollection() noexcept = default;

        [[nodiscard]] virtual auto
        LookupInfoForAddress(const MachO::SegmentList &SegList,
                             uint64_t Address,
                             const SectionInfo **SectionOut) const noexcept
            -> const SegmentInfo *;

        [[nodiscard]] auto
        MakeNodeForEntryInfo(const ExportTrieIterateInfo &Info,
                             const MachO::SegmentList *SegList) noexcept
            -> ChildNode *;

        void
        ParseFromTrie(const MachO::ExportTrieMap &Trie,
                      const MachO::SegmentList *SegList,
                      Error *Error) noexcept;
    public:
        static ExportTrieEntryCollection
        Open(const MachO::ExportTrieMap &Trie,
             const MachO::SegmentList *SegList,
             Error *ErrorOut = nullptr);

        [[nodiscard]] inline auto root() const noexcept {
            return reinterpret_cast<ChildNode *>(Root);
        }

        inline
        auto RemoveNode(ChildNode &Ref, const bool RemoveParentLeafs) noexcept {
            auto &Node = reinterpret_cast<ADT::TreeNode &>(Ref);
            const auto Result = Tree::removeNode(Node, RemoveParentLeafs);

            return reinterpret_cast<ChildNode *>(Result);
        }

        using Iterator = ADT::TreeDFSIterator<ChildNode>;
        using ConstIterator = ADT::TreeDFSIterator<const ChildNode>;

        [[nodiscard]] constexpr auto begin() const noexcept {
            return Iterator(root());
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return Iterator(nullptr);
        }

        [[nodiscard]] constexpr auto cbegin() const noexcept {
            return ConstIterator(root());
        }

        [[nodiscard]] constexpr auto cend() const noexcept {
            return ConstIterator(nullptr);
        }

        template <typename T>
        inline auto forEach(const T &Callback) noexcept -> decltype(*this) {
            forEachNode(Callback);
            return *this;
        }

        template <typename T>
        inline auto forEach(const T &Callback) const noexcept
            -> decltype(*this)
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

        constexpr
        auto setSegment(const SegmentInfo *Value) noexcept -> decltype(*this) {
            this->Segment = Value;
            return *this;
        }

        constexpr
        auto setSection(const SectionInfo *Value) noexcept -> decltype(*this) {
            this->Section = Value;
            return *this;
        }
    };

    struct ExportTrieExportCollection : public ADT::Tree {
    public:
        using EntryInfo = ExportTrieExportCollectionEntryInfo;
        using Error = ExportTrieParseError;
        using EntryListType = std::vector<std::unique_ptr<EntryInfo>>;
    private:
        explicit ExportTrieExportCollection() noexcept = default;
    protected:
        EntryListType EntryList;
    public:
        static ExportTrieExportCollection
        Open(const ExportTrieMap::ExportMap &Trie,
             const MachO::SegmentList *SegList,
             Error *Error) noexcept;

        using Iterator = EntryListType::iterator;
        using ConstIterator = EntryListType::const_iterator;

        [[nodiscard]] inline Iterator begin() noexcept {
            return EntryList.begin();
        }

        [[nodiscard]] inline Iterator end() noexcept {
            return EntryList.end();
        }

        [[nodiscard]] inline ConstIterator begin() const noexcept {
            return EntryList.cbegin();
        }

        [[nodiscard]] inline ConstIterator end() const noexcept {
            return EntryList.cend();
        }
    };
}