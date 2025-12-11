#ifndef TWODIMTREE_H
#define TWODIMTREE_H

#include <iostream>
using namespace std;


class Rectangle {
public:
    int Top;      // y-coordinate of upper edge
    int Left;     // x-coordinate of left edge
    int Bottom;   // y-coordinate of lower edge (exclusive)
    int Right;    // x-coordinate of right edge (exclusive)

    Rectangle() : Top(0), Left(0), Bottom(0), Right(0) {}

    Rectangle(int t, int l, int b, int r)
        : Top(t), Left(l), Bottom(b), Right(r) {}

    // Returns true if point (x,y) lies inside the rectangle.
    // Right and Bottom edges are EXCLUDED, as required.
    bool contains(int x, int y) const {
        return (Left <= x && x < Right &&
                Top  <= y && y < Bottom);
    }

    // Prints rectangle coordinates in the required output format.
    void print() const {
        cout << Top << " " << Left << " "
             << Bottom << " " << Right;
    }
};


class BadIterator {};

template <class Object> class List;
template <class Object> class ListItr;

/* ------------------------ ListNode --------------------------
   Holds a single object and a pointer to the next node.
   ----------------------------------------------------------- */
template <class Object>
class ListNode {
    ListNode(const Object & theElement = Object(),
             ListNode * n = nullptr)
        : element(theElement), next(n) {}

    Object element;
    ListNode *next;

    friend class List<Object>;
    friend class ListItr<Object>;
};

/* ------------------------- ListItr --------------------------
   Iterator for traversing the linked list.
   Throws BadIterator when accessing past-end.
   ----------------------------------------------------------- */
template <class Object>
class ListItr {
public:
    ListItr() : current(nullptr) {}

    bool isPastEnd() const { return current == nullptr; }

    void advance() {
        if (!isPastEnd())
            current = current->next;
    }

    // Returns the element at the current node.
    const Object & retrieve() const {
        if (isPastEnd()) throw BadIterator();
        return current->element;
    }

private:
    ListNode<Object>* current;
    ListItr(ListNode<Object>* theNode) : current(theNode) {}

    friend class List<Object>;
};

/* --------------------------- List ---------------------------
   A singly linked list with support for:
   - inserting at end,
   - obtaining an iterator to the first node,
   - counting elements.
   ----------------------------------------------------------- */
template <class Object>
class List {
public:
    List() { header = new ListNode<Object>; }

    List(const List & rhs) {
        header = new ListNode<Object>;
        *this = rhs;
    }

    ~List() {
        makeEmpty();
        delete header;
    }

    bool isEmpty() const { return header->next == nullptr; }

    // Insert new node after the iterator position.
    void insert(const Object & x, const ListItr<Object> & p) {
        if (p.current != nullptr)
            p.current->next = new ListNode<Object>(x, p.current->next);
    }

    // Insert at end of list.
    void insertAtEnd(const Object& x) {
        ListNode<Object>* itr = header;
        while (itr->next != nullptr)
            itr = itr->next;
        itr->next = new ListNode<Object>(x, nullptr);
    }

    // Return iterator to first real element.
    ListItr<Object> first() const {
        return ListItr<Object>(header->next);
    }

    // Count total number of elements.
    int count() const {
        int c = 0;
        ListNode<Object>* itr = header->next;
        while (itr) {
            ++c;
            itr = itr->next;
        }
        return c;
    }

private:
    ListNode<Object>* header;

    // Delete all nodes after the header.
    void makeEmpty() {
        while (!isEmpty()) {
            ListNode<Object>* old = header->next;
            header->next = old->next;
            delete old;
        }
    }
};

/* 
   TwoDimTreeNode
   Represents a single node in the 2D tree.

   - Extent: the rectangular region covered by this node.
   - Vertical: rectangles intersecting the vertical center line.
   - Horizontal: rectangles intersecting the horizontal center line.
   - Four child pointers representing the four quadrants:
       TopLeft, TopRight, BottomLeft, BottomRight.
 */
template <class T>
class TwoDimTreeNode {
public:
    Rectangle Extent;
    List<T> Vertical;
    List<T> Horizontal;

    TwoDimTreeNode<T>* TopLeft;
    TwoDimTreeNode<T>* TopRight;
    TwoDimTreeNode<T>* BottomLeft;
    TwoDimTreeNode<T>* BottomRight;

    TwoDimTreeNode(const Rectangle& e)
        : Extent(e),
          TopLeft(nullptr), TopRight(nullptr),
          BottomLeft(nullptr), BottomRight(nullptr) {}

    ~TwoDimTreeNode() {
        delete TopLeft;
        delete TopRight;
        delete BottomLeft;
        delete BottomRight;
    }
};

/* 
   TwoDimTree
   Stores rectangles and supports searching for all rectangles
   containing a given point (x,y).
   Rectangles are inserted according to whether they intersect
   the node's center lines or fall entirely inside one quadrant.
 */
template <class T>
class TwoDimTree {
private:
    TwoDimTreeNode<T>* root;

    /* 
       INSERT OPERATION
       Places rectangle R in the correct subtree:

       1. Compute center lines of current node.
       2. If rectangle intersects vertical center line → store in Vertical.
       3. Else if intersects horizontal center line → store in Horizontal.
       4. Else descend into the appropriate quadrant subtree.
       5. If extent cannot be subdivided further → store locally.
     */
    void insert(const T& R, TwoDimTreeNode<T>* node) {
        int centerX = (node->Extent.Left + node->Extent.Right) / 2;
        int centerY = (node->Extent.Top  + node->Extent.Bottom) / 2;

        // If the region is too small to subdivide, store rectangle here.
        if ((node->Extent.Right  - node->Extent.Left)  <= 1 ||
            (node->Extent.Bottom - node->Extent.Top)   <= 1) {
            node->Vertical.insertAtEnd(R);
            return;
        }

        // Check intersection with vertical center line
        if (R.Left <= centerX && R.Right > centerX) {
            node->Vertical.insertAtEnd(R);
            return;
        }

        // Check intersection with horizontal center line
        if (R.Top <= centerY && R.Bottom > centerY) {
            node->Horizontal.insertAtEnd(R);
            return;
        }

        // Otherwise rectangle belongs fully to one quadrant:

        // TOP LEFT quadrant
        if (R.Right <= centerX && R.Bottom <= centerY) {
            if (!node->TopLeft) {
                Rectangle e(node->Extent.Top, node->Extent.Left,
                            centerY, centerX);
                node->TopLeft = new TwoDimTreeNode<T>(e);
            }
            insert(R, node->TopLeft);
        }
        // TOP RIGHT quadrant
        else if (R.Left > centerX && R.Bottom <= centerY) {
            if (!node->TopRight) {
                Rectangle e(node->Extent.Top, centerX + 1,
                            centerY, node->Extent.Right);
                node->TopRight = new TwoDimTreeNode<T>(e);
            }
            insert(R, node->TopRight);
        }
        // BOTTOM LEFT quadrant
        else if (R.Right <= centerX && R.Top > centerY) {
            if (!node->BottomLeft) {
                Rectangle e(centerY + 1, node->Extent.Left,
                            node->Extent.Bottom, centerX);
                node->BottomLeft = new TwoDimTreeNode<T>(e);
            }
            insert(R, node->BottomLeft);
        }
        // BOTTOM RIGHT quadrant
        else {
            if (!node->BottomRight) {
                Rectangle e(centerY + 1, centerX + 1,
                            node->Extent.Bottom, node->Extent.Right);
                node->BottomRight = new TwoDimTreeNode<T>(e);
            }
            insert(R, node->BottomRight);
        }
    }

    /* 
       SEARCH OPERATION
       Searches for all rectangles containing point (x,y):

       1. Test all rectangles stored in Vertical and Horizontal lists.
       2. If point lies on center lines → do not descend further.
       3. Else recursively search the child quadrant that contains (x,y).
     */
    void search(int x, int y, TwoDimTreeNode<T>* node,
                List<T>& result) const {
        if (!node) return;

        // Check rectangles intersecting vertical center line
        for (ListItr<T> it = node->Vertical.first();
             !it.isPastEnd(); it.advance()) {
            if (it.retrieve().contains(x, y))
                result.insertAtEnd(it.retrieve());
        }

        // Check rectangles intersecting horizontal center line
        for (ListItr<T> it = node->Horizontal.first();
             !it.isPastEnd(); it.advance()) {
            if (it.retrieve().contains(x, y))
                result.insertAtEnd(it.retrieve());
        }

        int centerX = (node->Extent.Left + node->Extent.Right) / 2;
        int centerY = (node->Extent.Top  + node->Extent.Bottom) / 2;

        // If point lies on center lines, no further search needed.
        if (x == centerX || y == centerY)
            return;

        // Search the appropriate quadrant.
        if (x < centerX && y < centerY) {
            search(x, y, node->TopLeft, result);
        }
        else if (x > centerX && y < centerY) {
            search(x, y, node->TopRight, result);
        }
        else if (x < centerX && y > centerY) {
            search(x, y, node->BottomLeft, result);
        }
        else {
            search(x, y, node->BottomRight, result);
        }
    }

public:

    /* Constructor: initializes tree with given extent rectangle. */
    TwoDimTree(const Rectangle& r) {
        root = new TwoDimTreeNode<T>(r);
    }

    /* Destructor: recursively deletes entire tree. */
    ~TwoDimTree() {
        delete root;
    }

    /* Public insert function. */
    void insert(const T& R) {
        insert(R, root);
    }

    /* Public search function. */
    void search(int x, int y, List<T>& result) const {
        search(x, y, root, result);
    }
};

#endif
