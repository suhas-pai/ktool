/*
 * ADT/Tree.h
 * © suhas pai
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <set>

#include "Utils/Print.h"

namespace ADT {
    template <typename T>
    struct TreeDFSIterator {
    protected:
        T *Current = nullptr;
        T *End = nullptr;

        uint64_t DepthLevel = 1;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using element_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        constexpr TreeDFSIterator() noexcept = default;
        constexpr TreeDFSIterator(T *const Current) noexcept
        : TreeDFSIterator(Current, Current->parent()) {}

        constexpr TreeDFSIterator(T *const Current, T *const End) noexcept
        : Current(Current), End(End) {}

        [[nodiscard]] constexpr auto &operator*() const noexcept {
            return *Current;
        }

        [[nodiscard]] constexpr auto &operator*() noexcept {
            return *Current;
        }

        [[nodiscard]] constexpr auto operator->() const noexcept {
            return Current;
        }

        [[nodiscard]] constexpr auto node() const noexcept {
            return Current;
        }

        [[nodiscard]] constexpr auto depthLevel() const noexcept {
            return DepthLevel;
        }

        [[nodiscard]]
        constexpr auto printLineLength(const uint64_t TabLength) noexcept {
            return (TabLength * (depthLevel() - 1));
        }

        [[nodiscard]] auto isAtEnd() const noexcept {
            return (Current == End);
        }

        [[nodiscard]] constexpr
        auto getParentAtIndex(const uint64_t DepthIndex) const noexcept {
            auto Parent = Current;

            const auto ThisDepthIndex = DepthLevel - 1;
            const auto MoveCount = ThisDepthIndex - DepthIndex;

            for (auto I = uint64_t(); I != MoveCount; I++) {
                Parent = T::get(Parent->parent());
            }

            return Parent;
        }

        constexpr auto operator--() noexcept -> decltype(*this) {
            auto Node = Current;
            Current = nullptr;

            for (; DepthLevel != 0; DepthLevel--, Node = Node->parent()) {
                if (const auto PrevSibling = Node->prevSibling()) {
                    Current = PrevSibling->get(PrevSibling);
                    break;
                }
            }

            return *this;
        }

        constexpr auto operator--(int) noexcept {
            return operator--();
        }

        constexpr auto operator++() noexcept -> decltype(*this) {
            if (const auto FirstChild = Current->firstChild()) {
                Current = reinterpret_cast<T *>(FirstChild);
                DepthLevel += 1;

                return *this;
            }

            for (; Current != End; DepthLevel--, Current = Current->parent()) {
                if (const auto NextSibling = Current->nextSibling()) {
                    Current = T::get(NextSibling);
                    break;
                }
            }

            return *this;
        }

        constexpr auto operator++(int) noexcept {
            return operator++();
        }

        constexpr auto operator+=(const size_t Amount) noexcept
            -> decltype(*this)
        {
            for (auto I = 0; I != Amount; I++) {
                operator++();
            }

            return *this;
        }

        constexpr auto operator-=(const size_t Amount) noexcept
            -> decltype(*this)
        {
            for (auto I = 0; I != Amount; I++) {
                operator--();
            }

            return *this;
        }

        constexpr auto operator=(T *const Node) noexcept -> decltype(*this) {
            this->Current = Node;
            return *this;
        }

        [[nodiscard]]
        constexpr bool operator==(const TreeDFSIterator &Iter) const noexcept {
            return (Current == Iter.Current);
        }

        [[nodiscard]]
        constexpr bool operator!=(const TreeDFSIterator &Iter) const noexcept {
            return !operator==(Iter);
        }
    };

#define TREE_NODE_ITERATE_CHILDREN(Node, Iter)                                 \
    for (auto Iter = (Node).get((Node).firstChild());                          \
         Iter != nullptr;                                                      \
         Iter = (Iter)->get(Iter->nextSibling()))

#define TREE_NODE_ITERATE_CHILD_TILL_END(Node, End, Iter)                      \
    for (auto Iter = (Node).get((Node).firstChild());                          \
         Iter != (End);                                                        \
         Iter = (Iter)->get(Iter->nextSibling()))

#define TREE_NODE_ITERATE_CHILD_AFTER_FIRST(Node, Iter)                        \
    for (auto Iter = (Node).get((Node).firstChild()->nextSibling());           \
         Iter != nullptr;                                                      \
         Iter = (Iter)->get(Iter->nextSibling()))

#define TREE_NODE_ITERATE_SIBLINGS(Node, Iter)                                 \
    for (auto Iter = &(Node);                                                  \
         Iter != nullptr;                                                      \
         Iter = (Iter)->get(Iter->nextSibling()))

#define TREE_NODE_ITERATE_SIBLINGS_TILL_END(Node, End, Iter)                   \
    for (auto Iter = &(Node);                                                  \
         Iter != (End);                                                        \
         Iter = (Iter)->get(Iter->nextSibling()))

    struct Tree;
    struct TreeNode {
        friend struct Tree;
    protected:
        TreeNode *Parent = nullptr;
        TreeNode *PrevSibling = nullptr;
        TreeNode *NextSibling = nullptr;
        TreeNode *FirstChild = nullptr;
        TreeNode *LastChild = nullptr;

        void clearAndDestroy() noexcept {
            setParent(nullptr);
            setFirstChild(nullptr);
            setPrevSibling(nullptr);
            setNextSibling(nullptr);
            setLastChild(nullptr);

            delete this;
        }
    public:
        constexpr TreeNode() noexcept = default;

        [[nodiscard]] virtual auto createNew() const noexcept -> TreeNode * {
            return new TreeNode;
        }

        virtual ~TreeNode() noexcept = default;

        [[nodiscard]] constexpr auto parent() const noexcept {
            return Parent;
        }

        [[nodiscard]] constexpr auto prevSibling() const noexcept {
            return PrevSibling;
        }

        [[nodiscard]] constexpr auto nextSibling() const noexcept {
            return NextSibling;
        }

        [[nodiscard]] constexpr auto firstChild() const noexcept {
            return FirstChild;
        }

        [[nodiscard]] constexpr auto lastChild() const noexcept {
            return LastChild;
        }

        [[nodiscard]] constexpr auto leaf() const noexcept {
            return firstChild() == nullptr;
        }

        [[nodiscard]] constexpr auto hasOnlyOneChild() const noexcept {
            if (const auto FirstChild = firstChild()) {
                return FirstChild->nextSibling() == nullptr;
            }

            return false;
        }

        constexpr auto
        setParent(TreeNode *const Value) noexcept -> decltype(*this) {
            assert(Value != this);

            this->Parent = Value;
            return *this;
        }

        constexpr auto
        setPrevSibling(TreeNode *const Value) noexcept -> decltype(*this) {
            assert(Value != this);

            this->PrevSibling = Value;
            return *this;
        }

        constexpr auto
        setNextSibling(TreeNode *const Value) noexcept -> decltype(*this) {
            assert(Value != this);

            this->NextSibling = Value;
            return *this;
        }

        constexpr auto
        setFirstChild(TreeNode *const Value) noexcept -> decltype(*this) {
            assert(Value != this);

            this->FirstChild = Value;
            return *this;
        }

        constexpr auto
        setLastChild(TreeNode *const Value) noexcept -> decltype(*this) {
            assert(Value != this);

            this->LastChild = Value;
            return *this;
        }

        [[nodiscard]]
        constexpr static auto get(const TreeNode *const Node) noexcept {
            return Node;
        }

        [[nodiscard]]
        constexpr static auto get(TreeNode *const Node) noexcept {
            return Node;
        }

        template <typename T>
        constexpr auto forEachChild(const T &lambda) const noexcept {
            TREE_NODE_ITERATE_CHILDREN(*this, Child) {
                lambda(*Child);
            }
        }

        template <typename T>
        constexpr auto forSelfEachChild(const T &lambda) const noexcept {
            lambda(*this);
            forEachChild(lambda);
        }

        template <typename T>
        constexpr auto forSelfEachChild(const T &lambda) noexcept {
            lambda(*this);
            forEachChild(lambda);
        }

        template <typename T>
        constexpr auto forEachSibling(const T &lambda) const noexcept {
            TREE_NODE_ITERATE_SIBLINGS(*this, Sibling) {
                lambda(*Sibling);
            }
        }

        template <typename T>
        constexpr auto forSelfAndEachSibling(const T &lambda) const noexcept {
            lambda(*this);
            forEachSibling(lambda);
        }

        template <typename T>
        constexpr auto
        forEachSiblingTillEnd(const TreeNode *const End,
                            const T &lambda) const noexcept
        {
            TREE_NODE_ITERATE_SIBLINGS_TILL_END(*this, End, Sibling) {
                lambda(*Sibling);
            }
        }

        template <typename T>
        constexpr auto
        forSelfAndEachSiblingTillEnd(const TreeNode *const End,
                                    const T &lambda) const noexcept
        {
            lambda(*this);
            forEachSiblingTillEnd(End, lambda);
        }

        template <typename T>
        constexpr auto forEachChild(const T &lambda) noexcept {
            TREE_NODE_ITERATE_CHILDREN(*this, Child) {
                lambda(*Child);
            }
        }

        template <typename T>
        constexpr auto forSelfAndEachChild(const T &lambda) noexcept {
            lambda(*this);
            forEachChild(lambda);
        }

        template <typename T>
        constexpr auto forEachSibling(const T &lambda) noexcept {
            TREE_NODE_ITERATE_SIBLINGS(*nextSibling(), Sibling) {
                lambda(*Sibling);
            }
        }

        template <typename T>
        constexpr auto forSelfAndEachSibling(const T &lambda) noexcept {
            TREE_NODE_ITERATE_SIBLINGS(*this, Sibling) {
                lambda(*Sibling);
            }
        }

        template <typename T>
        constexpr auto
        forEachSiblingTillEnd(const TreeNode *const End,
                              const T &lambda) noexcept
        {
            TREE_NODE_ITERATE_SIBLINGS_TILL_END(*this, End, Sibling) {
                lambda(*Sibling);
            }
        }

        template <typename T>
        constexpr auto
        forSelfAndEachSiblingTillEnd(const TreeNode *const End,
                                     const T &lambda) noexcept
        {
            lambda(*this);
            forEachSiblingTillEnd(End, lambda);
        }

        constexpr static auto
        setParentOfSiblings(TreeNode *const Parent,
                            TreeNode &Node,
                            TreeNode *const End = nullptr) noexcept
        {
            auto LastSibling = &Node;
            for (auto Iter = &Node; Iter != End; Iter = Node.nextSibling()) {
                Iter->Parent = Parent;
                LastSibling = Iter;
            }

            return LastSibling;
        }

        constexpr auto
        setAsParentOfChildren(TreeNode &Node,
                              TreeNode *const End = nullptr) noexcept
        {
            auto LastSibling = &Node;
            for (auto Iter = &Node; Iter != End; Iter = Node.nextSibling()) {
                Iter->Parent = this;
                LastSibling = Iter;
            }

            return LastSibling;
        }

        inline auto addChild(TreeNode &Node) noexcept -> decltype(*this) {
            return addChildren(Node);
        }

        inline auto
        addChildren(TreeNode &Node, TreeNode *const End = nullptr) noexcept
            -> decltype(*this)
        {
            setParentOfSiblings(this, Node, End);

            if (const auto LastChild = lastChild()) {
                LastChild->NextSibling = &Node;
                Node.PrevSibling = LastChild;
            } else {
                this->FirstChild = &Node;
            }

            this->LastChild = &Node;
            Node.Parent = this;

            return *this;
        }

        constexpr auto addSibling(TreeNode &Node) noexcept -> decltype(*this) {
            return addSiblings(Node);
        }

        constexpr auto
        addSiblings(TreeNode &Node, TreeNode *const End = nullptr) noexcept
            -> decltype(*this)
        {
            if (const auto Parent = parent()) {
                auto LastSibling = setParentOfSiblings(Parent, Node, End);
                if (const auto LastChild = Parent->lastChild()) {
                    LastChild->NextSibling = &Node;
                    Node.PrevSibling = LastChild;
                } else {
                    Parent->FirstChild = &Node;
                }

                Parent->LastChild = LastSibling;
            }

            return *this;
        }

        [[nodiscard]]
        constexpr auto getChildCount() const noexcept -> uint64_t {
            auto Count = uint64_t();
            forEachChild([&Count]([[maybe_unused]] const auto Child) noexcept {
                Count++;
            });

            return Count;
        }

        [[nodiscard]] constexpr auto
        findNextSiblingForDFSIterator(
            const TreeNode *const End,
            uint64_t *const DepthChangeOut) const noexcept
            -> const TreeNode *
        {
            auto DepthChange = uint64_t();
            for (auto Node = this; Node != End; Node = Node->parent()) {
                if (const auto NextSibling = Node->nextSibling()) {
                    if (DepthChangeOut != nullptr) {
                        *DepthChangeOut = DepthChange;
                    }

                    return NextSibling;
                }

                DepthChange++;
            }

            if (DepthChangeOut != nullptr) {
                *DepthChangeOut = DepthChange;
            }

            return End;
        }

        [[nodiscard]] constexpr auto
        findNextNodeForDFSIterator(
            const TreeNode *const End = nullptr,
            int64_t *const DepthChangeOut = nullptr) const noexcept
            -> const TreeNode *
        {
            if (const auto FirstChild = firstChild()) {
                if (DepthChangeOut != nullptr) {
                    *DepthChangeOut = -1;
                }

                return FirstChild;
            }

            const auto Out = reinterpret_cast<uint64_t *>(DepthChangeOut);
            return findNextSiblingForDFSIterator(End, Out);
        }

        inline auto validateChildren() const noexcept -> decltype(*this) {
            if (leaf()) {
                return *this;
            }

            const auto FirstChild = firstChild();
            assert(FirstChild->parent() == this);

            const auto SecondChild = FirstChild->nextSibling();
            if (SecondChild == nullptr) {
                return *this;
            }

            auto Prev = FirstChild;
            auto Visited = std::set<TreeNode *>();

            Visited.insert(FirstChild);
            SecondChild->forSelfAndEachSibling([&](TreeNode &Sibling) {
                assert(!Visited.contains(&Sibling));
                assert(Sibling.parent() == this);
                assert(Sibling.prevSibling() == Prev);

                Prev = &Sibling;
                Visited.insert(&Sibling);
            });

            assert(Prev == lastChild());
            return *this;
        }

        [[nodiscard]]
        inline virtual auto getLength() const noexcept -> uint64_t {
            return 0;
        }

        template <typename NodePrinter>
        const TreeNode &
        PrintHorizontal(FILE *const OutFile,
                        const int TabLength,
                        const NodePrinter &NodePrinterFunc) noexcept
        {
            const auto RootDepthLevel = uint64_t(1);
            if (NodePrinterFunc(OutFile, 0, RootDepthLevel, *this)) {
                fputc('\n', OutFile);
            }

            auto Iter = TreeDFSIterator<const TreeNode>(this);
            for (Iter++; !Iter.isAtEnd(); Iter++) {
                auto WrittenOut = int();

                const auto &Info = *Iter;
                const auto DepthLevel = Iter.depthLevel();
                const auto End = DepthLevel - RootDepthLevel;

                for (auto I = RootDepthLevel; I != End; I++) {
                    if (Iter.getParentAtIndex(I)->nextSibling() != nullptr) {
                        fputs("│", OutFile);

                        WrittenOut += 1;
                        WrittenOut += Utils::PadSpaces(OutFile, TabLength - 1);
                    } else {
                        WrittenOut += Utils::PadSpaces(OutFile, TabLength);
                    }
                }

                WrittenOut += 1;
                if (Iter->nextSibling() != nullptr) {
                    fputs("├", OutFile);
                } else {
                    fputs("└", OutFile);
                }

                // Subtract 1 for the ├ or └ character, and 1 for the space in
                // between the "----" and the node-printer's string.

                const auto DashCount = (TabLength - 2);
                WrittenOut += DashCount;

                Utils::PrintMultTimes(OutFile,
                                      "─",
                                      static_cast<uint64_t>(DashCount));
                fputc(' ', OutFile);

                WrittenOut += 1;

                NodePrinterFunc(OutFile, WrittenOut, DepthLevel, Info);
                fputc('\n', OutFile);
            }

            return *this;
        }
    };

    static_assert(std::bidirectional_iterator<TreeDFSIterator<TreeNode>>);

    struct Tree {
    protected:
        TreeNode *Root = nullptr;
    public:
        constexpr Tree() noexcept = default;
        constexpr Tree(TreeNode *const Root) noexcept : Root(Root) {}

        [[nodiscard]] constexpr auto root() const noexcept {
            return Root;
        }

        constexpr
        auto setRoot(TreeNode *const Root) noexcept -> decltype(*this) {
            this->Root = Root;
            return *this;
        }

        [[nodiscard]] constexpr auto empty() const noexcept {
            return root() == nullptr;
        }

        struct DFS {
        protected:
            TreeNode *Root = nullptr;
        public:
            constexpr DFS() noexcept = default;
            constexpr DFS(TreeNode *const Root) noexcept : Root(Root) {}

            [[nodiscard]] constexpr auto root() const noexcept {
                return Root;
            }

            constexpr
            auto setRoot(TreeNode *const Root) noexcept -> decltype(*this) {
                this->Root = Root;
                return *this;
            }

            [[nodiscard]] constexpr auto empty() const noexcept {
                return root() == nullptr;
            }

            using Iterator = TreeDFSIterator<TreeNode>;

            [[nodiscard]] constexpr auto begin() const noexcept {
                return Iterator(Root);
            }

            [[nodiscard]] constexpr auto end() const noexcept {
                return Iterator(nullptr, nullptr);
            }

            template <typename T>
            constexpr auto forEach(const T &Lambda) const noexcept
                -> decltype(*this)
            {
                for (const auto &Info : *this) {
                    Lambda(Info);
                }

                return *this;
            }
        };

        [[nodiscard]] constexpr auto dfs() const noexcept {
            return DFS(Root);
        }

        template <typename Comparator>
        constexpr auto sort(const Comparator &Compare) noexcept
            -> decltype(*this)
        {
            const auto Swap = [](TreeNode &Lhs, TreeNode &Rhs) noexcept {
                const auto LPrevSibling = Lhs.prevSibling();
                const auto LNextSibling = Lhs.nextSibling();

                const auto RPrevSibling = Rhs.prevSibling();
                const auto RNextSibling = Rhs.nextSibling();

                if (LNextSibling != &Rhs) {
                    Lhs.setPrevSibling(RPrevSibling);
                    Rhs.setNextSibling(LNextSibling);

                    if (LNextSibling != nullptr) {
                        LNextSibling->setPrevSibling(&Rhs);
                    }

                    if (RPrevSibling != nullptr) {
                        RPrevSibling->setNextSibling(&Lhs);
                    }
                } else {
                    Rhs.setNextSibling(&Lhs);
                    Lhs.setPrevSibling(&Rhs);
                }

                Rhs.setPrevSibling(LPrevSibling);
                Lhs.setNextSibling(RNextSibling);

                if (const auto Parent = Lhs.parent()) {
                    if (Parent->firstChild() == &Lhs) {
                        Parent->setFirstChild(&Rhs);
                    }
                }

                if (LPrevSibling != nullptr) {
                    LPrevSibling->setNextSibling(&Rhs);
                }

                if (RNextSibling != nullptr) {
                    RNextSibling->setPrevSibling(&Lhs);
                }
            };

            for (const auto &Node : dfs()) {
                if (Node.leaf()) {
                    continue;
                }

                auto FirstChild = Node.firstChild();
                for (auto IChild = FirstChild->nextSibling();
                     IChild != nullptr;
                     IChild = IChild->nextSibling())
                {
                    for (auto JChild = FirstChild;
                         JChild != IChild;
                         JChild = JChild->nextSibling())
                    {
                        auto &LeftRef = *JChild;
                        auto &RightRef = *IChild;

                        if (Compare(LeftRef, RightRef) > 0) {
                            Swap(LeftRef, RightRef);
                            if (&LeftRef == FirstChild) {
                                FirstChild = Node.firstChild();
                            }

                            std::swap(IChild, JChild);
                        }
                    }
                }
            }

            return *this;
        }

        constexpr
        auto removeNode(TreeNode &Node, const bool RemoveParentLeafs) noexcept
            -> TreeNode *
        {
            const auto IsolateNode = [](TreeNode &Node) noexcept {
                // Fix the pointers of the sibling nodes to not point to this
                // node.

                auto ParentOnlyHasThisNode = false;
                if (const auto PrevSibling = Node.prevSibling()) {
                    if (const auto NextSibling = Node.nextSibling()) {
                        PrevSibling->setNextSibling(NextSibling);
                        NextSibling->setPrevSibling(Node.prevSibling());
                    } else {
                        PrevSibling->setNextSibling(nullptr);
                    }
                } else if (const auto NextSibling = Node.nextSibling()) {
                    NextSibling->setPrevSibling(nullptr);
                } else {
                    ParentOnlyHasThisNode = true;
                }

                // We try to merge Node's children with Node's siblings.

                const auto ParentPtr = Node.parent();
                if (const auto FirstChild = Node.firstChild()) {
                    if (const auto PrevSibling = Node.prevSibling()) {
                        if (ParentPtr != nullptr) {
                            TreeNode::setParentOfSiblings(ParentPtr,
                                                          *FirstChild);
                        }

                        PrevSibling->addSiblings(*FirstChild);
                    } else if (const auto NextSibling = Node.nextSibling()) {
                        if (ParentPtr != nullptr) {
                            ParentPtr->setFirstChild(FirstChild);
                            TreeNode::setParentOfSiblings(ParentPtr,
                                                          *FirstChild);
                        }

                        const auto LastChild = Node.lastChild();
                        LastChild->addSiblings(*NextSibling);
                    } else if (ParentPtr != nullptr) {
                        ParentPtr->setFirstChild(nullptr);
                        ParentPtr->setLastChild(nullptr);

                        ParentPtr->addChildren(*FirstChild);
                    }
                }

                if (ParentPtr == nullptr) {
                    return;
                }

                auto &Parent = *ParentPtr;
                if (!ParentOnlyHasThisNode) {
                    if (Parent.firstChild() == &Node) {
                        Parent.setFirstChild(Node.nextSibling());
                    }
                } else if (Node.leaf()) {
                    // If Node is a leaf, and ParentHasOnlyThisNode is true,
                    // then Parent will end up a leaf-node.

                    Parent.setFirstChild(nullptr);
                    Parent.setLastChild(nullptr);
                }
            };

            const auto DoRemoveLeafParents =
                [&](TreeNode &LeafParent, TreeNode &Root) noexcept
            {
                auto Child = &LeafParent;
                auto Parent = LeafParent.parent();
                auto RemovedRoot = false;

                do {
                    IsolateNode(*Child);
                    Child->clearAndDestroy();

                    if (Parent == nullptr || !Parent->leaf()) {
                        RemovedRoot = (Child == &Root);
                        break;
                    }

                    Child = Parent;
                    Parent = Child->parent();
                } while (true);

                return RemovedRoot;
            };

            const auto IsolateNodeAndRemoveFromParent =
                [&](TreeNode &Node,
                    TreeNode &Root,
                    const bool RemoveLeafParents) noexcept
            {
                IsolateNode(Node);

                auto RemovedRoot = false;
                if (const auto Parent = Node.parent();
                    Parent != nullptr && Parent->leaf() && RemoveLeafParents)
                {
                    RemovedRoot = DoRemoveLeafParents(*Parent, Root);
                }

                // IsolateNode() has already removed this Node from Parent.

                Node.clearAndDestroy();
                return RemovedRoot;
            };

            if (&Node == Root) {
                if (Node.leaf()) {
                    setRoot(nullptr);

                    IsolateNode(Node);
                    Node.clearAndDestroy();

                    return nullptr;
                }

                if (Node.hasOnlyOneChild()) {
                    setRoot(Node.firstChild());

                    IsolateNode(Node);
                    Node.clearAndDestroy();

                    return root();
                }

                setRoot(Node.createNew());
                root()->setAsParentOfChildren(*Node.firstChild());

                IsolateNode(Node);
                Node.clearAndDestroy();

                return nullptr;
            }

            const auto NextNode =
                const_cast<TreeNode *>(Node.findNextNodeForDFSIterator());

            if (IsolateNodeAndRemoveFromParent(Node,
                                               *Root,
                                               RemoveParentLeafs))
            {
                setRoot(nullptr);
            }

            return NextNode;
        }

        constexpr auto validateNodes() const noexcept -> decltype(*this) {
            dfs().forEach([](const TreeNode &Node) noexcept {
                Node.validateChildren();
            });

            return *this;
        }

        template <typename NodePrinter>
        auto
        PrintHorizontal(FILE *const OutFile,
                        const int TabLength,
                        const NodePrinter &NodePrinterFunc) const noexcept
            -> decltype(*this)
        {
            if (const auto Root = root()) {
                Root->PrintHorizontal<NodePrinter>(OutFile,
                                                   TabLength,
                                                   NodePrinterFunc);
            }

            return *this;
        }
    };
}