#include <iostream>
#include <fstream>
#include "TwoDimTree.h"
using namespace std;

int main()
{
    // read file
    string filename = "rectdb.txt";
    ifstream inputFile(filename);

    if (inputFile.fail()) {
        cout << "Error: Cannot open the file " << filename << endl;
        return 1;
    }

    // set up the tree
    int top, left, bottom, right;
    inputFile >> top >> left >> bottom >> right;
    Rectangle rootExtent(top, left, bottom, right);

    TwoDimTree<Rectangle> tree(rootExtent);

    // read rectangles
    while (inputFile >> top && top != -1) {
        inputFile >> left >> bottom >> right;
        tree.insert(Rectangle(top, left, bottom, right));
    }

    inputFile.close();

    // query cycle until x = -1
    int x, y;
    while (cin >> x && x != -1) {
        cin >> y;

        cout << x << " " << y << endl;

        // create empty result list
        List<Rectangle> results;

        // search the tree
        tree.search(x, y, results);

        // print number of found rectangles
        cout << results.count() << endl;

        // print rectangles
        for (ListItr<Rectangle> it = results.first(); !it.isPastEnd(); it.advance()) {
            it.retrieve().print();
            cout << endl;
        }
    }

    return 0;
}
