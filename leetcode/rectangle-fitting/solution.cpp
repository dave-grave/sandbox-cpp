// rectangle-fitting
// 
// Goal:
// you are handling a warehouse of rectangular boxes size m*n. there exist two operations,
// one to create a new box, and one to fit a package into a box. 
// - [0, x, y] creates a new box of size x*y.
// - [1, x, y] checks if a package of size x*y can fit inside of any of the previously made boxes.
//
// packages can be rotated so that they fit inside of a box. so either x*y or y*x.
//
// assume that once a box is created, there are infinitely many of them remaining in the warehouse
// to fit as many packages as possible. return a vector for each 1-operation whether or not 
// the package could fit inside of any previously created boxes.
//
// if there are no previously created boxes, a 1-operation should be true. (vacuously true statement lol)

// sol:
// we keep a normalized, monotonically increasing list of ordered pairs corresponding to the (x,y) sizes.
// - the pairs are normalized based off of the pattern: (min, max)
// - the list of pairs is ordered from the minimum, or first, element
//
// the list invariant is an ordered map for O(nlogn) total insertion of all the operations.
// on every box insertion operation, we check if the box covers any of the previous boxes. 
// 
// if we find any smaller boxes then we prune those accordingly to maintain the invariant that
// each box is the largest box for a given x-width that can't be covered by any of the previous boxes.
//
// this simplifies queries to be O(1) as well since we can use lower_bound to find the smallest box 
// that could possibly fit the query package.
//
// time complexity: O(nlogn), space complexity: O(n)

#include <map>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<bool> rectangle_fitting_sol(vector<vector<int>> &operations) {
	// create ordered map of best rectangles.
	map<int, int> rects;	
	vector<bool> res; 

	for (vector<int> &operation: operations) {
		int small = min(operation[1], operation[2]);
		int large = max(operation[1], operation[2]);
		auto it = rects.lower_bound(small);
		
		if (operation[0] == 0) {
			// insertion operation
			
			// 1. check if the current rectangle fits 
			if (it != rects.end() && it->second >= large) continue;
			
			// 2. update the new rectangle
			rects[small] = large;

			// 3. pruning step
			auto curr_it = rects.find(small);
			while (curr_it != rects.begin()) {
				// temporary store previous neighbor for safe deletions
				auto prev_it = prev(curr_it);

				if (prev_it->second <= large) {
					// delete all contained rectangles
					rects.erase(prev_it); 
				}
				else break;
			}
		}
		else /* (operation[0] == 1) */ {
			// query operation
			if (rects.empty()) {
				// empty edge case. we return true here
				res.push_back(true);
			}
			else {
				if (it == rects.end() || it->second < large) {
					res.push_back(false);
				}
				else {
					res.push_back(true);
				}
			}
		}
		
		// debug output
		cout << "\niteration:\n";
		for (auto &pair : rects) {
			cout << pair.first << " " << pair.second << '\n';
		}

	}
	return res;
}

int main() {
    // Helper lambda for printing results clearly
    auto run_test = [](vector<vector<int>>& ops, vector<bool>& expected, const std::string& name) {
        cout << "--- " << name << " ---\n";
        vector<bool> res = rectangle_fitting_sol(ops);
        cout << "Expected: ";
        for (bool r : expected) cout << (r ? "True " : "False ");
        cout << "\nActual:   ";
        for (bool r : res) cout << (r ? "True " : "False ");
        cout << "\n\n";
    };

    // ====== TEST 1: The Standard Check ======
    // (Your original test case)
    vector<vector<int>> ops1 = {
        {0, 1, 5}, 
        {0, 2, 4},
        {1, 2, 3},
        {1, 10, 1}
    };
    vector<bool> exp1 = {true, false};
    run_test(ops1, exp1, "Test 1: Standard & Rotation");


    // ====== TEST 2: Vacuously True & Exact Match ======
    // Queries before any box is made, then tests exact boundary fits.
    vector<vector<int>> ops2 = {
        {1, 10, 10}, 
        {0, 8, 8},
        {1, 8, 8},   
        {1, 9, 8}    
    };
    vector<bool> exp2 = {true, true, false};
    run_test(ops2, exp2, "Test 2: Empty Warehouse & Exact Match");


    // ====== TEST 3: The Pruning Sweep ======
    // Inserts smaller boxes, then inserts a massive box that should 
    // swallow everything, followed by queries.
    vector<vector<int>> ops3 = {
        {0, 5, 10},
        {0, 6, 8},
        {0, 8, 8},
        {1, 6, 9},   
		{0, 6, 15},
        {0, 20, 20}, 
        {1, 6, 9},   
        {1, 19, 19}  
    };
    vector<bool> exp3 = {false, true, true};
    run_test(ops3, exp3, "Test 3: The Pruning Sweep");


    // ====== TEST 4: The Descending Staircase ======
    // Ensures insertion logic perfectly maintains the staircase without
    // accidentally erasing valid, narrower-but-taller boxes.
    vector<vector<int>> ops4 = {
        {0, 2, 100},
        {0, 5, 50},
        {0, 10, 20},
        {1, 1, 90},  
        {1, 4, 40},  
        {1, 9, 15},  
        {1, 11, 10}  
    };
    vector<bool> exp4 = {true, true, true, true};
    run_test(ops4, exp4, "Test 4: Descending Staircase Integrity");

    return 0;
}
