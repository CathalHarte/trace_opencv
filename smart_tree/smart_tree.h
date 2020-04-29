/******************************************************************************/
/*!
 * @file  smart_tree.h
 * @brief Named a smart tree because it is an unsorted tree implemented using 
 *        smart pointers
 * 
 *        Header only template class
 * 
 * 
 * @author Cathal Harte <cathal.harte@protonmail.com>
 */
#ifndef _SMART_TREE_H
#define _SMART_TREE_H

/*******************************************************************************
* Includes
******************************************************************************/

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace smart_tree
{

/*! @defgroup smart_tree Smart_tree.
 *
 * @addtogroup smart_tree
 * @{
 * @brief 
 */
/*******************************************************************************
* Definitions and types
*******************************************************************************/

/*******************************************************************************
* Data
*******************************************************************************/

/*******************************************************************************
* Class prototypes
*******************************************************************************/

//  This class implements a node for an unsorted tree of strings
//  Design parameters :
//      A branch without a parent is a root.
//      A branch may have 1 parent and an unlimited number of children.
//      We use smart ptrs throughout
//          As long as a reference to the parent remains, the children will not expire
//          The corollary of this is that if a reference to a child is kept and no references to the parent are
//          then the parent and siblings will expire and the child becomes the new root
template <typename T>
class Branch
{
public:
    Branch() {}
    Branch(T data) : data(data) {}
    T data;

    bool isRoot() { return parent.expired(); }
    std::shared_ptr<Branch<T>> getParent() { return parent.lock(); }
    std::size_t getNumChildren() { return children.size(); }
    typename std::vector<std::shared_ptr<Branch<T>>>::iterator childrenBegin() { children.begin(); }
    typename std::vector<std::shared_ptr<Branch<T>>>::iterator childrenEnd() { children.end(); }

private:
    std::weak_ptr<Branch<T>> parent;
    std::vector<std::shared_ptr<Branch<T>>> children;

    template <typename Y>
    friend void addChild(std::shared_ptr<Branch<Y>> parent, std::shared_ptr<Branch<Y>> child);
    template <typename Y>
    friend void removeChild(std::shared_ptr<Branch<Y>> parent, std::shared_ptr<Branch<Y>> child);
};

/*******************************************************************************
* Function prototypes
*******************************************************************************/

template <typename Y>
void addChild(std::shared_ptr<Branch<Y>> parent, std::shared_ptr<Branch<Y>> child)
{
    assert(("Prospective child is parentless", child->parent.expired() == true));
    parent->children.emplace_back(child);
    child->parent = parent;
}

template <typename Y>
void removeChild(std::shared_ptr<Branch<Y>> parent, std::shared_ptr<Branch<Y>> child)
{
    auto iter = parent->children.begin();
    auto end = parent->children.end();
    for (; iter != end; advance(iter, 1))
    {
        if (child == *iter)
        {
            parent->children.erase(iter);
            child->parent.reset();
            return;
        }
    }
    throw "is not a child of parent";
}

/*! @}
 */

} // namespace smart_tree

#endif // _SMART_TREE_H
