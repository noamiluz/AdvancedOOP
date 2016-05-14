/* nadavkareen 316602689; noamiluz 201307436 */
#include "316602689_C_.h"


/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_C::step(Direction prevStep) {

	// update the amount of dirt in the last location
	map<pair<int, int>, char>::iterator it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
	char ch = it->second;
	it->second = ((m_prev_dirt > 0) ? ('0' + m_prev_dirt) : ((ch == 'D' ? ch : ((ch == 'W') ? ch : ' '))));

	// get the dirt amount in order to update the dirt amount in prev location next round
	SensorInformation s_i_ = m_sensor->sense();
	m_prev_dirt = s_i_.dirtLevel;

	// setting battery, locations and scanned house map according to prev step
	set_battery_level();
	update_relative_locations_and_stack(prevStep);

	// check if there is a need to return to 'D'
	int stack_size = m_path_stack.size(); // the stack porpuse is only to give an upper bound over the distance
	Direction d = Direction::Stay;
	if (m_about_to_finish_flag || (!m_needs_to_return && m_battery_level <= stack_size * m_config["BatteryConsumptionRate"])){
		if (m_about_to_finish_flag){
			d = check_if_needs_to_return(min(m_battery_level, m_config["MaxStepsAfterWinner"])); // using 'd' only if m_few_battery == true

		}
		else{
			d = check_if_needs_to_return(m_battery_level); // using 'd' only if m_few_battery == true

		}
	}

	if (m_battery_level >= m_config["BatteryCapacity"] && !m_finished_cleaning){
		m_needs_to_return = false;
		m_needs_to_trap_left_dirt_on_prev_path = false; // already got to 'D' starting a new round
	}

	// check if need to "return" of its prev path to get to the one dirt amount location that already pass on them
	if (m_battery_level <= m_config["BatteryCapacity"] && !m_one_dirt_left_locations.empty()){
		m_needs_to_trap_left_dirt_on_prev_path = true;
	}
	else{
		m_needs_to_trap_left_dirt_on_prev_path = false;
	}

	// check if i got to a node that was in m_one_dirt_left_locations vector - and reamove it
	vector<pair<int, int>>::iterator result = find(m_one_dirt_left_locations.begin(), m_one_dirt_left_locations.end(), get_scan_map_location());
	if (result != m_one_dirt_left_locations.end()) {
		/* m_one_dirt_left_locations contains the curr location */
		m_one_dirt_left_locations.erase(result);
	}

	update_scan_house_map(s_i_);

	if (m_needs_to_return /*1*/){
		it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
		if (it->second == 'D'){
			// empty stack
			stack<Direction> s; // an empty stack
			m_path_stack.swap(s); // clear the m_path_stack
			return Direction::Stay;// should get here at his last step
		}
		else if (d == Direction::Stay){
			// create graph from the scaned map and run bfs
			if (get_scan_map_location() == get_docking_station_location_in_scan_map()){ // we already in 'D'
				return Direction::Stay;
			}

			auto graph = create_graph_from_scan_map(get_scan_house_map());
			d = bfs(graph, get_scan_map_location(), graph[get_docking_station_location_in_scan_map()], false);
			delete_graph(graph);
			return d;

		}
		else if (d != Direction::Stay){
			return d;
		}

	}
	// return to 'D' - not directly - catches left dirt abounded(if there is)
	else if (m_needs_to_trap_left_dirt_on_prev_path && !m_one_dirt_left_locations.empty()){ 
		// getting the shortest direction to one of the '1 amount of dirt' stay in the way
		vector<pair<int, int>>::iterator it;
		int shortest_path_size = -1;
		pair<int, int> closest_node_location;
		// finds the closest 1 amount dirt left location
		for (it = m_one_dirt_left_locations.begin(); it != m_one_dirt_left_locations.end(); ++it){
			auto graph = create_graph_from_scan_map(get_scan_house_map());
			d = bfs(graph, get_scan_map_location(), graph[*it], false);
			if (shortest_path_size == -1){
				shortest_path_size = graph[get_scan_map_location()]->m_distance;
				closest_node_location = *it;
			}
			else if (shortest_path_size > graph[get_scan_map_location()]->m_distance){
				shortest_path_size = graph[get_scan_map_location()]->m_distance;
				closest_node_location = *it;
			}
			delete_graph(graph);
		}

		// runs bfs to it
		auto graph = create_graph_from_scan_map(get_scan_house_map());
		d = bfs(graph, get_scan_map_location(), graph[closest_node_location], false);
		delete_graph(graph);
		return d;

	}

	else { // no need to return to 'D' yet

		// this location has left 1 amount of dirt - add it to the vector - so the algo could came back to it.
		if (s_i_.dirtLevel == 2){ 
			m_one_dirt_left_locations.push_back(pair<int, int>(get_scan_map_location()));
		}

		// at first pass on a location - cleans it till amount_of_dirt - 1.
		if (s_i_.dirtLevel > 1){ 
			return Direction::Stay;
		}
		else{ // current position is clean (almost - may contain 1 amount of dirt)
			// Stratey: using bfs, find the closest un scanned node, and retun the direction to it.
			// if there isn't such - the algorithm done cleaning the house, return to d using bfs. 

			// create graph, and calculate the direction to the closest un cleaned node from current location.
			auto graph = create_graph_from_scan_map(get_scan_house_map());
			d = bfs(graph, get_scan_map_location(), graph[get_scan_map_location()], true);
			delete_graph(graph);

			if (d == Direction::Stay){
				if (s_i_.dirtLevel > 0){ // this is the last dirty location
					return Direction::Stay;
				}

				// i.e - the algorithm finished cleaning, bfs to 'D' just at the first time the algo
				// finishes cleaning.
				m_finished_cleaning = true;
				m_needs_to_return = true;
				if (get_scan_map_location() != get_docking_station_location_in_scan_map()){
					auto graph = create_graph_from_scan_map(get_scan_house_map());
					d = bfs(graph, get_scan_map_location(), graph[get_docking_station_location_in_scan_map()], false);
					delete_graph(graph);
				}
			}
			return d;
		}
	}
	return Direction::Stay; // should never get here.

}


/**
* Function that sets the battary level according to two cases:
* 1. last step was in 'D', 2. otherwise.
**/
void _316602689_C::set_battery_level(){

	// case before two steps the robot was at 'D' (attention - m_relative_docking_location isn't update to the last step taken yet)
	bool wasInD = m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0;
	if (wasInD){ // for any LAST step that was taken, there is only a charging of battery
		m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
	}
	else{ // for any LAST step that was taken, there is only a consumption of battery
		m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
	}

}

/**
* Function that gets the prevStep done by simulation,
* and updates the relative location of the docking station,
* and push to steps stack the last step taken(used just to give an upper
* bound on the distance from 'D').
**/
void _316602689_C::update_relative_locations_and_stack(Direction prevStep){

	map<pair<int, int>, char>::iterator it;
	switch (prevStep)
	{
	case Direction::North:
		m_relative_docking_location.first -= 1;
		m_scan_map_location.first -= 1;

		it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
		if (it->second == 'D'){
			stack<Direction> s; // an empty stack
			m_path_stack.swap(s); // clear the m_path_stack
		}
		else{
			m_path_stack.push(Direction::South);
		}
		break;
	case Direction::South:
		m_relative_docking_location.first += 1;
		m_scan_map_location.first += 1;

		it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
		if (it->second == 'D'){
			stack<Direction> s; // an empty stack
			m_path_stack.swap(s); // clear the m_path_stack
		}
		else{
			m_path_stack.push(Direction::North);
		}
		break;
	case Direction::East:
		m_relative_docking_location.second += 1;
		m_scan_map_location.second += 1;

		it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
		if (it->second == 'D'){
			stack<Direction> s; // an empty stack
			m_path_stack.swap(s); // clear the m_path_stack
		}
		else{
			m_path_stack.push(Direction::West);
		}
		break;
	case Direction::West:
		m_relative_docking_location.second -= 1;
		m_scan_map_location.second -= 1;

		it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
		if (it->second == 'D'){
			stack<Direction> s; // an empty stack
			m_path_stack.swap(s); // clear the m_path_stack
		}
		else{
			m_path_stack.push(Direction::East);
		}
		break;
	default: // there is no need to update m_relative_docking_location
		break; // should not get here
	}
}

/**
* Function that inits the scanned house map,
* sets the docking station location to be 'D',
* and its surrounding to be unknown (i.e '*').
**/
void _316602689_C::init_map(){
	// setting the 'D' surrounding to be '*'
	m_scan_house_map.insert(make_pair(pair<int, int>(0, 1), '*'));// east location
	m_scan_house_map.insert(make_pair(pair<int, int>(0, -1), '*'));// west location
	m_scan_house_map.insert(make_pair(pair<int, int>(1, 0), '*'));// south location
	m_scan_house_map.insert(make_pair(pair<int, int>(-1, 0), '*'));// north location


	// setting 'D' in location [0,0] in the map 
	m_scan_house_map.insert(make_pair(pair<int, int>(0, 0), 'D'));
}

/**
* Function that updates the scanned house map -
* sets the docking station location to be 'D',
* sets a not wall but unknown amount of dirt to be '*',
* sets the ones with known amount of dirt to be '1' ... '9'.
**/
void _316602689_C::update_scan_house_map(SensorInformation info){

	// update current location
	map<pair<int, int>, char>::iterator it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second));
	it->second = (info.dirtLevel == 0 && it->second != 'D') ? ' ' : '0' + info.dirtLevel;

	//update current location surrounding
	// update East
	it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second + 1));
	if (it != get_scan_house_map().end()){
		char temp_ch = it->second;
		it->second = info.isWall[0] ? 'W' : temp_ch;
	}
	else{// needs to add the location to the map
		if (info.isWall[0]){
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second + 1), 'W'));
		}
		else{
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second + 1), '*'));
		}
	}

	// update West
	it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second - 1));
	if (it != get_scan_house_map().end()){
		char temp_ch = it->second;
		it->second = info.isWall[1] ? 'W' : temp_ch;
	}
	else{// needs to add the location to the map
		if (info.isWall[1]){
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second - 1), 'W'));
		}
		else{
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first, m_scan_map_location.second - 1), '*'));
		}
	}

	// update South
	it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first + 1, m_scan_map_location.second));
	if (it != get_scan_house_map().end()){
		char temp_ch = it->second;
		it->second = info.isWall[2] ? 'W' : temp_ch;
	}
	else{// needs to add the location to the map
		if (info.isWall[2]){
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first + 1, m_scan_map_location.second), 'W'));
		}
		else{
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first + 1, m_scan_map_location.second), '*'));
		}
	}

	// update North
	it = get_scan_house_map().find(pair<int, int>(m_scan_map_location.first - 1, m_scan_map_location.second));
	if (it != get_scan_house_map().end()){
		char temp_ch = it->second;
		it->second = info.isWall[3] ? 'W' : temp_ch;
	}
	else{// needs to add the location to the map
		if (info.isWall[3]){
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first - 1, m_scan_map_location.second), 'W'));
		}
		else{
			get_scan_house_map().insert(make_pair(pair<int, int>(m_scan_map_location.first - 1, m_scan_map_location.second), '*'));
		}
	}

}

/**
* Function that converts a map to a graph -
* while ignoring the values that are walls (i.e 'W').
**/
map<pair<int, int>, _316602689_C::Vertex*> _316602689_C::create_graph_from_scan_map(map<pair<int, int>, char>& scan_map){
	map<pair<int, int>, _316602689_C::Vertex*> graph;
	// creating the vertices
	map<pair<int, int>, char>::iterator it;
	for (it = scan_map.begin(); it != scan_map.end(); it++)
	{
		if (it->second != 'W'){
			graph[{it->first.first, it->first.second}] = new _316602689_C::Vertex();
		}

	}

	// updating the neighbors vector for each vertex
	for (auto& pair_ : graph){
		for (int i = 0; i < 4; i++)
		{
			switch ((Direction)i)
			{
			case Direction::North:{
				it = scan_map.find(pair<int, int>(pair_.first.first - 1, pair_.first.second));
				if (it != scan_map.end()){ // only if this neighbor exists
					if (it->second != 'W'){
						pair_.second->neighbors.push_back(graph[{ pair_.first.first - 1, pair_.first.second }]);
					}
				}
				break;
			}
			case Direction::South:{
				it = scan_map.find(pair<int, int>(pair_.first.first + 1, pair_.first.second));
				if (it != scan_map.end()){ // only if this neighbor exists
					if (it->second != 'W'){
						pair_.second->neighbors.push_back(graph[{ pair_.first.first + 1, pair_.first.second }]);
					}
				}
				break;
			}
			case Direction::East:{
				it = scan_map.find(pair<int, int>(pair_.first.first, pair_.first.second + 1));
				if (it != scan_map.end()){ // only if this neighbor exists
					if (it->second != 'W'){
						pair_.second->neighbors.push_back(graph[{ pair_.first.first, pair_.first.second + 1}]);
					}
				}
				break;
			}
			case Direction::West:{
				it = scan_map.find(pair<int, int>(pair_.first.first, pair_.first.second - 1));
				if (it != scan_map.end()){ // only if this neighbor exists
					if (it->second != 'W'){
						pair_.second->neighbors.push_back(graph[{ pair_.first.first, pair_.first.second - 1}]);
					}
				}
				break;
			}
			default:
				break;
			}
		}
	}

	return graph;
}

/**
* Function deleting a given graph.
**/
void _316602689_C::delete_graph(map<pair<int, int>, _316602689_C::Vertex*> graph){
	for (auto& pair : graph){
		delete pair.second;
	}
}

/**
* Function that finds shortest paths on a graph, used in two ways:
* The first one is when only_un_scanned_nodes is FALSE, and then the function finds
* the shortest path from the current location(i.e s), to the docking station node (i.e t).
* The second one is when only_un_scanned_nodes is TRUE, and then the function finds the direction FROM
* the current node (i.e t) to the closest node that is still dirty/un scanned yet.
**/
Direction _316602689_C::bfs(map<pair<int, int>, _316602689_C::Vertex*> graph, pair<int, int> s, _316602689_C::Vertex* t, bool only_un_scanned_nodes){
	// do the bfs
	t->m_color = 'g';
	t->m_distance = 0;

	map<pair<int, int>, _316602689_C::Vertex*>::iterator it;
	_316602689_C::Vertex *source = nullptr, *cur = nullptr, *un_scaaned_node = nullptr;
	queue<Vertex*> q;
	q.push(t);
	bool found_s = false;

	while (!q.empty() && !found_s){
		cur = q.front();
		for (Vertex *v : cur->neighbors){
			if (v->m_color == 'w'){
				v->m_color = 'g';
				v->m_distance = cur->m_distance + 1;
				v->m_parent = cur;
				q.push(v);
				if (!only_un_scanned_nodes){ // only if we ARE NOT in an un scanned nodes garph, stop when you get to s,
					//otherwise t is curr location and map the distance to all the reachable nodes
					if (graph[s] == v){
						found_s = true;
						source = v;
					}
				}
				else{ // in case only_un_scanned_nodes is true, we found the closest un scanned node 
					for (it = graph.begin(); it != graph.end(); it++)
					{
						if (it->second == v && (get_scan_house_map().at(it->first) != 'W') && (get_scan_house_map().at(it->first) != '0')
							&& (get_scan_house_map().at(it->first) != ' ') && (get_scan_house_map().at(it->first) != 'D')){
							found_s = true;
							un_scaaned_node = v; /*un scanned node, or a node that was laft before the robot finished cleaning(due to battary)*/
							break;
						}
					}
				}
			}
		}
		q.pop();
		cur->m_color = 'b';
	}

	// in case of a full graph - find location of parent
	if (!only_un_scanned_nodes){
		pair<int, int> parent_loc;
		for (auto& pair : graph){
			if (pair.second == source->m_parent){
				parent_loc = pair.first;
			}
		}

		// calculate where is parent in relation to s
		pair<int, int> d = { s.first - parent_loc.first, s.second - parent_loc.second };
		if (d.first == 0){
			if (d.second == 1){ // parent is left to s
				return Direction::West;
			}
			else if (d.second == -1){ // parent is right to s
				return Direction::East;
			}
		}
		if (d.second == 0){
			if (d.first == 1){ // parent is above s
				return  Direction::North;
			}
			else if (d.first == -1) { // parent is below s
				return  Direction::South;
			}
		}
	}

	// in case we want to find the direction to the closest un scanned node, from currernt location
	else{
		if (un_scaaned_node == nullptr){ // the algorithm finished cleaning the house
			return Direction::Stay;
		}

		pair<int, int> child_loc;
		while (un_scaaned_node->m_parent != t){
			un_scaaned_node = un_scaaned_node->m_parent;
		}

		for (auto& pair : graph){
			if (pair.second == un_scaaned_node){
				child_loc = pair.first;
			}
		}

		// calculate where is child in relation to t
		pair<int, int> d = { s.first - child_loc.first, s.second - child_loc.second };
		if (d.first == 0){
			if (d.second == 1){ // child is left to t
				return Direction::West;
			}
			else if (d.second == -1){ // child is right to t
				return Direction::East;
			}
		}
		if (d.second == 0){
			if (d.first == 1){ // child is above s
				return  Direction::North;
			}
			else if (d.first == -1) { // child is below s
				return  Direction::South;
			}
		}

	}
	return Direction::Stay; // should never get here
}

/**
* Function that called one the flag 'about_to_finish' is turned on,
* or one we arrived to the upper bound for the distance from current location
* to docking station. This function checks if we have enough battary/steps(afterWinner)
* to continue - if yes, returns the direction, otherwise sets the TRUE the m_needs_to_return flag.
**/
Direction _316602689_C::check_if_needs_to_return(int batteryLeft_or_maxStepsAfterWinner){
	// create graph from the scaned map and run bfs
	int shortest_distance;

	Direction d = Direction::Stay;
	if (get_scan_map_location() != get_docking_station_location_in_scan_map()){
		auto graph = create_graph_from_scan_map(get_scan_house_map());
		d = bfs(graph, get_scan_map_location(), graph[get_docking_station_location_in_scan_map()], false);

		shortest_distance = graph[get_scan_map_location()]->m_distance;
		delete_graph(graph);
	}
	else{
		shortest_distance = 0;
	}

	// in case we have exactly the battary to came back OR the number of steps left after winner is less/equal to
	// the shortest path - come back to 'D' 
	if (batteryLeft_or_maxStepsAfterWinner <= (shortest_distance + 1) * m_config["BatteryConsumptionRate"] ||
		batteryLeft_or_maxStepsAfterWinner <= (shortest_distance + 1)){
		m_needs_to_return = true;
	}

	return d;
}


extern "C"{
	REGISTER_ALGORITHM(_316602689_C)
}