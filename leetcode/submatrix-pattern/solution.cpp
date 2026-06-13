// submatrix pattern
//
// Goal:
// given a k*k pattern submatrix of characters (lowercase english characters and digits),
// and an m*n board of digits, return the earliest submatrix whose pattern matches 
// those on the board. this is a tuple [row, column] representing the row and col values
// of the top left corner of the matching submatrix.
//
// if no matches exist, return [-1, -1].
//
// a pattern is matching if, for every entry of the submatrix:
// - a digit entry of the submatrix matches the digit entry of the board at the same pos
// - a char entry of the submatrix corresponds to a unique digit of the board at the 
//   same pos, and all chars correspond with exactly one unique digit. 
//     e.g. all the c's correspond to 2, all the d's to 1, etc. 
//
// time complexity: O(m*n*k^2)
// space complexity: O(k)

#include <vector>
#include <unordered_set>
#include <iostream>

using std::vector;
using std::unordered_set;
using std::cout;

vector<int> submatrix_pattern_sol( vector<vector<char>> &submatrix, vector<vector<int>> &board ) 
{
	// iterate through row and cols of the board first
	for ( int r_board=0; r_board<=board.size()-submatrix.size(); ++r_board )
	{
		for ( int c_board=0; c_board<=board[0].size()-submatrix[0].size(); ++c_board ) 
		{
			// create hashmap of from char to int
			vector<int> hashmap(26, -1);
			
			// set of corresponding integers for submatrix chars
			unordered_set<int> corrs;

			// iterate through submatrix
			bool isValid = true;
			for ( int r_sub=0; r_sub<submatrix.size(); ++r_sub )
			{
				for ( int c_sub=0; c_sub<submatrix[0].size(); ++c_sub ) 
				{
					char curr_sub = submatrix[r_sub][c_sub];
					int curr_board = board[r_board+r_sub][c_board+c_sub];
					if ( curr_sub - '0' > 9 || curr_sub - '0' < 0 )
					{
						// char
						if ( hashmap[curr_sub - 'a'] == -1 )	
						{
							if ( corrs.find(curr_board) != corrs.end() ) 
							{
								isValid = false;
								break; 	
							}

							else
							{
								hashmap[curr_sub - 'a'] = curr_board;
								corrs.insert(curr_board);
							}
						}
						else 
						{
							if ( hashmap[curr_sub - 'a'] != curr_board )
							{
								isValid = false;
								break;
							}
						}
							
					} 
					else 
					{
						// digit
						if (curr_sub - '0' != curr_board)
						{
							isValid = false;
							break;
						}
					}

				}

				if ( !isValid ) break; 
			}
			if ( isValid ) return {r_board, c_board};
		}
	}

	return {-1, -1}; 
}

int main( )
{
	vector<vector<int>> b1 = {
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	};
	vector<vector<char>> s1 = {
		{'a', 'b'},
		{'c', 'd'}
	};

	vector<int> res = submatrix_pattern_sol(s1, b1); 
	cout << res[0] << ", " << res[1] << '\n';
	return 0;
}

