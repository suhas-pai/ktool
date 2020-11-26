//
//  include/ADT/Tree.h
//  ktool
//
//  Created by Suhas Pai on 6/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#pragma once

#include <vector>
#include "Utils/PrintUtils.h"

struct BasicTreeNode;
struct Tree;

template <typename T>
struct TreeIterator {
protected:
    T *Current;
    T *End;

    uint64_t DepthLevel = 1;
public:
    TreeIterator(T *Current, T *End = nullptr) noexcept
    : Current(Current), End(End) {}

    inline T *& operator*() noexcept { return Current; }
    inline const T *const &operator*() const noexcept { return Current; }

    inline T *operator->() noexcept { return Current; }
    inline const T *operator->() const noexcept { return Current; }

    [[nodiscard]] inline T *getNode() noexcept { return Current; }
    [[nodiscard]] inline const T *getNode() const noexcept { return Current; }
    [[nodiscard]] inline const T *getConstNode() const noexcept {
        return Current;
    }

    [[nodiscard]] inline uint64_t getDepthLevel() const noexcept {
        return DepthLevel;
    }

    [[nodiscard]]
    inline uint64_t getPrintLineLength(uint64_t TabLength) noexcept {
        return (TabLength * (getDepthLevel() - 1));
    }

    [[nodiscard]] inline bool isAtEnd() const noexcept {
        return (Current == End);
    }

    [[nodiscard]] T *GetParentAtIndex(uint64_t DepthIndex) const noexcept {
        auto Parent = Current;

        const auto ThisDepthIndex = DepthLevel - 1;
        const auto MoveCount = ThisDepthIndex - DepthIndex;

        for (auto I = uint64_t(); I != MoveCount; I++) {
            Parent = T::get(Parent->getParent());
        }

        return Parent;
    }

    TreeIterator &operator--() noexcept {
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

    TreeIterator &operator++() noexcept {
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

    inline TreeIterator &operator++(int) noexcept {
        return ++(*this);
    }

    inline TreeIterator &operator+=(uint64_t Amt) noexcept {
        for (auto I = 0; I != Amt; I++) {
            ++(*this);
        }

        return *this;
    }

    inline TreeIterator &operator=(T *Node) noexcept {
        this->Current = Node;
        return *this;
    }

    [[nodiscard]]
    inline bool operator==(const TreeIterator &Iter) const noexcept {
        return (Current == Iter.Current);
    }

    [[nodiscard]]
    inline bool operator!=(const TreeIterator &Iter) const noexcept {
        return !(*this == Iter);
    }
};

template <typename T>
struct TreeNodeChildIterator {
protected:
    T *Current;
    T *End = nullptr;
public:
    TreeNodeChildIterator(T *Current, T *End = nullptr) noexcept
    : Current(Current), End(End) {}

    inline T *& operator*() noexcept { return Current; }
    inline const T *const &operator*() const noexcept { return Current; }

    inline T *operator->() noexcept { return Current; }
    inline const T *operator->() const noexcept { return Current; }

    [[nodiscard]] inline T *getNode() noexcept { return Current; }
    [[nodiscard]] inline const T *getNode() const noexcept { return Current; }
    [[nodiscard]] inline const T *getConstNode() const noexcept {
        return Current;
    }

    [[nodiscard]] inline bool isAtEnd() const noexcept {
        return (Current == End);
    }

    TreeNodeChildIterator &operator--() noexcept {
        if (const auto PrevSibling = Current->getPrevSibling()) {
            Current = reinterpret_cast<T *>(PrevSibling);
        } else {
            Current = End;
        }

        return *this;
    }

    TreeNodeChildIterator &operator++() noexcept {
        if (const auto NextSibling = Current->getNextSibling()) {
            Current = reinterpret_cast<T *>(NextSibling);
            return *this;
        }

        return *this;
    }

    inline TreeNodeChildIterator &operator++(int) noexcept {
        return ++(*this);
    }

    inline TreeNodeChildIterator &operator+=(uint64_t Amt) noexcept {
        for (auto I = 0; I != Amt; I++) {
            ++(*this);
        }

        return *this;
    }

    inline TreeNodeChildIterator &operator=(T *Node) noexcept {
        this->Current = Node;
        return *this;
    }

    [[nodiscard]]
    inline bool operator==(const TreeNodeChildIterator &Iter) const noexcept {
        return (Current == Iter.Current);
    }

    [[nodiscard]]
    inline bool operator!=(const TreeNodeChildIterator &Iter) const noexcept {
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

struct BasicTreeNode {
    friend struct BasicTree;
protected:
    BasicTreeNode *Parent = nullptr;
    BasicTreeNode *FirstChild = nullptr;
    BasicTreeNode *PrevSibling = nullptr;
    BasicTreeNode *NextSibling = nullptr;
    BasicTreeNode *LastChild = nullptr;

    void clearAndDestroy() noexcept;
public:
    BasicTreeNode() noexcept = default;

    [[nodiscard]] virtual BasicTreeNode *createNew() const noexcept = 0;
    virtual ~BasicTreeNode() noexcept = default;

    [[nodiscard]] inline BasicTreeNode *getParent() const noexcept {
        return Parent;
    }

    inline BasicTreeNode &setParent(BasicTreeNode *Parent) noexcept {
        this->Parent = Parent;
        return *this;
    }

    [[nodiscard]] inline BasicTreeNode *getFirstChild() const noexcept {
        return FirstChild;
    }

    [[nodiscard]] inline BasicTreeNode *getLastChild() const noexcept {
        return LastChild;
    }

    [[nodiscard]] inline BasicTreeNode *getPrevSibling() const noexcept {
        return PrevSibling;
    }

    [[nodiscard]] inline BasicTreeNode *getNextSibling() const noexcept {
        return NextSibling;
    }

    [[nodiscard]]
    static inline BasicTreeNode *get(BasicTreeNode *Node) noexcept {
        return Node;
    }

    [[nodiscard]] static
    inline const BasicTreeNode *get(const BasicTreeNode *Node) noexcept {
        return Node;
    }

    [[nodiscard]] uint64_t GetChildCount() const noexcept;

    BasicTreeNode &SetAsParentOfChildren() noexcept;
    BasicTreeNode &SetAsParentOfChildren(BasicTreeNode &Node) noexcept;

    void ValidateChildArray() const noexcept;
    BasicTreeNode &AddChild(BasicTreeNode &Node) noexcept;

    BasicTreeNode &
    AddChildren(BasicTreeNode &Node, BasicTreeNode *End = nullptr) noexcept;

    BasicTreeNode &AddSibling(BasicTreeNode &Node) noexcept;

    BasicTreeNode &
    AddSiblings(BasicTreeNode &Node, BasicTreeNode *End = nullptr) noexcept;

    [[nodiscard]] const BasicTreeNode *
    FindPrevNodeForIterator(const BasicTreeNode *End = nullptr,
                            uint64_t *DepthChangeOut = nullptr) const noexcept;

    [[nodiscard]] const BasicTreeNode *
    FindNextSiblingForIterator(
        const BasicTreeNode *End = nullptr,
        uint64_t *DepthChangeOut = nullptr) const noexcept;

    [[nodiscard]] const BasicTreeNode *
    FindNextNodeForIterator(const BasicTreeNode *End = nullptr,
                            int64_t *DepthChangeOut = nullptr) const noexcept;

    inline BasicTreeNode &setFirstChild(BasicTreeNode *Node) noexcept {
        assert(Node != this);

        this->FirstChild = Node;
        return *this;
    }

    inline BasicTreeNode &setPrevSibling(BasicTreeNode *Node) noexcept {
        assert(Node != this);

        this->PrevSibling = Node;
        return *this;
    }

    inline BasicTreeNode &setNextSibling(BasicTreeNode *Node) noexcept {
        assert(Node != this);

        this->NextSibling = Node;
        return *this;
    }

    inline BasicTreeNode &setLastChild(BasicTreeNode *Node) noexcept {
        assert(Node != this);

        this->LastChild = Node;
        return *this;
    }

    [[nodiscard]] inline bool isLeaf() const noexcept {
        return (getFirstChild() == nullptr);
    }

    [[nodiscard]] inline bool hasOnlyOneChild() const noexcept {
        if (const auto FirstChild = getFirstChild()) {
            return (FirstChild->getNextSibling() == nullptr);
        }

        return false;
    }

    [[nodiscard]] virtual uint64_t GetLength() const noexcept = 0;

    using Iterator = TreeIterator<const BasicTreeNode>;
    using ConstIterator = Iterator;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(this);
    }

    [[nodiscard]] inline Iterator end() const noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(this);
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    template <typename NodePrinter>
    const BasicTreeNode &
    PrintHorizontal(FILE *OutFile,
                    int TabLength,
                    const NodePrinter &NodePrinterFunc) const noexcept
    {
        const auto RootDepthLevel = 1;
        if (NodePrinterFunc(OutFile, 0, RootDepthLevel, *this)) {
            fputc('\n', OutFile);
        }

        auto Iter = TreeIterator<const BasicTreeNode>(this, getParent());
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

struct BasicTree {
protected:
    BasicTreeNode *Root;
public:
    BasicTree() noexcept = default;
    BasicTree(BasicTreeNode *Root) noexcept;

    virtual ~BasicTree() noexcept;

    using Iterator = TreeIterator<BasicTreeNode>;
    using ConstIterator = TreeIterator<const BasicTreeNode>;

    [[nodiscard]] inline Iterator begin() const noexcept {
        return Iterator(getRoot());
    }

    [[nodiscard]] inline Iterator end() const noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] inline ConstIterator cbegin() const noexcept {
        return ConstIterator(getRoot());
    }

    [[nodiscard]] inline ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] inline BasicTreeNode *getRoot() const noexcept {
        return Root;
    }

    inline BasicTree &setRoot(BasicTreeNode *Root) noexcept {
        this->Root = Root;
        return *this;
    }

    [[nodiscard]] inline bool empty() const noexcept {
        return (Root == nullptr);
    }

    template <typename T>
    [[nodiscard]] inline std::vector<T *> GetAsList() const noexcept {
        auto Result = std::vector<T *>();
        Result.reserve(GetCount());

        for (const auto &Iter : *this) {
            Result.emplace_back(reinterpret_cast<T *>(Iter));
        }

        return Result;
    }

    template <typename Comparator>
    BasicTree &Sort(const Comparator &ShouldSwap) noexcept {
        const auto Swap = [](BasicTreeNode &Lhs, BasicTreeNode &Rhs) noexcept {
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

        for (const auto &Node : *this) {
            if (Node->isLeaf()) {
                continue;
            }

            auto FirstChild = Node->getFirstChild();
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
                            FirstChild = Node->getFirstChild();
                        }

                        std::swap(IChild, JChild);
                    }
                }
            }
        }

        return *this;
    }

    BasicTreeNode *
    RemoveNode(BasicTreeNode &Node, bool RemoveParentLeafs) noexcept;

    void ValidateNodes() const noexcept;
    [[nodiscard]] virtual uint64_t GetCount() const noexcept;

    template <typename NodePrinter>
    const BasicTree &
    PrintHorizontal(FILE *OutFile,
                    int TabLength,
                    const NodePrinter &NodePrinterFunc) const noexcept
    {
        if (empty()) {
            return *this;
        }

        Root->PrintHorizontal(OutFile, TabLength, NodePrinterFunc);
        return *this;
    }

    template <typename NodePrinter>
    BasicTree &
    PrintHorizontal(FILE *OutFile,
                    int TabLength,
                    const NodePrinter &NodePrinterFunc) noexcept
    {
        const_cast<const BasicTreeNode &>(*Root).
            PrintHorizontal(OutFile, TabLength, NodePrinterFunc);

        return *this;
    }
};
