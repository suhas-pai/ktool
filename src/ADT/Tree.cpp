//
//  src/ADT/Tree.cpp
//  stool
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
    BASIC_TREE_NODE_ITERATE_SIBLINGS_TILL_END(*Node.getNextSibling(), End) {
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

    auto Prev = getFirstChild();
    BASIC_TREE_NODE_ITERATE_SIBLINGS(*SecondChild) {
        assert(Sibling->getPrevSibling() == Prev);
        Prev = Sibling;
    }
}

static void
AddChildrenRaw(BasicTreeNode &This,
               BasicTreeNode &Node,
               BasicTreeNode *End,
               BasicTreeNode *LongestChild = nullptr) noexcept
{
    auto &Back = SetParentOfSiblings(&This, Node, End);
    Back.setNextSibling(nullptr);

    if (This.getFirstChild() == nullptr) {
        Node.setPrevSibling(nullptr);

        This.setFirstChild(&Node);
        This.setLongestChild(LongestChild);
    } else {
        const auto PrevSibling = This.getLastChild();

        PrevSibling->setNextSibling(&Node);
        Node.setPrevSibling(PrevSibling);

        if (This.getLongestChild()->GetLength() < LongestChild->GetLength()) {
            This.setLongestChild(LongestChild);
        }
    }
}

BasicTreeNode &BasicTreeNode::AddChild(BasicTreeNode &Node) noexcept {
    AddChildrenRaw(*this, Node, nullptr, &Node);
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddChildren(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    AddChildrenRaw(*this, Node, End);
    return *this;
}

BasicTreeNode &
BasicTreeNode::AddSiblings(BasicTreeNode &Node, BasicTreeNode *End) noexcept {
    auto &Back = SetParentOfSiblings(Parent, Node, End);

    Node.setPrevSibling(this);
    Back.setNextSibling(getNextSibling());

    if (const auto NextSibling = getNextSibling()) {
        NextSibling->setPrevSibling(&Back);
    }

    setNextSibling(&Node);

    const auto Parent = getParent();
    if (Parent == nullptr) {
        return *this;
    }

    auto &NewLongestChild = CalculateLongestNode(Node, End);
    if (Parent->getLongestChild()->GetLength() < NewLongestChild.GetLength()) {
        Parent->setLongestChild(&NewLongestChild);
    }

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

        PrevChild->NextSibling = Node;
        Node->PrevSibling = PrevChild;

        PrevChild = Node;
    }

    PrevChild->NextSibling = nullptr;
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
    if (ParentPtr == nullptr) {
        return;
    }

    auto &Parent = *ParentPtr;
    const auto FirstChild = Node.getFirstChild();

    if (FirstChild != nullptr)  {
        if (const auto PrevSibling = Node.getPrevSibling()) {
            PrevSibling->AddSiblings(*FirstChild);
        } else if (const auto NextSibling = Node.getNextSibling()) {
            const auto LastChild = Node.getLastChild();
            SetParentOfSiblings(ParentPtr, *FirstChild);

            LastChild->AddSiblings(*NextSibling);
            Parent.setFirstChild(FirstChild);
        } else {
            Parent.setFirstChild(nullptr);
            Parent.AddChildren(*FirstChild);
        }
    }

    auto DidDelete = false;
    if (!ParentHasOnlyOneChild) {
        if (Parent.getFirstChild() == &Node) {
            Parent.setFirstChild(Node.getNextSibling());
        }

        if (Parent.getLongestChild() == &Node) {
            DidDelete = true;
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
    if (getParent() == Root || getParent()->FirstChild != nullptr) {
        ClearNode(*this);
        return *this;
    }

    if (RemoveLeafParents) {
        DoRemoveLeafParents(*getParent());
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

BasicTreeNode *BasicTreeNode::FindNextNodeForIterator() const noexcept {
    for (auto Node = this; Node != nullptr; Node = Node->getParent()) {
        if (const auto NextSibling = Node->getNextSibling()) {
            return NextSibling;
        }
    }

    return nullptr;
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

        setRoot(getRoot()->createNew());
        Node.setParent(getRoot());

        BasicIsolate(Node);
        ClearNode(Node);

        return getRoot()->FirstChild;
    }

    auto NextNode = Node.getFirstChild();
    if (NextNode == nullptr) {
        NextNode = Node.FindNextNodeForIterator();
    }

    Node.IsolateAndRemoveFromParent(RemoveParentLeafs);
    return NextNode;
}

void BasicTreeNode::clear() noexcept {
    ClearNode(*this);
}