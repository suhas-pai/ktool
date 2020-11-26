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
    auto ChildCount = uint64_t();
    BASIC_TREE_NODE_ITERATE_CHILDREN(*this) {
        (void)Child;
        ChildCount++;
    }

    return ChildCount;
}

static BasicTreeNode &
SetParentOfSiblings(BasicTreeNode *Parent,
                    BasicTreeNode &Node,
                    BasicTreeNode *End = nullptr) noexcept
{
    auto Back = &Node;
    BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(Node, End) {
        Sibling->setParent(Parent);
        Back = Sibling;
    }

    return *Back;
}

void BasicTreeNode::ValidateChildArray() const noexcept {
    if (this->isLeaf()) {
        return;
    }

    const auto FirstChild = getFirstChild();
    assert(FirstChild->getParent() == this);

    const auto SecondChild = FirstChild->getNextSibling();
    if (SecondChild == nullptr) {
        return;
    }

    assert(SecondChild->getParent() == this);

    auto Prev = FirstChild;
    BASIC_TREE_NODE_ITERATE_SIBLINGS(*SecondChild) {
        assert(Sibling->getParent() == this);
        assert(Sibling->getPrevSibling() == Prev);

        Prev = Sibling;
    }

    assert(Prev == getLastChild());
}

static void
AddSiblingsRaw(BasicTreeNode &This,
               BasicTreeNode &Node,
               BasicTreeNode *End) noexcept
{
    const auto NextSibling = This.getNextSibling();
    const auto Parent = This.getParent();

    auto &Back = SetParentOfSiblings(Parent, Node, End);

    Node.setPrevSibling(&This);
    Back.setNextSibling(NextSibling);

    if (NextSibling != nullptr) {
        NextSibling->setPrevSibling(&Back);
    }

    This.setNextSibling(&Node);
    if (Parent != nullptr) {
        if (&Node == Parent->getLastChild()) {
            Parent->setLastChild(&Back);
        }
    }
}

static void
AddChildrenRaw(BasicTreeNode &Parent,
               BasicTreeNode &Node,
               BasicTreeNode *End) noexcept
{
    if (Parent.isLeaf()) {
        SetParentOfSiblings(&Parent, Node, End);
        Node.setPrevSibling(nullptr);

        Parent.setFirstChild(&Node);
        Parent.setLastChild(&Node);

        return;
    }

    AddSiblingsRaw(*Parent.getLastChild(), Node, End);
}

BasicTreeNode &BasicTreeNode::AddChild(BasicTreeNode &Node) noexcept {
    AddChildrenRaw(*this, Node, nullptr);
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddChildren(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    AddChildrenRaw(*this, Node, End);
    return *this;
}

BasicTreeNode &BasicTreeNode::AddSibling(BasicTreeNode &Node) noexcept {
    AddSiblingsRaw(*this, Node, nullptr);
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddSiblings(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    AddSiblingsRaw(*this, Node, End);
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
    Node.setLastChild(nullptr);

    delete &Node;
}

void BasicTreeNode::clearAndDestroy() noexcept {
    ClearNode(*this);
}

static void IsolateNode(BasicTreeNode &Node) noexcept {
    // Fix the pointers of the sibling nodes to not point to this node.

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

    // We try to merge Node's children with Node's siblings.

    const auto ParentPtr = Node.getParent();
    if (const auto FirstChild = Node.getFirstChild(); FirstChild != nullptr) {
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
            ParentPtr->setLastChild(nullptr);

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
    } else if (Node.isLeaf()) {
        // If Node is a leaf, and ParentHasOnlyThisNode is true, then Parent
        // will end up a leaf-node.

        Parent.setFirstChild(nullptr);
        Parent.setLastChild(nullptr);
    }
}

[[nodiscard]] static bool
DoRemoveLeafParents(BasicTreeNode &LeafParent, BasicTreeNode &Root) noexcept {
    auto Child = &LeafParent;
    auto Parent = LeafParent.getParent();
    auto RemovedRoot = false;

    do {
        IsolateNode(*Child);
        ClearNode(*Child);

        if (Parent == nullptr || !Parent->isLeaf()) {
            RemovedRoot = (Child == &Root);
            break;
        }

        Child = Parent;
        Parent = Child->getParent();
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

    // IsolateNode() has already removed this Node from Parent.

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

void BasicTree::ValidateNodes() const noexcept {
    for (const auto &Node : *this) {
        Node->ValidateChildArray();
    }
}
