//
//  ADT/Tree.cpp
//  ktool
//
//  Created by Suhas Pai on 6/8/20.
//  Copyright © 2020 - 2024 Suhas Pai. All rights reserved.
//

#include "ADT/Tree.h"

TreeNode &TreeNode::SetAsParentOfChildren() noexcept {
    return SetAsParentOfChildren(*this);
}

TreeNode &
TreeNode::SetAsParentOfChildren(TreeNode &Node) noexcept {
    Node.forEachChild([&](TreeNode &Child) {
        Child.setParent(&Node);
    });

    return *this;
}

uint64_t TreeNode::GetChildCount() const noexcept {
    auto ChildCount = uint64_t();
    forEachChild([&](const TreeNode &Child) {
        (void)Child;
        ChildCount++;
    });

    return ChildCount;
}

static TreeNode &
SetParentOfSiblings(TreeNode *const Parent,
                    TreeNode &Node,
                    TreeNode *const End = nullptr) noexcept
{
    auto Back = &Node;
    Node.forEachSiblingTillEnd(End, [&](TreeNode &Sibling) {
        Sibling.setParent(Parent);
        Back = &Sibling;
    });

    return *Back;
}

void TreeNode::ValidateChildArray() const noexcept {
    if (this->isLeaf()) {
        return;
    }

    const auto FirstChild = this->getFirstChild();
    assert(FirstChild->getParent() == this);

    const auto SecondChild = FirstChild->getNextSibling();
    if (SecondChild == nullptr) {
        return;
    }

    assert(SecondChild->getParent() == this);

    auto Prev = FirstChild;
    SecondChild->forEachSibling([&](TreeNode &Sibling) {
        assert(Sibling.getParent() == this);
        assert(Sibling.getPrevSibling() == Prev);

        Prev = &Sibling;
    });

    assert(Prev == this->getLastChild());
}

static void
AddSiblingsRaw(TreeNode &This,
               TreeNode &Node,
               TreeNode *const End) noexcept
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
AddChildrenRaw(TreeNode &Parent,
               TreeNode &Node,
               TreeNode *const End) noexcept
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

TreeNode &TreeNode::AddChild(TreeNode &Node) noexcept {
    AddChildrenRaw(*this, Node, nullptr);
    return *this;
}

auto TreeNode::AddChildren(TreeNode &Node, TreeNode *const End) noexcept
    -> decltype(*this)
{
    AddChildrenRaw(*this, Node, End);
    return *this;
}

auto TreeNode::AddSibling(TreeNode &Node) noexcept
    -> decltype(*this)
{
    AddSiblingsRaw(*this, Node, nullptr);
    return *this;
}

auto
TreeNode::AddSiblings(TreeNode &Node, TreeNode *const End) noexcept
    -> decltype(*this)
{
    AddSiblingsRaw(*this, Node, End);
    return *this;
}

Tree::Tree(TreeNode *Root) noexcept : Root(Root) {}
Tree::~Tree() noexcept {}

uint64_t Tree::GetCount() const noexcept {
    auto Count = uint64_t();
    forEachNode([&](const TreeNode &Node) {
        (void)Node;
        Count++;
    });

    return Count;
}

auto
TreeNode::FindPrevNodeForIterator(const TreeNode *const End,
                                  uint64_t *const DepthChangeOut) const noexcept
    -> const TreeNode *
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

auto
TreeNode::FindNextSiblingForIterator(
    const TreeNode *const End,
    uint64_t *const DepthChangeOut) const noexcept
    -> const TreeNode *
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

auto
TreeNode::FindNextNodeForIterator(const TreeNode *const End,
                                  int64_t *const DepthChangeOut) const noexcept
    -> const TreeNode *
{
    if (const auto FirstChild = this->getFirstChild()) {
        if (DepthChangeOut != nullptr) {
            *DepthChangeOut = -1;
        }

        return FirstChild;
    }

    const auto Out = reinterpret_cast<uint64_t *>(DepthChangeOut);
    return this->FindNextSiblingForIterator(End, Out);
}

static void ClearNode(TreeNode &Node) noexcept {
    Node.setParent(nullptr);
    Node.setFirstChild(nullptr);
    Node.setPrevSibling(nullptr);
    Node.setNextSibling(nullptr);
    Node.setLastChild(nullptr);

    delete &Node;
}

void TreeNode::clearAndDestroy() noexcept {
    ClearNode(*this);
}

static void IsolateNode(TreeNode &Node) noexcept {
    // Fix the pointers of the sibling nodes to not point to this node.

    auto ParentOnlyHasThisNode = false;
    if (const auto PrevSibling = Node.getPrevSibling()) {
        if (const auto NextSibling = Node.getNextSibling()) {
            PrevSibling->setNextSibling(NextSibling);
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
    if (const auto FirstChild = Node.getFirstChild()) {
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

[[nodiscard]] static auto
DoRemoveLeafParents(TreeNode &LeafParent, TreeNode &Root) noexcept {
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

[[nodiscard]] static auto
IsolateNodeAndRemoveFromParent(TreeNode &Node,
                               TreeNode &Root,
                               const bool RemoveLeafParents) noexcept
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

auto
Tree::RemoveNode(TreeNode &Node, const bool RemoveParentLeafs) noexcept
    -> TreeNode *
{
    if (&Node == this->getRoot()) {
        if (Node.isLeaf()) {
            this->setRoot(nullptr);

            IsolateNode(Node);
            Node.clearAndDestroy();

            return nullptr;
        }

        if (Node.hasOnlyOneChild()) {
            this->setRoot(Node.getFirstChild());

            IsolateNode(Node);
            Node.clearAndDestroy();

            return this->getRoot();
        }

        this->setRoot(Node.createNew());
        this->getRoot()->SetAsParentOfChildren(*Node.getFirstChild());

        IsolateNode(Node);
        Node.clearAndDestroy();

        return nullptr;
    }

    const auto NextNode =
        const_cast<TreeNode *>(Node.FindNextNodeForIterator());

    if (IsolateNodeAndRemoveFromParent(Node, *Root, RemoveParentLeafs)) {
        this->setRoot(nullptr);
    }

    return NextNode;
}

void Tree::ValidateNodes() const noexcept {
    for (const auto &Node : *this) {
        Node->ValidateChildArray();
    }
}
