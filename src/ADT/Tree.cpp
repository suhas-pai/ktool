//
//  src/ADT/Tree.cpp
//  ktool
//
//  Created by Suhas Pai on 6/8/20.
//  Copyright © 2020 Suhas Pai. All rights reserved.
//

#include <algorithm>

#include "Utils/PrintUtils.h"
#include "Tree.h"

BasicTreeNode &BasicTreeNode::SetAsParentOfChildren() noexcept {
    return SetAsParentOfChildren(*this);
}

BasicTreeNode &
BasicTreeNode::SetAsParentOfChildren(BasicTreeNode &Node) noexcept {
    BASIC_TREE_NODE_ITERATE_CHILDREN(Node) {
        Child->setParent(&Node);
    }

    return *this;
}

uint64_t BasicTreeNode::GetChildCount() const noexcept {
    uint64_t ChildCount = 0;
    BASIC_TREE_NODE_ITERATE_CHILDREN(*this) {
        (void)Child;
        ChildCount++;
    }

    return ChildCount;
}

static BasicTreeNode &
CalculateLongestNode(BasicTreeNode &Node,
                     BasicTreeNode *End = nullptr) noexcept
{
    auto Longest = &Node;
    const auto NextSibling = Node.getNextSibling();

    if (NextSibling == nullptr) {
        return Node;
    }

    BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(*NextSibling, End) {
        if (Longest->GetLength() < Sibling->GetLength()) {
            Longest = Sibling;
        }
    }

    return *Longest;
}

BasicTreeNode *BasicTreeNode::getLastChild() const noexcept {
    auto Back = FirstChild;
    BASIC_TREE_NODE_ITERATE_SIBLINGS(*Back) {
        Back = Sibling;
    }

    return Back;
}

static BasicTreeNode &
SetParentOfSiblings(BasicTreeNode *Parent,
                    BasicTreeNode &Node,
                    BasicTreeNode *End = nullptr) noexcept
{
    auto Back = &Node;
    BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(*Back, End) {
        Sibling->setParent(Parent);
        Back = Sibling;
    }

    return *Back;
}

void BasicTreeNode::ValidateChildArray() const noexcept {
    if (this->isLeaf()) {
        return;
    }

    const auto SecondChild = getFirstChild()->getNextSibling();
    if (SecondChild == nullptr) {
        return;
    }

    auto Prev = FirstChild;
    BASIC_TREE_NODE_ITERATE_SIBLINGS(*SecondChild) {
        assert(Sibling->getPrevSibling() == Prev);
        Prev = Sibling;
    }
}

static void
SetIfLongestChild(BasicTreeNode &Parent, BasicTreeNode &Child) noexcept {
    if (Parent.getLongestChild()->GetLength() < Child.GetLength()) {
        Parent.setLongestChild(&Child);
    }
}

static void
AddChildrenRaw(BasicTreeNode &Parent,
               BasicTreeNode &Node,
               BasicTreeNode *End,
               BasicTreeNode &LongestChild) noexcept
{
    auto &Back = SetParentOfSiblings(&Parent, Node, End);
    Back.setNextSibling(nullptr);

    if (Parent.isLeaf()) {
        Node.setPrevSibling(nullptr);

        Parent.setFirstChild(&Node);
        Parent.setLongestChild(&LongestChild);
    } else {
        const auto PrevSibling = Parent.getLastChild();

        PrevSibling->setNextSibling(&Node);
        Node.setPrevSibling(PrevSibling);

        SetIfLongestChild(Parent, LongestChild);
    }
}

BasicTreeNode &BasicTreeNode::AddChild(BasicTreeNode &Node) noexcept {
    AddChildrenRaw(*this, Node, nullptr, Node);
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddChildren(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    AddChildrenRaw(*this, Node, End, CalculateLongestNode(Node));
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddSiblings(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    auto &Back = SetParentOfSiblings(Parent, Node, End);
    const auto NextSibling = getNextSibling();

    Node.setPrevSibling(this);
    Back.setNextSibling(NextSibling);

    if (NextSibling != nullptr) {
        NextSibling->setPrevSibling(&Back);
    }

    setNextSibling(&Node);

    const auto Parent = getParent();
    if (Parent == nullptr) {
        return *this;
    }

    auto &NewLongestChild = CalculateLongestNode(Node, End);
    SetIfLongestChild(*Parent, NewLongestChild);

    return *this;
}

BasicTreeNode &
BasicTreeNode::SetChildrenFromList(
    const std::vector<BasicTreeNode *> &List) noexcept
{
    if (List.empty()) {
        return *this;
    }

    const auto End = List.cend();
    for (auto Iter = List.cbegin() + 1; Iter != End; Iter++) {
        AddChild(**Iter);
    }

    return *this;
}

BasicTree::BasicTree(BasicTreeNode *Root) noexcept : Root(Root) {}
BasicTree::~BasicTree() noexcept {}

uint64_t BasicTree::GetCount() const noexcept {
    auto Count = uint64_t();
    for (auto &Iter : *this) {
        (void)Iter;
        Count++;
    }

    return Count;
}

const BasicTreeNode *
BasicTreeNode::FindPrevNodeForIterator(const BasicTreeNode *End,
                                       uint64_t *DepthChangeOut) const noexcept
{
    auto DepthChange = uint64_t();
    for (auto Node = this; Node != End; Node = Node->getParent()) {
        if (const auto PrevSibling = Node->getPrevSibling()) {
            if (DepthChangeOut != nullptr) {
                *DepthChangeOut = DepthChange;
            }

            return PrevSibling;
        }

        DepthChange++;
    }

    return End;
}

const BasicTreeNode *
BasicTreeNode::FindNextSiblingForIterator(
    const BasicTreeNode *End,
    uint64_t *DepthChangeOut) const noexcept
{
    auto DepthChange = uint64_t();
    for (auto Node = this; Node != End; Node = Node->getParent()) {
        if (const auto NextSibling = Node->getNextSibling()) {
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

const BasicTreeNode *
BasicTreeNode::FindNextNodeForIterator(const BasicTreeNode *End,
                                       int64_t *DepthChangeOut) const noexcept
{
    if (const auto FirstChild = getFirstChild()) {
        if (DepthChangeOut != nullptr) {
            *DepthChangeOut = -1;
        }

        return FirstChild;
    }

    const auto Out = reinterpret_cast<uint64_t *>(DepthChangeOut);
    return FindNextSiblingForIterator(End, Out);
}

static void ClearNode(BasicTreeNode &Node) noexcept {
    Node.setParent(nullptr);
    Node.setFirstChild(nullptr);
    Node.setPrevSibling(nullptr);
    Node.setNextSibling(nullptr);
    Node.setLongestChild(nullptr);

    delete &Node;
}

static void IsolateNode(BasicTreeNode &Node) noexcept {
    auto ParentOnlyHasThisNode = false;
    if (const auto PrevSibling = Node.getPrevSibling()) {
        if (const auto NextSibling = Node.getNextSibling()) {
            PrevSibling->setNextSibling(Node.getNextSibling());
            NextSibling->setPrevSibling(Node.getPrevSibling());
        } else {
            PrevSibling->setNextSibling(nullptr);
        }
    } else if (const auto NextSibling = Node.getNextSibling()) {
        NextSibling->setPrevSibling(nullptr);
    } else {
        ParentOnlyHasThisNode = true;
    }

    const auto ParentPtr = Node.getParent();
    const auto FirstChild = Node.getFirstChild();

    // We try to merge Node's children with Node's siblings.

    if (FirstChild != nullptr)  {
        if (const auto PrevSibling = Node.getPrevSibling()) {
            if (ParentPtr != nullptr) {
                SetParentOfSiblings(ParentPtr, *FirstChild);
            }

            PrevSibling->AddSiblings(*FirstChild);
        } else if (const auto NextSibling = Node.getNextSibling()) {
            if (ParentPtr != nullptr) {
                ParentPtr->setFirstChild(FirstChild);
                SetParentOfSiblings(ParentPtr, *FirstChild);
            }

            const auto LastChild = Node.getLastChild();
            LastChild->AddSiblings(*NextSibling);
        } else if (ParentPtr != nullptr) {
            ParentPtr->setFirstChild(nullptr);
            ParentPtr->AddChildren(*FirstChild);
        }
    }

    if (ParentPtr == nullptr) {
        return;
    }

    auto &Parent = *ParentPtr;
    if (!ParentOnlyHasThisNode) {
        if (Parent.getFirstChild() == &Node) {
            Parent.setFirstChild(Node.getNextSibling());
        }

        if (Parent.getLongestChild() == &Node) {
            auto &FirstChild = *Parent.getFirstChild();
            Parent.setLongestChild(&CalculateLongestNode(FirstChild));
        }
    } else if (Node.isLeaf()) {
        // If Node is a leaf, and ParentHasOnlyThisNode is true, then Parent
        // will end up a leaf-node.

        Parent.setFirstChild(nullptr);
        Parent.setLongestChild(nullptr);
    }
}

[[nodiscard]] static bool
DoRemoveLeafParents(BasicTreeNode &LeafParent, BasicTreeNode &Root) noexcept {
    auto RemovedRoot = false;
    auto Parent = LeafParent.getParent();
    auto Child = &LeafParent;

    do {
        IsolateNode(*Child);
        ClearNode(*Child);

        RemovedRoot = (Child == &Root);
        if (!Parent || !Parent->isLeaf()) {
            break;
        }

        Child = Parent;
        Parent = Parent->getParent();
    } while (true);

    return RemovedRoot;
}

[[nodiscard]] static bool
IsolateNodeAndRemoveFromParent(BasicTreeNode &Node,
                               BasicTreeNode &Root,
                               bool RemoveLeafParents) noexcept
{
    IsolateNode(Node);

    auto RemovedRoot = false;
    if (const auto Parent = Node.getParent();
        Parent != nullptr && Parent->isLeaf() && RemoveLeafParents)
    {
        RemovedRoot = DoRemoveLeafParents(*Parent, Root);
    }

    // BasicIsolate() has already removed this Node from Parent.

    ClearNode(Node);
    return RemovedRoot;
}

BasicTreeNode *
BasicTree::RemoveNode(BasicTreeNode &Node, bool RemoveParentLeafs) noexcept {
    if (&Node == Root) {
        if (Node.isLeaf()) {
            setRoot(nullptr);

            IsolateNode(Node);
            Node.clearAndDestroy();

            return nullptr;
        }

        if (Node.hasOnlyOneChild()) {
            setRoot(Node.getFirstChild());

            IsolateNode(Node);
            Node.clearAndDestroy();

            return getRoot();
        }

        setRoot(Node.createNew());
        getRoot()->SetAsParentOfChildren(*Node.getFirstChild());

        IsolateNode(Node);
        Node.clearAndDestroy();

        return nullptr;
    }

    const auto NextNode =
        const_cast<BasicTreeNode *>(Node.FindNextNodeForIterator());

    if (IsolateNodeAndRemoveFromParent(Node, *Root, RemoveParentLeafs)) {
        setRoot(nullptr);
    }

    return NextNode;
}

void BasicTreeNode::clearAndDestroy() noexcept {
    ClearNode(*this);
}
