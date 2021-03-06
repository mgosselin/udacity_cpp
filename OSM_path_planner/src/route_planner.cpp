#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &m_Model.FindClosestNode(start_x, start_y);
  	end_node   = &m_Model.FindClosestNode(end_x, end_y);

}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
	// float distance(Node other)
  	return end_node->distance(*node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
	// Node::FindNeighbors does emplace_back(new_neighbor) for each new neighbor node
	current_node->FindNeighbors();
    for(RouteModel::Node * indiv_node : current_node->neighbors) {
    	indiv_node->parent  = current_node;  // Set parent
      	indiv_node->h_value = CalculateHValue(indiv_node); // Set h_value
      	indiv_node->g_value = current_node->g_value + indiv_node->distance(*current_node); // Set g_value
      	indiv_node->visited = true;
      	this->open_list.push_back(indiv_node);
    }
}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

bool RoutePlanner::NodeCompare(const RouteModel::Node *node1, const RouteModel::Node *node2) {
	// A function which compares node1 and node2
  	// If g+h of node1 is greater than node2 returns true, else, false.
  	float f1 = node1->g_value + node1->h_value;
  	float f2 = node2->g_value + node2->h_value;
  	return f1 > f2;
}

RouteModel::Node *RoutePlanner::NextNode() {
  	std::sort(open_list.begin(), open_list.end(), NodeCompare);
  	RouteModel::Node *lowest_f_node = open_list.back();
    open_list.pop_back();
    return lowest_f_node;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
	// so long as we're not at the start node, keep going.
  	// NOTE: the start node's parent was initialzed as a
  	// nullptr and should have remained as such.
  	while(current_node->parent!=nullptr) {
      	// de-reference and add the current_node
      	path_found.push_back(*current_node);
      	// append to the total distance
        distance += current_node->parent->distance(*current_node);
      	// get the next node (actually, its pointer) along the path
      	current_node = current_node->parent;
    }
	// don't forget to add the start node (and de-reference it!)
    path_found.push_back(*current_node);
  	// reverse the path so it's in the correct order
  	std::reverse(path_found.begin(),path_found.end());
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    // TODO: Implement your solution here.

	// assign the start node as the current node
    RouteModel::Node *current_node = start_node;
	// mark the start node as visited
	current_node->visited = true;
	// add the start node to the open list
  	open_list.push_back(start_node);
  	// keep looping to search, until the open list is exhausted
	while(!open_list.empty() && current_node != nullptr) {
      	// Check if we're at the end
      	if(current_node == end_node) {
          	// If we're at the end, get the final path, store it in the model
			m_Model.path = ConstructFinalPath(current_node);
          	break;
        }
      	else {
            // we're not at the end, explore and add to the open list
            AddNeighbors(current_node);
            // get the next node, assign it as the current node
            current_node = NextNode();

        }
    }
}