#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    void rotateRight(AVLNode<Key, Value>* grandparent); 
    void rotateLeft(AVLNode<Key, Value>* parent);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<typename Key, typename Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    if(this->root_ == NULL){
        // empty tree - done
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        this->root_ = newNode;
        newNode->setBalance(0);
        return;
    }
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    // walk the tree to a leaf
    while(current != NULL){
        parent = current;
        if(new_item.first == current->getKey()){
            current->setValue(new_item.second);
            break;
        }
        // less than
        else if(new_item.first < current->getKey()){
            // left child alr exists
            if(current->getLeft()){
                current = current->getLeft();
            }
            // make new node
            else {
                current->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, current));
                current = current->getLeft();
                current->setBalance(0);
                // update parent
                if(parent != NULL){
                    if(parent->getBalance() == -1 || parent->getBalance() == 1){
                        parent->setBalance(0);
                    } 
                    else { // parent's balance was 0
                        parent->updateBalance((parent->getLeft() == current) ? -1 : 1);
                        insertFix(parent, current);
                    }
                }
                return;
            }
        }
        // greater than
        else {
            // right child alr exists
            if(current->getRight()){
                current = current->getRight();
            }
            // make new node
            else {
                current->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, current));
                current = current->getRight();
                current->setBalance(0);
                // update parent
                if(parent != NULL){
                    if(parent->getBalance() == -1 || parent->getBalance() == 1){
                        parent->setBalance(0);
                    } 
                    else { // parent's balance was 0
                        parent->updateBalance((parent->getRight() == current) ? 1 : -1);
                        insertFix(parent, current);
                    }
                }
                return;
            }
        }
    }

}

template<typename Key, typename Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
    if(parent == NULL || parent->getParent() == NULL){
        return;
    }

    AVLNode<Key, Value>* grandparent = static_cast<AVLNode<Key, Value>*>(parent->getParent());
    
    // parent is left child of grandparent
    if(grandparent->getLeft() == parent){
        grandparent->updateBalance(-1);
        // case 1
        if(grandparent->getBalance() == 0){
            return;
        }
        // case 2
        else if(grandparent->getBalance() == -1){
            insertFix(grandparent, parent);
        }
        // case 3
        else if(grandparent->getBalance() == -2){
            // LL zigzig
            if(parent->getBalance() == -1){
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            // LR zigzag
            else {
                rotateLeft(parent);
                rotateRight(grandparent);
                // case 3a
                if(node->getBalance() == -1){
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                }
                // case 3b
                else if(node->getBalance() == 0){
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                }
                // case 3c
                else if(node->getBalance() == 1){
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                }
                node->setBalance(0);

            }
        }
    }
    // assume parent is right child of grandparent
    else if(grandparent->getRight() == parent){
        grandparent->updateBalance(1);
        // case 1
        if(grandparent->getBalance() == 0){
            return;
        }
        // case 2
        else if(grandparent->getBalance() == 1){
            insertFix(grandparent, parent);
        }
        // case 3
        else if(grandparent->getBalance() == 2){
            // RR zigzig
            if(parent->getBalance() == 1){
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            // RL zigzag
            else {
                rotateRight(parent);
                rotateLeft(grandparent);
                // case 3a
                if(node->getBalance() == 1){

                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                }
                // case 3b
                else if(node->getBalance() == 0){
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                }
                // case 3c
                else if(node->getBalance() == -1){
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // find node n to remove
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    int8_t diff = 0;
    if(n == NULL){
        return;
    }

    // if n has two children swap positions with predecessor
    if(n->getLeft() != NULL && n->getRight() != NULL){
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key, Value>* parent = n->getParent();
    if(parent){
        if(n == parent->getLeft()){
            diff = 1;
        }
        else {
            diff = -1;
        }
    }
    int children = 0;
    if(n->getParent() == NULL){
        children = 0;
    }
    else if(n->getParent()->getLeft() == n){
        children = -1;
    }
    else {
        children = 1;
    }

    // leaf node - no children
    if(n->getLeft() == NULL && n->getRight() == NULL){
        if(children == -1){
            parent->setLeft(NULL);
        }
        else if(children == 1){
            parent->setRight(NULL);
        }
        else {
            this->root_ = NULL;
        }
        delete n;
    }
    // one child
    else if(n->getLeft() != NULL || n->getRight() != NULL){
        // left child
        if(n->getRight() == NULL){
            if(children == -1){
                n->getLeft()->setParent(n->getParent());
                n->getParent()->setLeft(n->getLeft());
            }
            if(children == 1){
                n->getLeft()->setParent(n->getParent());
                n->getParent()->setRight(n->getLeft());
            }
            else {
                this->root_ = n->getLeft();
                this->root_->setParent(NULL);
            }
        }
        // right child
        else if(n->getLeft() == NULL){
            if(children == -1){
                n->getRight()->setParent(n->getParent());
                n->getParent()->setLeft(n->getRight());
            }
            if(children == 1){
                n->getRight()->setParent(n->getParent());
                n->getParent()->setRight(n->getRight());
            }
            else {
                this->root_ = n->getRight();
                this->root_->setParent(NULL);
            }
        }
        delete n;
    }
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff) {
    if(node == NULL){
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int ndiff = 0;
    if(parent != NULL){
        ndiff = 1;
    }
    else {
        ndiff = -1;
    }

    if(diff == -1){
        // case 1
        if(node->getBalance() + diff == -2){
            AVLNode<Key, Value>* c = node->getLeft();
            // case 1a - zigzig
            if(c->getBalance() == -1){
                rotateRight(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // case 1b - zigzig
            else if(c->getBalance() == 0){
                rotateRight(node);
                node->setBalance(-1);
                c->setBalance(1);
            }
            // case 1c
            else if (c->getBalance() == 1){
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(node);
                if(g->getBalance() == 1){
                    node->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0){
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    node->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(parent, ndiff);
            }
        }
    }
    else { // diff = 1
        // mirror case 1
        if(node->getBalance() + diff == 2){
            AVLNode<Key, Value>* c = node->getRight();
            // case 1a - zigzig
            if(c->getBalance() == 1){
                rotateLeft(node);
                node->setBalance(0);
                c->setBalance(0);
                removeFix(parent, ndiff);
            }
            // case 1b - zigzig
            else if(c->getBalance() == 0){
                rotateLeft(node);
                node->setBalance(1);
                c->setBalance(-1);
                return;
            }
            // case 1c
            else if (c->getBalance() == -1){
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(node);
                if(g->getBalance() == -1){
                    node->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0){
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else {
                    node->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(parent, ndiff);
            }
        }
        // case 2
        else if(node->getBalance() + diff == 1){
            node->setBalance(1);
            return;
        }
        // case 3
        else if(node->getBalance() + diff == 0){
            node->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* grandparent) {
    
    AVLNode<Key, Value>* gp = grandparent->getParent();
    AVLNode<Key, Value>* pivot = grandparent->getLeft();
    AVLNode<Key, Value>* gr = pivot->getRight();

    if(gp != NULL){
        if(grandparent == gp->getLeft()){
            gp->setLeft(pivot);
        }
        else {
            gp->setRight(pivot);
        }
    }
    else {
        this->root_ = pivot;
    }
    pivot->setParent(gp);
    pivot->setRight(grandparent);
    grandparent->setParent(pivot);
    grandparent->setLeft(gr);
    if(gr != NULL){
        gr->setParent(grandparent);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* parent) {
    AVLNode<Key, Value>* p = parent->getParent();
    AVLNode<Key, Value>* pivot = parent->getRight();
    AVLNode<Key, Value>* l = pivot->getLeft();

    if(p != NULL){
        if(parent == p->getLeft()){
            p->setLeft(pivot);
        }
        else {
            p->setRight(pivot);
        }
    }
    else {
        this->root_ = pivot;
    }
    pivot->setParent(p);
    pivot->setLeft(parent);
    parent->setParent(pivot);
    parent->setRight(l);
    if(l != NULL){
        l->setParent(parent);
    }
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
    // TODO
    // we have left child
    AVLNode<Key, Value>* pred = NULL;
    if(current == NULL){
        return NULL;
    }
    if(current->getLeft() != NULL){
        pred = current->getLeft();
        while(pred->getRight() != NULL){
            pred = pred->getRight();
        }
    }
    else {
        bool f = false;
        while(current->getParent() && !f){
            if(current->getParent()->getRight() != current){
                pred = current->getParent();
                f = true;
            }
            current = current->getParent();
        }
    }
    return pred;
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
