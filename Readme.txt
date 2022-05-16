GOAP Planner is used in relation with the state machine and uses pathfinding algorithm to search for the best plan
by relaxing the preconditions set upon the actions that the ship need to perform. Action is performed to relax
the precondition and get closer to the goal state. My GOAP is using a BFS search that is known for it's exponential
time space complexity however in this case, since the branching factor is small, the algorithm is still considered
efficient.

The Planner makes it very easy to expand upon the system and introduce more actions without the need to rebuild
most of the state machine code. As opposed to a general purpose finite state machine that would need to be changed
often as we introduce new actions, GOAP planner makes it such that distributed actions with set preconditions can
be performed without the developers having to disturb other actions associated with it.

For my collision avoidance algorithm, I use object at locaiton given in the gridnode.h file to update the ships' 
location. This in addition with a TMap<AShip*, bool> ReservedPath which stores the path that each ship takes to
reach its goal, Is used to detect any chenges and have a reactive type strategy.

As the ship moves it unreserves the path before it as it won't be used anymore. If there is a spot on the path that
is reserved by more than one ship, the algorithm checks for the ship that will finish its path quickly and priorities
its movement while all the other ship replan their path.

As this strategy involves replanning it can not be considered MCP strategy. To make it more efficient, we have some
pre checks to make sure there is no other ship at our goal node that is already collecting resources or wether the 
gold we were about to collect has already been collected by some other ship.

The collision avoidance algorithm is by no means optimal however it is complete in theory.