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
    void insertFix( AVLNode<Key,Value>* p, AVLNode<Key,Value>* n );
    void removeFix( AVLNode<Key,Value>* n, int diff );
    void rotate( AVLNode<Key, Value>* n, int heavy );
    static AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(AVLTree<Key, Value>::root_ == NULL)
        AVLTree<Key, Value>::root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    else{
        //If root is not NULL
        AVLNode<Key, Value> *temp = dynamic_cast<AVLNode<Key, Value>*>(AVLTree<Key, Value>::root_);
        bool overwrite = false;
        bool isLeftChild = false;
        while(true){
            if(new_item.first < temp->getKey()){
                if(temp->getLeft() == NULL){ //If we are going off the end, make note of direction and stop
                    isLeftChild = true;
                    break;
                }
                temp = temp->getLeft();
            } else if(new_item.first > temp->getKey()) {
                if(temp->getRight() == NULL){ //If we are going off the end, make note of direction and stop
                    isLeftChild = false;
                    break;
                }
                temp = temp->getRight();
            } else{ //If key exists already
                overwrite = true;
                break;
            }
        }
    
        // Modify values as instructed
        if(overwrite)
            temp->setValue(new_item.second);
        else{
            if(isLeftChild)
                temp->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
            else
                temp->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
            // AVL Tree balancing
            if(temp->getBalance() != 0)
                temp->setBalance(0);
            else{
                if(isLeftChild){
                    temp->updateBalance(-1);
                    insertFix(temp, temp->getLeft());
                } else {
                    temp->updateBalance(1);
                    insertFix(temp, temp->getRight());
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix( AVLNode<Key,Value>* p, AVLNode<Key,Value>* n )
{
    if(p == NULL || p->getParent() == NULL)
        return;
    AVLNode<Key, Value>* g = p->getParent();
    int mult = 1; // 1 means right, -1 means left
    if(g->getLeft() == p)
        mult = -1;
    g->updateBalance(mult); // Update g's value to new accurate value
    if(g->getBalance() == 0)
        return;
    else if(g->getBalance() == 1*mult)
        insertFix(g, p); // Recurse
    else if(g->getBalance() == 2*mult) {
        if((mult == 1 && p->getRight() == n) || (mult == -1 && p->getLeft() == n)){ // Zig-zig
            rotate(g, mult);
            g->setBalance(0);
            p->setBalance(0);
        } else { // Zig-zag
            rotate(p, -1*mult);
            rotate(g, mult);
            if(n->getBalance() == mult){
                p->setBalance(0);
                g->setBalance(-1*mult);
                n->setBalance(0);
            } else if(n->getBalance() == 0) {
                p->setBalance(0);
                g->setBalance(0);
                n->setBalance(0);
            } else if(n->getBalance() == -1*mult) {
                p->setBalance(mult);
                g->setBalance(0);
                n->setBalance(0);
            }
        }
    }
}

// If heavy = -1 then it's rotate right, if heavy = 1 then it's rotate left
template<class Key, class Value>
void AVLTree<Key, Value>::rotate( AVLNode<Key, Value>* n, int heavy ) {
    if(heavy == -1) { // Rotate right, 6 changes necessary
        AVLNode<Key, Value>* p = n->getParent();
        AVLNode<Key, Value>* c = n->getLeft();
        
        if(p != NULL){
            if(p->getLeft() == n) //Check which child n is then set that child of p to c
                p->setLeft(c);
            else
                p->setRight(c);
        } else
            AVLTree<Key, Value>::root_ = c; // If we are rotating root we need to set root_ to c
        c->setParent(p);
        n->setParent(c);
        n->setLeft(c->getRight());
        c->setRight(n);
        if(n->getLeft() != NULL) // Check if child exists before setting parent
            n->getLeft()->setParent(n);

    } else {
        AVLNode<Key, Value>* p = n->getParent();
        AVLNode<Key, Value>* c = n->getRight();
        
        if(p != NULL){
            if(p->getLeft() == n) // Check which child n is then set that child of p to c
                p->setLeft(c);
            else
                p->setRight(c);
        } else
            AVLTree<Key, Value>::root_ = c; // If we are rotating root we need to set root_ to c
        c->setParent(p);
        n->setParent(c);
        n->setRight(c->getLeft());
        c->setLeft(n);
        if(n->getRight() != NULL) // Check if child exists before setting parent
            n->getRight()->setParent(n);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    if(AVLTree<Key, Value>::root_ == NULL){
        return;
    }
    //If root is not NULL
    AVLNode<Key, Value> *temp = dynamic_cast<AVLNode<Key, Value>*>(AVLTree<Key, Value>::root_);
    while(temp != NULL){
        if(key > temp->getKey()) // If key is greater than current node, go right
            temp = temp->getRight();
        else if (key < temp->getKey())
            temp = temp->getLeft(); // If key is less than current node, go left
        else { // We found the node
            if(temp->getLeft() != NULL && temp->getRight() != NULL) // If has both children
                nodeSwap(temp, predecessor(temp));
            AVLNode<Key, Value>* p = temp->getParent();
            int diff = 0;
            if(p != NULL){
                bool isLeftChild = p->getLeft() == temp;
                if(isLeftChild)
                    diff = 1;
                else
                    diff = -1;
            }
            // Update pointers, from BST
            if(temp->getLeft() == NULL && temp->getRight() == NULL){ // Has no children, set parent's child pointer to NULL, delete
                if(p == NULL) // If no parent, set root to NULL, then delete
                    AVLTree<Key, Value>::root_ = NULL;
                else if(p->getRight() == temp) //If is right child, set parent's right child to NULL
                    p->setRight(NULL);
                else // Else if is left child, set parent's left child to NULL
                    p->setLeft(NULL);
            } else if(temp->getLeft() == NULL && temp->getRight() != NULL){ // Has right child only, promote child then delete
                if(p != NULL){ // If has parent
                    if(p->getRight() == temp) //If temp is right child, set parent's right child to temp's right
                        p->setRight(temp->getRight());
                    else // Else if temp is left child, set parent's left child to temp's right
                        p->setLeft(temp->getRight());
                    temp->getRight()->setParent(p); //Set child's parent to temp's parent
                } else { // If no parent, set root to child, then delete
                    AVLTree<Key, Value>::root_ = temp->getRight();
                    AVLTree<Key, Value>::root_->setParent(NULL);
                }
            } else if(temp->getLeft() != NULL && temp->getRight() == NULL){ // Has left child only, promote child then delete
                if(p != NULL){ // If has parent
                    if(p->getRight() == temp) //If temp is right child, set parent's right child to temp's left
                        p->setRight(temp->getLeft());
                    else // Else if temp is left child, set parent's left child to temp's left
                        p->setLeft(temp->getLeft());
                    temp->getLeft()->setParent(p); //Set child's parent to temp's parent
                } else { // If no parent, set root to child, then delete
                    AVLTree<Key, Value>::root_ = temp->getLeft();
                    AVLTree<Key, Value>::root_->setParent(NULL);
                }
            }
            delete temp;
            removeFix(p, diff);
            break;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix( AVLNode<Key,Value>* n, int diff )
{
    if(n == NULL)
        return;
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = -1;
    if(p != NULL && p->getLeft() == n)
        ndiff = 1;
    if(n->getBalance() + diff == 2*diff) { // Case 1
        AVLNode<Key, Value>* c;
        if(diff == -1)
            c = n->getLeft();
        else
            c = n->getRight();
        if(c->getBalance() == 1*diff){ // Case 1a, zig zig
            rotate(n, 1*diff);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        } else if(c->getBalance() == 0) { // Case 1b, zig zig
            rotate(n, 1*diff);
            n->setBalance(1*diff);
            c->setBalance(-1*diff);
        } else if(c->getBalance() == -1*diff) { // Case 1c, zig zag
            AVLNode<Key, Value>* g;
            if (diff == -1)
                g = c->getRight();
            else
                g = c->getLeft();
            int gBal = g->getBalance();
            rotate(c, -1*diff);
            rotate(n, 1*diff);
            if(gBal == -1*diff){
                n->setBalance(0);
                c->setBalance(1*diff);
                g->setBalance(0);
            } else if(gBal == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            } else {// gBal == 1*diff
                n->setBalance(-1*diff);
                c->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p, ndiff);
        }
    } else if(n->getBalance() + diff == 1*diff) { // Case 2
        n->setBalance(1*diff);
    } else { // Case 3, balance + diff == 0
        n->setBalance(0);
        removeFix(p, ndiff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current)
{
    if(current->getLeft() != NULL){ // Need to find the largest node in left subtree
        AVLNode<Key, Value> *temp = current->getLeft();
        while(temp->getRight() != NULL) { temp = temp->getRight(); }
        return temp;
    }
    //If no left child need to check if this is a right or left child
    if(current->getParent()->getLeft() == current) // If left child, then return NULL
        return NULL;
    else
        return current->getParent(); // If right child, return parent
}

#endif
