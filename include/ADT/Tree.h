//
//  ADT/Tree.h
//  ktool
//
//  Created by Suhas Pai on 6/8/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "Utils/PrintUtils.h"

struct TreeNode;
struct Tree;

template <typename T>
struct TreeIterator {
protected:
    T *Current;
    T *End;

    uint64_t DepthLevel = 1;
public:
    constexpr explicit TreeIterator(T *const Current) noexcept
    : TreeIterator(Current, Current->getParent()) {}

    explicit
    TreeIterator(T *Current, T *End) noexcept : Current(Current), End(End) {}

    static inline auto Null() noexcept {
        return TreeIterator(nullptr, nullptr);
    }

    inline auto &operator*() noexcept { return Current; }
    inline auto &operator*() const noexcept { return Current; }

    inline auto operator->() noexcept { return Current; }
    inline auto operator->() const noexcept { return Current; }

    [[nodiscard]] inline auto getNode() noexcept { return Current; }
    [[nodiscard]] inline auto getNode() const noexcept { return Current; }
    [[nodiscard]] inline auto getConstNode() const noexcept {
        return Current;
    }

    [[nodiscard]] inline auto getDepthLevel() const noexcept {
        return DepthLevel;
    }

    [[nodiscard]] inline auto getPrintLineLength(uint64_t TabLength) noexcept {
        return TabLength * (this->getDepthLevel() - 1);
    }

    [[nodiscard]] inline auto isAtEnd() const noexcept {
        return Current == End;
    }

    [[nodiscard]] auto GetParentAtIndex(uint64_t DepthIndex) const noexcept {
        auto Parent = Current;

        const auto ThisDepthIndex = DepthLevel - 1;
        const auto MoveCount = ThisDepthIndex - DepthIndex;

        for (auto I = uint64_t(); I != MoveCount; I++) {
            Parent = T::get(Parent->getParent());
        }

        return Parent;
    }

    auto operator--() noexcept -> decltype(*this) {
        auto Node = Current;
        Current = nullptr;

        for (; DepthLevel != 0; DepthLevel--, Node = Node->getParent()) {
            if (const auto PrevSibling = Node->getPrevSibling()) {
                Current = PrevSibling->get(PrevSibling);
                break;
            }
        }

        return *this;
    }

    auto operator++() noexcept -> decltype(*this) {
        if (const auto FirstChild = Current->getFirstChild()) {
            Current = reinterpret_cast<T *>(FirstChild);
            DepthLevel += 1;

            return *this;
        }

        for (; Current != End; DepthLevel--, Current = Current->getParent()) {
            if (const auto NextSibling = Current->getNextSibling()) {
                Current = T::get(NextSibling);
                break;
            }
        }

        return *this;
    }

    inline auto operator++(int) noexcept -> decltype(*this) {
        return ++(*this);
    }

    inline auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
        for (auto I = 0; I != Amt; I++) {
            ++(*this);
        }

        return *this;
    }

    inline auto operator=(T *Node) noexcept -> decltype(*this) {
        this->Current = Node;
        return *this;
    }

    [[nodiscard]]
    inline auto operator==(const TreeIterator &Iter) const noexcept {
        return Current == Iter.Current;
    }

    [[nodiscard]]
    inline auto operator!=(const TreeIterator &Iter) const noexcept {
        return !(*this == Iter);
    }
};

template <typename T>
struct TreeNodeChildIterator {
protected:
    T *Current;
    T *End = nullptr;
public:
    constexpr explicit
    TreeNodeChildIterator(T *const Current, T *const End = nullptr) noexcept
    : Current(Current), End(End) {}

    inline auto &operator*() noexcept { return Current; }
    inline auto &operator*() const noexcept { return Current; }

    inline auto operator->() noexcept { return Current; }
    inline auto operator->() const noexcept { return Current; }

    [[nodiscard]] inline auto getNode() noexcept { return Current; }
    [[nodiscard]] inline auto getNode() const noexcept { return Current; }
    [[nodiscard]] inline auto getConstNode() const noexcept {
        return Current;
    }

    [[nodiscard]] inline auto isAtEnd() const noexcept {
        return Current == End;
    }

    auto operator--() noexcept -> decltype(*this) {
        if (const auto PrevSibling = Current->getPrevSibling()) {
            Current = reinterpret_cast<T *>(PrevSibling);
        } else {
            Current = End;
        }

        return *this;
    }

    auto operator++() noexcept -> decltype(*this) {
        if (const auto NextSibling = Current->getNextSibling()) {
            Current = reinterpret_cast<T *>(NextSibling);
            return *this;
        }

        return *this;
    }

    inline auto operator++(int) noexcept -> decltype(*this) {
        return ++(*this);
    }

    inline auto operator+=(uint64_t Amt) noexcept -> decltype(*this) {
        for (auto I = uint64_t(); I != Amt; I++) {
            ++(*this);
        }

        return *this;
    }

    inline auto operator=(T *Node) noexcept -> decltype(*this) {
        this->Current = Node;
        return *this;
    }

    [[nodiscard]]
    inline auto operator==(const TreeNodeChildIterator &Iter) const noexcept {
        return Current == Iter.Current;
    }

    [[nodiscard]]
    inline auto operator!=(const TreeNodeChildIterator &Iter) const noexcept {
        return !(*this == Iter);
    }
};

#define BASIC_TREE_NODE_ITERATE_CHILDREN(Node)                                \
    for (auto Child = (Node).get((Node).getFirstChild());                     \
         Child != nullptr;                                                    \
         Child = (Child)->get(Child->getNextSibling()))

#define BASIC_TREE_NODE_ITERATE_CHILDREN_TILL_END(Node, ENd)                  \
    for (auto Child = (Node).get((Node).getFirstChild());                     \
         Child != (End);                                                      \
         Child = (Child)->get(Child->getNextSibling()))

#define BASIC_TREE_NODE_ITERATE_CHILDREN_AFTER_FIRST(Node)                    \
    for (auto Child = (Node).get((Node).getFirstChild()->getNextSibling());   \
         Child != nullptr;                                                    \
         Child = (Child)->get(Child->getNextSibling()))

#define BASIC_TREE_NODE_ITERATE_SIBLINGS(Node)                                \
    for (auto Sibling = &(Node);                                              \
         Sibling != nullptr;                                                  \
         Sibling = (Sibling)->get(Sibling->getNextSibling()))

#define BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(Node, End)                  \
    for (auto Sibling = &(Node);                                              \
         Sibling != (End);                                                    \
         Sibling = (Sibling)->get(Sibling->getNextSibling()))

struct TreeNode {
    friend struct Tree;
protected:
    TreeNode *Parent = nullptr;
    TreeNode *FirstChild = nullptr;
    TreeNode *PrevSibling = nullptr;
    TreeNode *NextSibling = nullptr;
    TreeNode *LastChild = nullptr;

    void clearAndDestroy() noexcept;
public:
    TreeNode() noexcept = default;

    [[nodiscard]] virtual TreeNode *createNew() const noexcept = 0;
    virtual ~TreeNode() noexcept = default;

    [[nodiscard]] inline auto getParent() const noexcept {
        return Parent;
    }

    inline auto setParent(TreeNode *const Parent) noexcept
        -> decltype(*this)
    {
        this->Parent = Parent;
        return *this;
    }

    [[nodiscard]] inline auto getFirstChild() const noexcept {
        return this->FirstChild;
    }

    [[nodiscard]] inline auto getLastChild() const noexcept {
        return this->LastChild;
    }

    [[nodiscard]] inline auto getPrevSibling() const noexcept {
        return this->PrevSibling;
    }

    [[nodiscard]] inline auto getNextSibling() const noexcept {
        return this->NextSibling;
    }

    [[nodiscard]] static inline auto get(TreeNode *const Node) noexcept {
        return Node;
    }

    [[nodiscard]] static inline auto get(const TreeNode *const Node) noexcept {
        return Node;
    }

    [[nodiscard]] uint64_t GetChildCount() const noexcept;

    TreeNode &SetAsParentOfChildren() noexcept;
    TreeNode &SetAsParentOfChildren(TreeNode &Node) noexcept;

    void ValidateChildArray() const noexcept;
    TreeNode &AddChild(TreeNode &Node) noexcept;

    auto
    AddChildren(TreeNode &Node, TreeNode *End = nullptr) noexcept
        -> decltype(*this);

    auto AddSibling(TreeNode &Node) noexcept -> decltype(*this);

    auto
    AddSiblings(TreeNode &Node, TreeNode *End = nullptr) noexcept
        -> decltype(*this);

    [[nodiscard]] auto
    FindPrevNodeForIterator(const TreeNode *End = nullptr,
                            uint64_t *DepthChangeOut = nullptr) const noexcept
        -> const TreeNode *;

    [[nodiscard]] auto
    FindNextSiblingForIterator(
        const TreeNode *End = nullptr,
        uint64_t *DepthChangeOut = nullptr) const noexcept
            -> const TreeNode *;

    [[nodiscard]] auto
    FindNextNodeForIterator(const TreeNode *End = nullptr,
                            int64_t *DepthChangeOut = nullptr) const noexcept
        -> const TreeNode *;

    inline auto setFirstChild(TreeNode *const Node) noexcept -> decltype(*this)
    {
        assert(Node != this);

        this->FirstChild = Node;
        return *this;
    }

    inline auto setPrevSibling(TreeNode *const Node) noexcept -> decltype(*this)
    {
        assert(Node != this);

        this->PrevSibling = Node;
        return *this;
    }

    inline auto setNextSibling(TreeNode *const Node) noexcept
        -> decltype(*this)
    {
        assert(Node != this);

        this->NextSibling = Node;
        return *this;
    }

    inline auto setLastChild(TreeNode *const Node) noexcept
        -> decltype(*this)
    {
        assert(Node != this);

        this->LastChild = Node;
        return *this;
    }

    [[nodiscard]] inline auto isLeaf() const noexcept {
        return this->getFirstChild() == nullptr;
    }

    [[nodiscard]] inline auto hasOnlyOneChild() const noexcept {
        if (const auto FirstChild = this->getFirstChild()) {
            return FirstChild->getNextSibling() == nullptr;
        }

        return false;
    }

    [[nodiscard]] virtual uint64_t GetLength() const noexcept = 0;

    using Iterator = TreeIterator<const TreeNode>;
    using ConstIterator = Iterator;

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(this);
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(this);
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(nullptr);
    }

    template <typename T>
    inline void forEachChild(const T &lambda) const noexcept {
        BASIC_TREE_NODE_ITERATE_CHILDREN(*this) {
            lambda(*Child);
        }
    }

    template <typename T>
    inline void forSelfAndEachChild(const T &lambda) const noexcept {
        lambda(*this);
        forEachChild(lambda);
    }

    template <typename T>
    inline void forEachSibling(const T &lambda) const noexcept {
        BASIC_TREE_NODE_ITERATE_SIBLINGS(*this) {
            lambda(*Sibling);
        }
    }

    template <typename T>
    inline void forSelfAndEachSibling(const T &lambda) const noexcept {
        lambda(*this);
        forEachSibling(lambda);
    }

    template <typename T>
    inline void
    forEachSiblingTillEnd(const TreeNode *const End,
                          const T &lambda) const noexcept
    {
        BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(*this, End) {
            lambda(*Sibling);
        }
    }

    template <typename T>
    inline void
    forSelfAndEachSiblingTillEnd(const TreeNode *const End,
                                 const T &lambda) const noexcept
    {
        lambda(*this);
        forEachSiblingTillEnd(End, lambda);
    }

    template <typename T>
    inline void forEachChild(const T &lambda) noexcept {
        BASIC_TREE_NODE_ITERATE_CHILDREN(*this) {
            lambda(*Child);
        }
    }

    template <typename T>
    inline void forSelfAndEachChild(const T &lambda) noexcept {
        lambda(*this);
        forEachChild(lambda);
    }

    template <typename T>
    inline void forEachSibling(const T &lambda) noexcept {
        BASIC_TREE_NODE_ITERATE_SIBLINGS(*this) {
            lambda(*Sibling);
        }
    }

    template <typename T>
    inline void forSelfAndEachSibling(const T &lambda) noexcept {
        lambda(*this);
        forEachSibling(lambda);
    }

    template <typename T>
    inline void
    forEachSiblingTillEnd(const TreeNode *const End, const T &lambda) noexcept {
        BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(*this, End) {
            lambda(*Sibling);
        }
    }

    template <typename T>
    inline void
    forSelfAndEachSiblingTillEnd(const TreeNode *const End,
                                 const T &lambda) noexcept
    {
        lambda(*this);
        forEachSiblingTillEnd(End, lambda);
    }

    template <typename NodePrinter>
    auto
    PrintHorizontal(FILE *const OutFile,
                    const int TabLength,
                    const NodePrinter &NodePrinterFunc) const noexcept
        -> decltype(*this)
    {
        const auto RootDepthLevel = static_cast<uint64_t>(1);
        if (NodePrinterFunc(OutFile, 0, RootDepthLevel, *this)) {
            fputc('\n', OutFile);
        }

        auto Iter = TreeIterator<const TreeNode>(this);
        for (Iter++; !Iter.isAtEnd(); Iter++) {
            auto WrittenOut = int();

            const auto &Info = *Iter;
            const auto DepthLevel = Iter.getDepthLevel();
            const auto End = DepthLevel - RootDepthLevel;

            for (auto I = RootDepthLevel; I != End; I++) {
                if (Iter.GetParentAtIndex(I)->getNextSibling() != nullptr) {
                    fputs("│", OutFile);

                    WrittenOut += 1;
                    WrittenOut += PrintUtilsPadSpaces(OutFile, TabLength - 1);
                } else {
                    WrittenOut += PrintUtilsPadSpaces(OutFile, TabLength);
                }
            }

            WrittenOut += 1;
            if (Iter->getNextSibling() != nullptr) {
                fputs("├", OutFile);
            } else {
                fputs("└", OutFile);
            }

            // Subtract 1 for the ├ or └ character, and 1 for the space in
            // between the "----" and the node-printer's string.

            const auto DashCount = (TabLength - 2);
            WrittenOut += DashCount;

            PrintUtilsStringMultTimes(OutFile, "─", DashCount);
            fputc(' ', OutFile);

            WrittenOut += 1;

            NodePrinterFunc(OutFile, WrittenOut, DepthLevel, *Info);
            fputc('\n', OutFile);
        }

        return *this;
    }
};

struct Tree {
protected:
    TreeNode *Root;
public:
    Tree() noexcept = default;
    Tree(TreeNode *Root) noexcept;

    virtual ~Tree() noexcept;

    [[nodiscard]] inline auto getRoot() const noexcept {
        return Root;
    }

    using Iterator = TreeIterator<TreeNode>;
    using ConstIterator = TreeIterator<const TreeNode>;

    [[nodiscard]] inline auto begin() const noexcept {
        return Iterator(this->getRoot());
    }

    [[nodiscard]] inline auto end() const noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] inline auto cbegin() const noexcept {
        return ConstIterator(this->getRoot());
    }

    [[nodiscard]] inline auto cend() const noexcept {
        return ConstIterator(nullptr);
    }

    inline auto setRoot(TreeNode *const Root) noexcept -> decltype(*this) {
        this->Root = Root;
        return *this;
    }

    [[nodiscard]] inline auto empty() const noexcept {
        return this->getRoot() == nullptr;
    }

    template <typename T>
    inline void forEachNode(const T &lambda) const noexcept {
        if (const auto Root = this->getRoot()) {
            Root->forSelfAndEachChild(lambda);
        }
    }

    template <typename T>
    [[nodiscard]] inline auto GetAsList() const noexcept {
        auto Result = std::vector<T *>();
        this->forEachNode([&](const auto &Iter) noexcept {
            Result.emplace_back(reinterpret_cast<T *>(&Iter));
        });

        return Result;
    }

    template <typename Comparator>
    auto Sort(const Comparator &ShouldSwap) noexcept -> decltype(*this) {
        const auto Swap = [](TreeNode &Lhs, TreeNode &Rhs) noexcept {
            const auto LPrevSibling = Lhs.getPrevSibling();
            const auto LNextSibling = Lhs.getNextSibling();

            const auto RPrevSibling = Rhs.getPrevSibling();
            const auto RNextSibling = Rhs.getNextSibling();

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

            if (const auto Parent = Lhs.getParent()) {
                if (Parent->getFirstChild() == &Lhs) {
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

        this->forEachNode([&](const auto &Node) noexcept {
            if (Node.isLeaf()) {
                return;
            }

            auto FirstChild = Node.getFirstChild();
            for (auto IChild = FirstChild->getNextSibling();
                 IChild != nullptr;
                 IChild = IChild->getNextSibling())
            {
                for (auto JChild = FirstChild;
                     JChild != IChild;
                     JChild = JChild->getNextSibling())
                {
                    auto &LeftRef = *JChild;
                    auto &RightRef = *IChild;

                    if (ShouldSwap(LeftRef, RightRef)) {
                        Swap(LeftRef, RightRef);
                        if (&LeftRef == FirstChild) {
                            FirstChild = Node.getFirstChild();
                        }

                        std::swap(IChild, JChild);
                    }
                }
            }
        });

        return *this;
    }

    auto RemoveNode(TreeNode &Node, bool RemoveParentLeafs) noexcept
        -> TreeNode *;

    void ValidateNodes() const noexcept;
    [[nodiscard]] virtual uint64_t GetCount() const noexcept;

    template <typename NodePrinter>
    auto
    PrintHorizontal(FILE *const OutFile,
                    const int TabLength,
                    const NodePrinter &NodePrinterFunc) const noexcept
        -> decltype(*this)
    {
        if (!this->empty()) {
            Root->PrintHorizontal(OutFile, TabLength, NodePrinterFunc);
        }

        return *this;
    }

    template <typename NodePrinter>
    auto
    PrintHorizontal(FILE *const OutFile,
                    const int TabLength,
                    const NodePrinter &NodePrinterFunc) noexcept
        -> decltype(*this)
    {
        const_cast<const TreeNode &>(*Root).
            PrintHorizontal(OutFile, TabLength, NodePrinterFunc);

        return *this;
    }
};
