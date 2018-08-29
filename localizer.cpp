/**
	localizer.cpp

	Purpose: implements a 2-dimensional histogram filter
	for a robot living on a colored cyclical grid by 
	correctly implementing the "initialize_beliefs", 
	"sense", and "move" functions.

	This file is incomplete! Your job is to make these
	functions work. Feel free to look at localizer.py 
	for working implementations which are written in python.
*/

#include "helpers.cpp"
#include <stdlib.h>
#include "debugging_helpers.cpp"

using namespace std;

/**
 
    Initializes a grid of beliefs to a uniform distribution. 

    @param grid - a two dimensional grid map (vector of vectors 
    	   of chars) representing the robot's world. For example:
    	   
    	   g g g
    	   g r g
    	   g g g
		   
		   would be a 3x3 world where every cell is green except 
		   for the center, which is red.

    @return - a normalized two dimensional grid of floats. For 
           a 2x2 grid, for example, this would be:

           0.25 0.25
           0.25 0.25
*/
vector< vector <float> > initialize_beliefs(vector< vector <char> > grid) {
	// your code here
	//  height = len(grid)
	//	width = len(grid[0])
	//	area = height * width
	//	belief_per_cell = 1.0 / area
	//	beliefs = []
	//	for i in range(height) :
	//		row = []
	//		for j in range(width) :
	//			row.append(belief_per_cell)
	//			beliefs.append(row)
	//			return beliefs
	int height = grid.size();
	int width = grid[0].size();
	float area = width * height;
	float belief_per_cell = 1.0f / area;
	vector< vector <float> > beliefs;
	for (int i = 0; i < height; ++i)
	{
		vector<float> row;
		for (int j = 0; j < width; ++i)
		{
			row.push_back(belief_per_cell);
		}
		beliefs.push_back(row);
	}

	return beliefs;
}

/**
    
    Implements robot sensing by updating beliefs based on the 
    color of a sensor measurement 

	@param color - the color the robot has sensed at its location

	@param grid - the current map of the world, stored as a grid
		   (vector of vectors of chars) where each char represents a 
		   color. For example:

		   g g g
    	   g r g
    	   g g g

   	@param beliefs - a two dimensional grid of floats representing
   		   the robot's beliefs for each cell before sensing. For 
   		   example, a robot which has almost certainly localized 
   		   itself in a 2D world might have the following beliefs:

   		   0.01 0.98
   		   0.00 0.01

    @param p_hit - the RELATIVE probability that any "sense" is 
    	   correct. The ratio of p_hit / p_miss indicates how many
    	   times MORE likely it is to have a correct "sense" than
    	   an incorrect one.

   	@param p_miss - the RELATIVE probability that any "sense" is 
    	   incorrect. The ratio of p_hit / p_miss indicates how many
    	   times MORE likely it is to have a correct "sense" than
    	   an incorrect one.

    @return - a normalized two dimensional grid of floats 
    	   representing the updated beliefs for the robot. 
*/
vector< vector <float> > sense(char color, 
	vector< vector <char> > grid, 
	vector< vector <float> > beliefs, 
	float p_hit,
	float p_miss) 
{
	//new_beliefs = []
	//for y, row in enumerate(grid) :
	//	new_row = []
	//	for x, cell in enumerate(row) :
	//		prior = beliefs[y][x]
	//		if cell == color :
	//			p = prior * p_hit
	//		else :
	//			p = prior * p_miss
	//			#print p
	//		new_row.append(p)
	//	new_beliefs.append(new_row)
//new_beliefs = normalize(new_beliefs)	
	float p = 0;
	vector< vector <float> > new_beliefs;
	for (int y = 0; y < grid.size(); ++y) {
		vector<float> new_row;
		for (int x = 0; x < grid[0].size(); ++x) {
			float prior = beliefs[y][x];
			if (grid[0][x] == color) {
				p = prior * p_hit;
			}
			else {
				p = prior * p_miss;
			}
			new_row.push_back(p);
		}
		new_beliefs.push_back(new_row);
	}
	return normalize(new_beliefs);
}


/**
    
    Implements robot motion by updating beliefs based on the 
    intended dx and dy of the robot. 

    For example, if a localized robot with the following beliefs

    0.00  0.00  0.00
    0.00  1.00  0.00
    0.00  0.00  0.00 

    and dx and dy are both 1 and blurring is 0 (noiseless motion),
    than after calling this function the returned beliefs would be

    0.00  0.00  0.00
    0.00  0.00  0.00
    0.00  0.00  1.00 

	@param dy - the intended change in y position of the robot

	@param dx - the intended change in x position of the robot

   	@param beliefs - a two dimensional grid of floats representing
   		   the robot's beliefs for each cell before sensing. For 
   		   example, a robot which has almost certainly localized 
   		   itself in a 2D world might have the following beliefs:

   		   0.01 0.98
   		   0.00 0.01

    @param blurring - A number representing how noisy robot motion
           is. If blurring = 0.0 then motion is noiseless.

    @return - a normalized two dimensional grid of floats 
    	   representing the updated beliefs for the robot. 
*/
vector< vector <float> > move(int dy, int dx, 
	vector < vector <float> > beliefs,
	float blurring) 
{
	//height = len(beliefs)
	//	width = len(beliefs[0])
	//	new_G = [[0.0 for i in range(width)] for j in range(height)]
	//	for i, row in enumerate(beliefs) :
	//		for j, cell in enumerate(row) :
	//			new_i = (i + dy) % width
	//			new_j = (j + dx) % height
	//			# pdb.set_trace()
	//			new_G[int(new_j)][int(new_i)] = cell
	//	return blur(new_G, blurring)
	vector < vector <float> > newGrid;
	int height = beliefs.size();
	int width = beliefs[0].size();

	vector < vector<float> > new_G;
	for (int h = 0; h < height; ++h) {
		vector<float> init_row;
		for (int w = 0; w < width; ++w) {
			init_row.push_back(0);
		}
		new_G.push_back(init_row);
	}
			

	for (int i = 0; i < beliefs.size(); ++i) {
		for (int j = 0; j < beliefs[0].size(); ++j) {
			float new_i = (i + dy) % width;
			float new_j = (j + dx) % height;
			new_G[(int)new_j][(int)new_i] = beliefs[0][j];
		}
	}
	return blur(newGrid, blurring);
}