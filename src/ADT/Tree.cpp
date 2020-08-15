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

BasicTreeNode &BasicTreeNode::SetParentOfChildren() noexcept {
    return SetParentOfChildren(*this);
}

BasicTreeNode &
BasicTreeNode::SetParentOfChildren(BasicTreeNode &Node) noexcept {
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

static inline void RecalculateLongestChild(BasicTreeNode &Node) noexcept {
    Node.setLongestChild(&CalculateLongestNode(Node));
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
    const auto FirstChild = getFirstChild();
    if (FirstChild == nullptr) {
        return;
    }

    const auto SecondChild = FirstChild->getNextSibling();
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
AddChildrenRaw(BasicTreeNode &This,
               BasicTreeNode &Node,
               BasicTreeNode *End,
               BasicTreeNode &LongestChild) noexcept
{
    auto &Back = SetParentOfSiblings(&This, Node, End);
    Back.setNextSibling(nullptr);

    if (This.getFirstChild() == nullptr) {
        Node.setPrevSibling(nullptr);

        This.setFirstChild(&Node);
        This.setLongestChild(&LongestChild);
    } else {
        const auto PrevSibling = This.getLastChild();

        PrevSibling->setNextSibling(&Node);
        Node.setPrevSibling(PrevSibling);

        SetIfLongestChild(This, LongestChild);
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
    assert(!List.empty());
    setFirstChild(List.front());

    const auto End = List.cend();
    auto PrevChild = FirstChild;

    for (auto Iter = List.cbegin() + 1; Iter != End; Iter++) {
        auto Node = *Iter;
        assert(Node != nullptr);

        PrevChild->setNextSibling(Node);
        Node->setPrevSibling(PrevChild);

        PrevChild = Node;
    }

    PrevChild->setNextSibling(nullptr);
    return *this;
}

static void ClearNode(BasicTreeNode &Node) noexcept {
    Node.setParent(nullptr);
    Node.setFirstChild(nullptr);
    Node.setPrevSibling(nullptr);
    Node.setNextSibling(nullptr);
    Node.setLongestChild(nullptr);

    delete &Node;
}

static void BasicIsolate(BasicTreeNode &Node) noexcept {
    auto ParentHasOnlyOneChild = false;
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
        ParentHasOnlyOneChild = true;
    }

    const auto ParentPtr = Node.getParent();
    const auto FirstChild = Node.getFirstChild();

    if (FirstChild != nullptr)  {
        if (const auto PrevSibling = Node.getPrevSibling()) {
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
    if (!ParentHasOnlyOneChild) {
        if (Parent.getFirstChild() == &Node) {
            Parent.setFirstChild(Node.getNextSibling());
        }

        if (Parent.getLongestChild() == &Node) {
            RecalculateLongestChild(Parent);
        }
    } else if (FirstChild == nullptr) {
        Parent.setFirstChild(nullptr);
        Parent.setLongestChild(nullptr);
    }
}

static void DoRemoveLeafParents(BasicTreeNode &LeafParent) noexcept {
    auto Parent = LeafParent.getParent();
    auto Child = &LeafParent;

    for (; Parent != nullptr; Child = Parent, Parent = Parent->getParent()) {
        BasicIsolate(*Child);
        ClearNode(*Child);

        if (Parent->getFirstChild() != nullptr) {
            break;
        }
    }
}

BasicTreeNode &
BasicTreeNode::IsolateAndRemoveFromParent(bool RemoveLeafParents,
                                          BasicTreeNode *Root) noexcept
{
    BasicIsolate(*this);

    const auto Parent = getParent();
    if (Parent == nullptr) {
        ClearNode(*this);
        return *this;
    }

    if (Parent == Root || Parent->getFirstChild() != nullptr) {
        ClearNode(*this);
        return *this;
    }

    if (RemoveLeafParents) {
        DoRemoveLeafParents(*Parent);
        ClearNode(*this);
    } else {
        ClearNode(*this);
    }

    return *this;
}

BasicTree::BasicTree(BasicTreeNode *Root) noexcept : Root(Root) {}
ConstBasicTree::ConstBasicTree(const BasicTreeNode *Root) noexcept
: BasicTree(const_cast<BasicTreeNode *>(Root)) {}

BasicTree::~BasicTree() noexcept {}

uint64_t BasicTree::GetCount() const noexcept {
    auto Count = uint64_t();
    for (auto &Iter : *this) {
        (void)Iter;
        Count++;
    }

    return Count;
}

BasicTree::Iterator BasicTree::begin() const noexcept {
    return Iterator(getRoot());
}

BasicTree::Iterator BasicTree::end() const noexcept {
    return Iterator(nullptr);
}

BasicTree::ConstIterator BasicTree::cbegin() const noexcept {
    return ConstIterator(getRoot());
}

BasicTree::ConstIterator BasicTree::cend() const noexcept {
    return ConstIterator(nullptr);
}

ConstBasicTree::Iterator ConstBasicTree::begin() const noexcept {
    return Iterator(getRoot());
}

ConstBasicTree::Iterator ConstBasicTree::end() const noexcept {
    return Iterator(nullptr);
}

ConstBasicTree::ConstIterator ConstBasicTree::cbegin() const noexcept {
    return ConstIterator(getRoot());
}

ConstBasicTree::ConstIterator ConstBasicTree::cend() const noexcept {
    return ConstIterator(nullptr);
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

BasicTreeNode *
BasicTree::RemoveNode(BasicTreeNode &Node, bool RemoveParentLeafs) noexcept {
    if (&Node == Root) {
        if (Node.getFirstChild() == nullptr) {
            Node.clear();

            setRoot(nullptr);
            return nullptr;
        }

        if (Node.getFirstChild()->getNextSibling() == nullptr) {
            setRoot(Node.getFirstChild());
            Node.clear();

            return getRoot()->getFirstChild();
        }

        setRoot(Node.createNew());
        Node.setParent(getRoot());

        BasicIsolate(Node);
        ClearNode(Node);

        return getRoot()->getFirstChild();
    }

    auto NextNode = const_cast<BasicTreeNode *>(Node.FindNextNodeForIterator());

    Node.IsolateAndRemoveFromParent(RemoveParentLeafs);
    return NextNode;
}

void BasicTreeNode::clear() noexcept {
    ClearNode(*this);
}
