/*
 * ADT/Trie.h
 * © suhas pai
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ADT/MemoryMap.h"
#include "ADT/Tree.h"

namespace ADT {
    struct TrieNodeInfo {
    protected:
        uint64_t Offset = 0;
        uint64_t Size = 0;
        uint8_t ChildCount = 0;

        std::string_view Prefix;
    public:
        explicit TrieNodeInfo() noexcept = default;

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

    enum class TrieParseError {
        None,
        InvalidUleb128,
        InvalidFormat,
        OverlappingRanges,
        TooDeep,
    };

    struct TrieParser {
    public:
        explicit TrieParser() noexcept = default;

        using Error = TrieParseError;
        using NodeInfo = TrieNodeInfo;

        virtual auto
        ParseNode(uint8_t *Begin,
                  uint8_t *Ptr,
                  uint8_t *End,
                  std::vector<ADT::Range> &RangeList,
                  TrieNodeInfo *const InfoOut) noexcept -> Error;

        virtual auto
        ParseNextNode(uint8_t *const Begin,
                      uint8_t *& Ptr,
                      uint8_t *const End,
                      std::vector<ADT::Range> &RangeList,
                      TrieNodeInfo *const InfoOut) noexcept -> Error;
    };

    struct TrieStackInfo {
    public:
        using NodeInfo = TrieNodeInfo;
    protected:
        NodeInfo Node;
        uint16_t ChildOrdinal = 0;
        std::vector<ADT::Range>::size_type RangeListSize = 0;
    public:
        explicit TrieStackInfo() noexcept = default;
        explicit TrieStackInfo(const NodeInfo &Node) noexcept : Node(Node) {}

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

    template<typename T>
    concept TrieExportInfoParser =
        requires(T a,
                 uint8_t *& Ptr,
                 uint8_t *NodeEnd,
                 uint8_t *TrieEnd,
                 std::string_view String,
                 std::vector<ADT::Range> &RangeList,
                 std::vector<TrieStackInfo> &StackList)
        {
            {
                a.ParseExportData(Ptr,
                                  NodeEnd,
                                  TrieEnd,
                                  String,
                                  RangeList,
                                  StackList)
            } noexcept -> std::same_as<TrieParseError>;
            { a.clearExclusiveInfo() } noexcept;
        };

    struct TrieParseOptions {
        uint64_t RangeListReserveSize = 128;
        uint64_t StackListReserveSize = 16;
        uint64_t StringReserveSize = 128;
        uint64_t MaxDepth = 128;
    };

    template <TrieExportInfoParser T>
    struct Trie {
    public:
        using NodeInfo = TrieNodeInfo;
        using StackInfo = TrieStackInfo;
        using ParseOptions = TrieParseOptions;
        using ParseError = TrieParseError;

        struct IterateInfo {
        protected:
            uint64_t MaxDepth;
            std::string String;

            std::vector<ADT::Range> RangeList;
            std::vector<StackInfo> StackList;

            T &ExportInfo;
            bool IsExport;
        public:
            explicit IterateInfo(T &ExportInfo) noexcept
            : ExportInfo(ExportInfo) {}

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

            [[nodiscard]] inline auto &stack() noexcept {
                assert(!stackListRef().empty());
                return stackListRef().back();
            }

            [[nodiscard]] inline auto &stack() const noexcept {
                assert(!stackList().empty());
                return stackList().back();
            }

            [[nodiscard]] inline auto &node() noexcept {
                return stack().node();
            }

            [[nodiscard]] inline auto &node() const noexcept {
                return stack().node();
            }

            [[nodiscard]] constexpr auto isExport() const noexcept {
                return  IsExport;
            }

            [[nodiscard]] inline auto &exportInfo() const noexcept {
                return ExportInfo;
            }

            [[nodiscard]] inline auto &exportInfo() noexcept {
                return ExportInfo;
            }

            [[nodiscard]] inline auto depthLevel() const noexcept {
                assert(!stackList().empty());
                return stackList().size();
            }

            constexpr auto setMaxDepth(const uint64_t Value) noexcept
                -> decltype(*this)
            {
                this->MaxDepth = Value;
                return *this;
            }

            constexpr auto setIsExport(const bool Value = true) noexcept
                -> decltype(*this)
            {
                this->IsExport = Value;
                return *this;
            }
        };

        struct IteratorEnd {};
        struct Iterator {
        public:
            using Error = TrieParseError;
            using ParseOptions = TrieParseOptions;

            enum class Direction {
                Normal,
                MoveUptoParentNode
            };
        protected:
            Error ParseError;

            uint8_t *Begin;
            uint8_t *End;

            std::unique_ptr<IterateInfo> Info;
            std::unique_ptr<StackInfo> NextStack;

            Direction Direction;
            TrieParser &Parser;

            void SetupInfoForNewStack() noexcept {
                Info->stringRef().append(NextStack->node().prefix());
                Info->stackListRef().emplace_back(std::move(*NextStack));
            }

            [[nodiscard]] auto MoveUptoParentNode() noexcept -> bool {
                auto &StackList = Info->stackListRef();
                if (StackList.size() == 1) {
                    StackList.clear();
                    return false;
                }

                auto &String = Info->stringRef();
                auto &Top = StackList.back();

                const auto EraseSuffixLength =
                    String.length() - Top.node().prefix().length();

                String.erase(EraseSuffixLength);
                if (Top.rangeListSize() != 0) {
                    auto &RangeList = Info->rangeListRef();
                    RangeList.erase(RangeList.cbegin() +
                                        static_cast<long>(Top.rangeListSize()),
                                    RangeList.cend());
                }

                StackList.pop_back();
                StackList.back().childOrdinalRef() += 1;

                return true;
            }

            [[nodiscard]] auto Advance() noexcept -> Error {
                auto &StackList = Info->stackListRef();
                const auto MaxDepth = Info->maxDepth();

                if (!StackList.empty()) {
                    if (StackList.size() == MaxDepth) {
                        return Error::TooDeep;
                    }

                    auto &PrevStack = StackList.back();
                    const auto &PrevNode = PrevStack.node();

                    if (PrevNode.isExport()) {
                        Info->exportInfo().clearExclusiveInfo();
                        Info->setIsExport(false);

                        if (PrevNode.childCount() == 0) {
                            if (!MoveUptoParentNode()) {
                                return Error::None;
                            }
                        }

                        PrevStack.setChildOrdinal(1);
                    } else {
                        switch (Direction) {
                            case Direction::Normal:
                                PrevStack.childOrdinalRef() += 1;
                                SetupInfoForNewStack();

                                break;
                            case Direction::MoveUptoParentNode:
                                if (MoveUptoParentNode()) {}
                                break;
                        }
                    }

                    switch (Direction) {
                        case Direction::Normal:
                            break;
                        case Direction::MoveUptoParentNode:
                            if (MoveUptoParentNode()) {}
                            return Error::None;
                    }
                } else {
                    SetupInfoForNewStack();
                }

                this->Direction = Direction::Normal;

                do {
                    auto &Stack = StackList.back();
                    auto &Node = Stack.node();

                    auto Ptr =
                        Utils::AddPtrAndCheckOverflow(this->Begin,
                                                      Node.offset()).value();

                    const auto ExpectedEnd = Ptr + Node.size();
                    const auto UpdateOffset = [&]() noexcept {
                        Node.setOffset(
                            static_cast<uint64_t>(Ptr - this->Begin));
                    };

                    if (Stack.childOrdinal() == 0) {
                        const auto IsExportInfo = Node.isExport();
                        if (IsExportInfo) {
                            Info->setIsExport(true);
                            const auto Error =
                                Info->exportInfo().ParseExportData(
                                    Ptr,
                                    ExpectedEnd,
                                    End,
                                    Info->string(),
                                    Info->rangeListRef(),
                                    StackList);

                            UpdateOffset();
                            if (Error != TrieParseError::None) {
                                return Error;
                            }

                            // Return later, after checking the child-count, so
                            // we can update the stack-list if necessary.
                        }

                        // We're done with this entire stack if this node has no
                        // children. To finish, we move to the parent-stack. If
                        // we don't have a parent-stack, we've reached the end
                        // of th export-trie.

                        if (Node.childCount() == 0) {
                            if (IsExportInfo) {
                                break;
                            }

                            if (MoveUptoParentNode()) {
                                continue;
                            }

                            break;
                        }

                        // Skip the byte storing the child-count.
                        Ptr++;
                        Node.offsetRef() += 1;

                        if (IsExportInfo) {
                            break;
                        }
                    } else if (Stack.childOrdinal() == Node.childCount() + 1) {
                        // We've finished with this node if ChildOrdinal is past
                        // ChildCount.

                        if (MoveUptoParentNode()) {
                            continue;
                        }

                        break;
                    }

                    // Prepare the next-stack before returning.
                    const auto Error =
                        Parser.ParseNextNode(Begin,
                                             Ptr,
                                             End,
                                             Info->rangeListRef(),
                                             &NextStack->node());

                    UpdateOffset();
                    if (Error != Error::None) {
                        return Error;
                    }

                    NextStack->setRangeListSize(Info->rangeList().size());

                    // We've already returned this node once if our
                    // Child-Ordinal is not zero.

                    if (Stack.childOrdinal() == 0) {
                        break;
                    }

                    if (StackList.size() == MaxDepth) {
                        return Error::TooDeep;
                    }

                    SetupInfoForNewStack();
                } while (true);

                return Error::None;
            }
        public:
            explicit
            Iterator(uint8_t *const Begin,
                     uint8_t *const End,
                     TrieParser &Parser,
                     T &ExportInfoParser,
                     const ParseOptions &Options = ParseOptions()) noexcept
            : Begin(Begin), End(End),
              Info(std::make_unique<IterateInfo>(ExportInfoParser)),
              Parser(Parser)
            {
                Info->setMaxDepth(Options.MaxDepth);

                Info->rangeListRef().reserve(Options.RangeListReserveSize);
                Info->stackListRef().reserve(Options.StackListReserveSize);
                Info->stringRef().reserve(Options.StringReserveSize);

                auto Node = NodeInfo();
                this->ParseError =
                    Parser.ParseNode(Begin,
                                     Begin,
                                     End,
                                     Info->rangeListRef(),
                                     &Node);

                if (ParseError != Error::None) {
                    return;
                }

                if (Node.childCount() != 0) {
                    NextStack = std::make_unique<StackInfo>(Node);
                    this->ParseError = Advance();
                }
            }

            [[nodiscard]] inline auto &info() noexcept {
                return *Info;
            }

            [[nodiscard]] inline auto &info() const noexcept {
                return *Info;
            }

            [[nodiscard]] inline auto isAtEnd() const noexcept {
                return info().stackListRef().empty();
            }

            [[nodiscard]] constexpr auto hasError() const noexcept {
                return ParseError != Error::None;
            }

            [[nodiscard]] constexpr auto getError() const noexcept {
                return ParseError;
            }

            constexpr auto setDirection(const enum Direction Direction) noexcept
                -> decltype(*this)
            {
                this->Direction = Direction;
                return *this;
            }

            inline auto operator++() noexcept -> decltype(*this) {
                assert(!hasError());
                ParseError = Advance();

                return *this;
            }

            inline auto operator++(int) noexcept -> decltype(*this) {
                return operator++();
            }

            inline auto operator+=(const uint64_t Amt) noexcept
                -> decltype(*this)
            {
                for (auto I = uint64_t(); I != Amt; I++) {
                    operator++();
                }

                return *this;
            }

            [[nodiscard]] inline auto &operator*() noexcept {
                return *Info;
            }

            [[nodiscard]] inline auto &operator*() const noexcept {
                return *Info;
            }

            [[nodiscard]] inline auto *operator->() noexcept {
                return Info.get();
            }

            [[nodiscard]] inline const auto *operator->() const noexcept {
                return Info.get();
            }

            [[nodiscard]]
            inline auto operator==(const IteratorEnd &) const noexcept {
                return isAtEnd();
            }

            [[nodiscard]]
            inline auto operator!=(const IteratorEnd &End) const noexcept {
                return !operator==(End);
            }
        };

        struct ExportIterator {
        public:
            using Error = ParseError;
        protected:
            Iterator Iter;
            auto Advance() noexcept -> decltype(*this) {
                do {
                    Iter++;
                    if (Iter.hasError()) {
                        break;
                    }

                    if (Iter.isAtEnd()) {
                        break;
                    }

                    if (Iter.info().node().isExport()) {
                        break;
                    }
                } while (true);

                return *this;
            }
        public:
            explicit
            ExportIterator(
                uint8_t *Begin,
                uint8_t *End,
                TrieParser &TrieParser,
                T &ExportInfoParser,
                const ParseOptions &Options = ParseOptions()) noexcept
            : Iter(Begin, End, TrieParser, ExportInfoParser, Options) {
                if (!Iter.hasError() &&
                    !Iter.isAtEnd() &&
                    !Iter.info().node().isExport())
                {
                    Advance();
                }
            }

            explicit
            ExportIterator(
                const ADT::MemoryMap &Map,
                TrieParser &TrieParser,
                T &ExportInfoParser,
                const ParseOptions &Options = ParseOptions()) noexcept
            : Iter(Map, TrieParser, ExportInfoParser, Options) {
                if (!Iter.hasError() &&
                    !Iter.isAtEnd() &&
                    !Iter.info().node().isExport())
                {
                    Advance();
                }
            }

            [[nodiscard]] inline auto &info() noexcept {
                return Iter.info();
            }

            [[nodiscard]] inline auto &info() const noexcept {
                return Iter.info();
            }

            [[nodiscard]] inline auto isAtEnd() const noexcept {
                return Iter.isAtEnd();
            }

            [[nodiscard]] inline auto hasError() const noexcept {
                return Iter.hasError();
            }

            [[nodiscard]] inline auto getError() const noexcept {
                return Iter.getError();
            }

            inline auto operator++() noexcept -> decltype(*this) {
                Advance();
                return *this;
            }

            inline auto operator++(int) noexcept -> decltype(*this) {
                return operator++();
            }

            inline auto operator+=(const uint64_t Amt) noexcept
                -> decltype(*this)
            {
                for (auto I = uint64_t(); I != Amt; I++) {
                    operator++();
                }

                return *this;
            }

            [[nodiscard]] inline auto &operator*() noexcept {
                return Iter.info();
            }

            [[nodiscard]] inline const auto &operator*() const noexcept {
                return Iter.info();
            }

            [[nodiscard]] inline auto operator->() noexcept {
                return &Iter.info();
            }

            [[nodiscard]] inline auto operator->() const noexcept {
                return &Iter.info();
            }

            [[nodiscard]]
            inline auto operator==(const IteratorEnd &End) const noexcept {
                return Iter== End;
            }

            [[nodiscard]]
            inline auto operator!=(const IteratorEnd &End) const noexcept {
                return !operator==(End);
            }
        };
    protected:
        uint8_t *Begin;
        uint8_t *End;

        TrieParser &Parser;
        T &ExportInfoParser;
    public:
        constexpr explicit
        Trie(uint8_t *const Begin,
             uint8_t *const End,
             TrieParser &TrieParser,
             T &ExportInfoParser) noexcept
        : Begin(Begin), End(End), Parser(TrieParser),
          ExportInfoParser(ExportInfoParser) {};

        constexpr explicit
        Trie(const ADT::MemoryMap &Map,
             TrieParser &TrieParser,
             T &ExportInfoParser) noexcept
        : Begin(Map.base<uint8_t>()),
          End(const_cast<uint8_t *>(Map.end<uint8_t>())),
          Parser(TrieParser), ExportInfoParser(ExportInfoParser) {};

        [[nodiscard]] inline auto begin() const noexcept {
            return Iterator(Begin, End, Parser, ExportInfoParser);
        }

        [[nodiscard]] inline auto cbegin() const noexcept {
            return Iterator(Begin, End, Parser, ExportInfoParser);
        }

        [[nodiscard]]
        inline auto begin(const ParseOptions &Options) const noexcept {
            return Iterator(Begin, End, Parser, ExportInfoParser, Options);
        }

        [[nodiscard]]
        inline auto cbegin(const ParseOptions &Options) const noexcept {
            return Iterator(Begin, End, Parser, ExportInfoParser, Options);
        }

        [[nodiscard]] inline auto end() const noexcept {
            return IteratorEnd();
        }

        [[nodiscard]] inline auto cend() const noexcept {
            return IteratorEnd();
        }

        struct ExportMap {
        protected:
            uint8_t *Begin;
            uint8_t *End;

            TrieParser &Parser;
            T &ExportInfoParser;
        public:
            constexpr
            ExportMap(uint8_t *const Begin,
                      uint8_t *const End,
                      TrieParser &Parser,
                      T &ExportInfoParser) noexcept
            : Begin(Begin), End(End), Parser(Parser),
              ExportInfoParser(ExportInfoParser) {}

            explicit
            ExportMap(const ADT::MemoryMap &Map,
                      TrieParser &Parser,
                      T &ExportInfoParser) noexcept
            : Begin(Map.base<uint8_t>()),
              End(const_cast<uint8_t *>(Map.end<uint8_t>())),
              Parser(Parser), ExportInfoParser(ExportInfoParser) {}

            using Iterator = ExportIterator;

            [[nodiscard]] inline auto begin() const noexcept {
                return Iterator(Begin, End, Parser, ExportInfoParser);
            }

            [[nodiscard]] inline auto cbegin() const noexcept {
                return Iterator(Begin, End, Parser, ExportInfoParser);
            }

            [[nodiscard]]
            inline auto begin(const ParseOptions &Options) const noexcept {
                return Iterator(Begin, End, Parser, ExportInfoParser, Options);
            }

            [[nodiscard]]
            inline auto cbegin(const ParseOptions &Options) const noexcept {
                return Iterator(Begin, End, Parser, ExportInfoParser, Options);
            }

            [[nodiscard]] inline auto end() const noexcept {
                return IteratorEnd();
            }

            [[nodiscard]] inline auto cend() const noexcept {
                return IteratorEnd();
            }
        };

        [[nodiscard]] inline auto exportMap() const noexcept {
            return ExportMap(Begin, End, Parser, ExportInfoParser);
        }
    };

    template<typename T, typename U>
    concept TrieNodeCollectionNodeCreator =
        requires(T a, typename Trie<U>::IterateInfo &b) {
            { a.createChildNode(b) } noexcept -> TreeNodeDerived;
        };

    template <TreeDerived TreeType,
              TrieExportInfoParser T,
              TrieNodeCollectionNodeCreator<T> NodeCreatorType>

    auto
    TrieCreateTree(TreeType &Tree,
                   Trie<T> &Trie,
                   NodeCreatorType &NodeCreator,
                   const TrieParseOptions &Options = TrieParseOptions(),
                   TrieParseError *const ErrorOut = nullptr) noexcept
    {
        auto Iter = Trie.begin(Options);
        const auto End = Trie.end();

        if (Iter == End) {
            return;
        }

        setRoot(NodeCreator.createChildNode(*Iter));

        auto Parent = Tree.root();
        auto PrevDepthLevel = uint64_t(1);

        const auto MoveUpParentHierarchy = [&](const uint64_t Amt) noexcept {
            for (auto I = uint64_t(); I != Amt; I++) {
                Parent = Parent->parent();
            }
        };

        for (Iter++; Iter != End; Iter++) {
            if (Iter.hasError()) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Iter.getError();
                }

                return;
            }

            const auto DepthLevel = Iter->depthLevel();
            if (PrevDepthLevel > DepthLevel) {
                MoveUpParentHierarchy(
                    static_cast<uint64_t>(PrevDepthLevel) - DepthLevel);
            }

            const auto Current = NodeCreator.createChildNode(*Iter);

            Parent->addChild(*Current);
            if (Iter->node().childCount() != 0) {
                Parent = Current;
            }

            PrevDepthLevel = DepthLevel;
        }

        return Tree;
    }

    template <TrieExportInfoParser T,
              TrieNodeCollectionNodeCreator<T> NodeCreatorType>

    struct TrieNodeCollection : public ADT::Tree {
    public:
        using ParseOptions = TrieParseOptions;
        using Error = TrieParseError;
    public:
        explicit TrieNodeCollection() noexcept = default;

        void
        ParseFromTrie(Trie<T> &Trie,
                      NodeCreatorType &NodeCreator,
                      const ParseOptions &Options = ParseOptions(),
                      Error *const ErrorOut = nullptr) noexcept
        {
            auto Iter = Trie.begin(Options);
            if (Iter.hasError()) {
                if (ErrorOut != nullptr) {
                    *ErrorOut = Iter.getError();
                }

                return;
            }

            const auto End = Trie.end();
            if (Iter == End) {
                return;
            }

            setRoot(NodeCreator.createChildNode(*Iter));

            auto Parent = root();
            auto PrevDepthLevel = uint64_t(1);

            const auto MoveUpParentHierarchy = [&](const uint64_t Amt) noexcept
            {
                for (auto I = uint64_t(); I != Amt; I++) {
                    Parent = Parent->parent();
                }
            };

            for (Iter++; Iter != End; Iter++) {
                if (Iter.hasError()) {
                    if (ErrorOut != nullptr) {
                        *ErrorOut = Iter.getError();
                    }

                    return;
                }

                const auto DepthLevel = Iter->depthLevel();
                if (PrevDepthLevel > DepthLevel) {
                    MoveUpParentHierarchy(
                        static_cast<uint64_t>(PrevDepthLevel) - DepthLevel);
                }

                const auto Current = NodeCreator.createChildNode(*Iter);

                Parent->addChild(*Current);
                if (Iter->node().childCount() != 0) {
                    Parent = Current;
                }

                PrevDepthLevel = DepthLevel;
            }
        }
    };
}