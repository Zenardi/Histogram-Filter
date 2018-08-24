# Histogram-Filter
Histogram Filter using C++

# Python Version 
## localizer.py
```import pdb
from helpers import normalize, blur

def initialize_beliefs(grid):
    height = len(grid)
    width = len(grid[0])
    area = height * width
    belief_per_cell = 1.0 / area
    beliefs = []
    for i in range(height):
        row = []
        for j in range(width):
            row.append(belief_per_cell)
        beliefs.append(row)
    return beliefs

def sense(color, grid, beliefs, p_hit, p_miss):
    new_beliefs = []
    for y, row in enumerate(grid):
        new_row = []
        for x, cell in enumerate(row):
            prior = beliefs[y][x]
            if cell == color:
                p = prior * p_hit
            else:
                p = prior * p_miss
            #print p
            new_row.append(p)
        new_beliefs.append(new_row)
    new_beliefs = normalize(new_beliefs)

    return new_beliefs

def move(dy, dx, beliefs, blurring):
    height = len(beliefs)
    width = len(beliefs[0])
    new_G = [[0.0 for i in range(width)] for j in range(height)]
    for i, row in enumerate(beliefs):
        for j, cell in enumerate(row):
            new_i = (i + dy ) % width
            new_j = (j + dx ) % height
            # pdb.set_trace()
            new_G[int(new_j)][int(new_i)] = cell
    return blur(new_G, blurring)
```   

## simulate.py
```import localizer 
import random
from copy import deepcopy
from matplotlib import pyplot as plt

class Simulation(object):
	def __init__(self, grid, blur, p_hit,start_pos=None):
		"""

		"""
		self.grid = grid
		self.beliefs = localizer.initialize_beliefs(self.grid)
		self.height = len(grid)
		self.width  = len(grid[0])
		self.blur   = blur
		self.p_hit = p_hit
		self.p_miss = 1.0
		self.incorrect_sense_probability = self.p_miss / (p_hit + self.p_miss)
		self.colors = self.get_colors()
		self.num_colors = len(self.colors)
		if not start_pos:
			self.true_pose = (self.height/2, self.width/2)
		else:
			self.true_pose = start_pos
		self.prev_pose = self.true_pose
		self.prepare_visualizer()

	def prepare_visualizer(self):
		self.X = []
		self.Y = []
		self.P = []

	def get_colors(self):
		all_colors = []
		for row in self.grid:
			for cell in row:
				if cell not in all_colors:
					all_colors.append(cell)
		return all_colors

	def sense(self):
		color = self.get_observed_color()
		beliefs = deepcopy(self.beliefs)
		new_beliefs = localizer.sense(color, self.grid, beliefs, self.p_hit, self.p_miss)
		if not new_beliefs or len(new_beliefs) == 0:
			print "NOTE! The robot doesn't have a working sense function at this point."
			self.beliefs = beliefs
		else:
			self.beliefs = new_beliefs

	def move(self, dy, dx):
		new_y = (self.true_pose[0] + dy) % self.height
		new_x = (self.true_pose[1] + dx) % self.width
		self.prev_pose = self.true_pose
		self.true_pose = (new_y, new_x)
		beliefs = deepcopy(self.beliefs)
		new_beliefs = localizer.move(dy, dx, beliefs, self.blur)
		self.beliefs = new_beliefs


	def get_observed_color(self):
		y,x = self.true_pose
		true_color = self.grid[y][x]
		if random.random() < self.incorrect_sense_probability:
			possible_colors = []
			for color in self.colors:
				if color != true_color and color not in possible_colors:
					possible_colors.append(color)
			color = random.choice(possible_colors)
		else:
			color = true_color
		return color

	def show_beliefs(self,past_turn=False):
		if past_turn:
			X = deepcopy(self.X)
			Y = deepcopy(self.Y)
			P = deepcopy(self.P)
		
		del(self.X[:])
		del(self.Y[:])
		del(self.P[:])
		for y, row in enumerate(self.beliefs):
			for x, belief in enumerate(row):
				self.X.append(x)
				self.Y.append(self.height-y-1) # puts large y ABOVE small y
				self.P.append(5000.0 * belief)
		plt.figure()
		if past_turn:
			plt.scatter(X, Y, s=P, alpha=0.3,color="blue")
			plt.scatter([self.prev_pose[1]], [self.height-self.true_pose[0]-1], color='red', marker="*", s=200, alpha=0.3)
		plt.scatter(self.X,self.Y,s=self.P,color="blue")
		plt.scatter([self.true_pose[1]], [self.height-self.true_pose[0]-1], color='red', marker="*", s=200)
		plt.show()

	def random_move(self):
		dy = random.choice([-1,0,1])
		dx = random.choice([-1,0,1])
		return dy,dx

	def run(self, num_steps=1):
		for i in range(num_steps):
			self.sense()
			dy, dx = self.random_move()
			self.move(dy,dx)
```
## helpers.py

```def normalize(grid):
    """
    Given a grid of unnormalized probabilities, computes the
    correspond normalized version of that grid. 
    """
    total = 0.0
    for row in grid:
        for cell in row:
            total += cell
    for i,row in enumerate(grid):
        for j,cell in enumerate(row):
            grid[i][j] = float(cell) / total
    return grid


def blur(grid, blurring):
    """
    Spreads probability out on a grid using a 3x3 blurring window.
    The blurring parameter controls how much of a belief spills out
    into adjacent cells. If blurring is 0 this function will have 
    no effect. 
    """
    height = len(grid)
    width  = len(grid[0])

    center_prob = 1.0-blurring
    corner_prob = blurring / 12.0
    adjacent_prob = blurring / 6.0

    window = [
            [corner_prob,  adjacent_prob,  corner_prob],
            [adjacent_prob, center_prob,  adjacent_prob],
            [corner_prob,  adjacent_prob,  corner_prob]
        ]
    new = [[0.0 for i in range(width)] for j in range(height)]
    for i in range(height):
        for j in range(width):
            grid_val = grid[i][j]
            for dx in range(-1,2):
                for dy in range(-1,2):
                    mult = window[dx+1][dy+1]
                    new_i = (i + dy) % height
                    new_j = (j + dx) % width
                    new[new_i][new_j] += mult * grid_val
    return normalize(new)

def is_robot_localized(beliefs, true_pos):
    """
    Returns None if the robot has no "strong opininon" about
    its belief. The robot has a strong opinion when the 
    size of it's best belief is greater than twice the size of 
    its second best belief.

    If it DOES have a strong opinion then this function returns 
    True if that opinion is correct and False if it is not.
    """
    best_belief = 0.0
    best_pos = None
    second_best = 0.0
    for y, row in enumerate(beliefs):
        for x, belief in enumerate(row):
            if belief > best_belief:
                second_best = best_belief
                best_belief = belief
                best_pos = (y,x)
            elif belief > second_best:
                second_best = belief
    if second_best <= 0.00001 or best_belief / second_best > 2.0:
        # robot thinks it knows where it is
        localized =  best_pos == true_pos
        return localized, best_pos
    else:
        # No strong single best belief
        return None, best_pos

def close_enough(g1, g2):
    if len(g1) != len(g2):
        return False
    if len(g1) == 0 or len(g1[0]) != len(g2[0]):
        return False
    for r1, r2 in zip(g1,g2):
        for v1, v2 in zip(r1, r2):
            if abs(v1 - v2) > 0.001:
                print(v1, v2)
                return False
    return True
    
´´´
