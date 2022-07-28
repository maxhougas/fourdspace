Welcome to sweet, sweet four dimensional space. Grab something and hold on because it's going to be a bumpy ride.

This project is sort of half-way between a game and an educational tool. The idea is to develop a system in which the user can explore orientation and motion in four dimensional space. The other idea is to fly around and shoot the other guy with your gun. Ultimately I'd like to implement graphics in OpenGL, but that goal is beyond the horizon at this point.

For now the game is entirely text based. When you run 4d, you will be presented with a list of possible actions most of which are self explanatory, but here is a description anyway:

t) Fire Thrusters!
 You will be prompted to input the magnitude of acceleration; at the end of your turn your heading will be scaled by your acceleration and added to your velocity, which will then be added to your location. At the beginning of your next turn your acceleration will be reset to zero. The ship is capable of of accelerations between -1.5 and 1 inclusive.

r) Reorient Ship
 You will be prompted for angles theta,phi,psi. Theta is analogous to the azimuth in three dimensional spherical coordinates; phi is analogous to the elevation in that situation. Psi is the remaining angle corresponding to the remaining dimension. Given theta,phi,psi, the normalized cartesian equivalent is <cos(psi)cos(phi)cos(theta),cos(psi)cos(phi)sin(theta),cos(psi)sin(phi),sin(psi)>.

b) BREAKS!
 Sets your velocity and acceleration to zero.

n) Null orient ship
 Sets your relative i,j,k,l vectors back to their originals, that is <1,0,0,0>,<0,1,0,0>,<0,0,1,0>,<0,0,0,1>.

r) Review sensor output
 Prints some incomplete information about the other ships. If any of the other ships are within your short sensor range (less than 0.5), their full data will be printed out.

l) Look at
 You will be prompted to enter a point in the form of r,theta,phi,psi, this point is relative to the heading and location of your ship. If you're look at point is within 0.6 of an enemy, you will be given a readout detailing his location, orientation, velocity, and acceleration.

p) Print ship info
 Prints information about your ship's location, orientation, velocity, and acceleration.

w) Weapons
 You will be given the option to fire your scattergun or your rifle. The scatter gun fires in the hexidecitant (is that even the right word?) defined by your ships relative i,j,k,l vectors. It fires 30 shots which take random linear trajectories, persist for a distance of no more than 1.5 and then explode. The traveling bullet cannot hurt other ships, but if the explosion is centered within 0.3 of an enemy, you will wreck his whole day. The rifle fires along your relative i vector; any ships caught within 0.2 of the trajectory will have new and exciting holes drilled into them. The distance between an enemy and your rifle shot is computed by multiplying the angle between the bullet trajectory and the vector to the enemy ship by the distance to the enemy ship: this has the consequence of making the rifle much more finiky at close range.

g) Go
 End your turn.

c) Change controls
 Alter the input method for angles. Options are radians, radians/pi, and radians/(tau=2pi);

x) Exit
 I have no idea why you would ever want to do this.

You start at <0,0,0,0> and your opponent starts at <9,9,9,9>. There is only one opponent and he doesn't move or shoot, so if you went to public school feel free to work off that extra aggression.