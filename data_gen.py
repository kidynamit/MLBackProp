#!/usr/bin/python
'''
Created on 14 Feb 2014

@author: benjamin
'''
import numpy as np
from math import acos, pi

no_training_samples_per_dim = 50
no_inputs = 2
no_hidden = 3
no_layers = 1
no_out = 2
learning_rate = 0.3
mse_cutoff = 0.01
file_name = "training_data.txt"


def normalize(v):
    return v / v.max()
'''
generate many possible responses for the dot product
between the look direction and the direction to the mine/
super mine/rock.

Remember (first year / CS Games) the dot product between 
two normalized vectors ranges from -1 through to 1:
+1 if the two vectors point in the same direction
0 if the two vectors are orthogonal
-1 if the two vectors point in opposite directions
'''
dot_look_mine_vec = np.linspace(-1,1,no_training_samples_per_dim)
dot_look_super_mine_or_rock_vec = np.linspace(-1,1,no_training_samples_per_dim)
dist_mine_super_mine = np.linspace (0, 1, no_training_samples_per_dim)
dist_mine_rock = np.linspace (0, 1)
#if we're pointing towards the mine do not turn:
resp_dot_look_mine_vec = np.ones(len(dot_look_mine_vec))
for i in range(0,len(dot_look_mine_vec)):
    resp_dot_look_mine_vec[i] = 1 if dot_look_mine_vec[i] < 0.96 else 0  
#if we're pointing towards the super mine / rock turn sharply, 
#if we're pointing somewhere between perpendicular and the same direction stop turning:
resp_dot_look_super_mine_or_rock_vec = np.zeros(len(dot_look_super_mine_or_rock_vec))
for i in range(0,len(dot_look_super_mine_or_rock_vec)):
    resp_dot_look_super_mine_or_rock_vec[i] = 1 if dot_look_super_mine_or_rock_vec[i] > 0.5 else 0
 
f = open(file_name,'w')
f.write(str(no_training_samples_per_dim**no_inputs)+"\n")
f.write(str(no_inputs)+"\n")
f.write(str(no_hidden)+"\n")
f.write(str(no_layers)+"\n")
f.write(str(no_out)+"\n")
f.write(str(learning_rate)+"\n")
f.write(str(mse_cutoff)+"\n")
for m in range(0,no_training_samples_per_dim):
    for sm in range(0,no_training_samples_per_dim):
        f.write(str(dot_look_mine_vec[m]) + " " + 
                str(dot_look_super_mine_or_rock_vec[sm]) + "\n")
        iscolinear = abs (acos (dot_look_super_mine_or_rock_vec[sm]) - acos (dot_look_mine_vec[m])) < pi/12
        f.write(str(resp_dot_look_mine_vec[m] if resp_dot_look_super_mine_or_rock_vec[sm] == 0 and not iscolinear else 0) + " " + 
                str(resp_dot_look_super_mine_or_rock_vec[sm]) + "\n")
#print "Complete... Terminating"                  
f.close() 
