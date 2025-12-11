#include <iostream>
#include <vector>
#include <algorithm>
#include "MPQ.h"

using namespace std;

// This event struct keeps information about the left or right side
// of each building. The assignment document says we need this
// to do the sweep line easier.
struct Event {
    int x;          // x coordinate of the event
    int height;     // height of the building
    int id;         // id of the building
    bool isStart;   // true = left side, false = right side
};

// Sorting function for events.
// I follow the rules explained in the assignment document:
// - smaller x first
// - if same x: start comes before end
// - if start: higher one first
// - if end: lower one first
bool eventSort(const Event &a, const Event &b) {

    // 1) smaller x first
    if (a.x < b.x) return true;
    if (a.x > b.x) return false;

    // 2) start event should come before end event
    if (a.isStart && !b.isStart) return true;
    if (!a.isStart && b.isStart) return false;

    // 3) if both start: higher height first
    if (a.isStart) return a.height > b.height;

    // 4) if both end: lower height first
    return a.height < b.height;
}


int main() {
    int n;
    cin >> n;   // number of buildings

    vector<int> L(n), H(n), R(n);

    // Reading buildings: left x, height, right x
    // This part is same as example in the homework document.
    for (int i = 0; i < n; i++) {
        cin >> L[i] >> H[i] >> R[i];
    }

    vector<Event> events;

    // Create 2 events for each building,
    // like assignment paper shows.
    for (int i = 0; i < n; i++) {

        Event e1;
        e1.x = L[i];
        e1.height = H[i];
        e1.id = i;
        e1.isStart = true;
        events.push_back(e1);

        Event e2;
        e2.x = R[i];
        e2.height = H[i];
        e2.id = i;
        e2.isStart = false;
        events.push_back(e2);
    }

    // Sort the events for sweep line
    sort(events.begin(), events.end(), eventSort);

    // The assignment document says:
    // If no building begins at x=0, skyline first point is (0,0).
    int firstX = events[0].x;

    if (firstX > 0) {
        cout << "0 0" << endl;
    }

    // Create MPQ object (heap + location array)
    // because assignment wants a modified priority queue.
    MPQ mpq(n + 5);

    int currentMax = 0; // current skyline height

    // Sweep line algorithm (assignment suggests this way)
    for (int i = 0; i < events.size(); i++) {
        Event &e = events[i];

        if (e.isStart) {
            // building begins → add height into MPQ
            mpq.insert(e.height, e.id);
        } else {
            // building ends → remove by label
            mpq.Remove(e.id);
        }

        // get new max height after update
        int newMax = mpq.GetMax();

        // if height changed, skyline changes → print it
        if (newMax != currentMax) {
            cout << e.x << " " << newMax << endl;
            currentMax = newMax;
        }
    }

    return 0;
}
