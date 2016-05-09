/* nadavkareen 316602689; noamiluz 201307436 */


#include "316602689_B_.h"

/**
free graph memory
*/
void _316602689_B::delete_graph(){
	for (auto& pair : m_graph){
		delete pair.second;
	}
}

/**
init vertices values for BFS
*/
void _316602689_B::init_BFS(){
	for (auto& pair : m_graph){
		pair.second->set_color(WHITE);
		pair.second->set_distance(INT_MAX);
		pair.second->set_parent(nullptr);
	}
}

/**
init vertices values for DIJKSTRA
*/
void _316602689_B::init_DIJKSTRA(){
	for (auto& pair : m_graph){
		pair.second->set_color(WHITE);
		pair.second->set_distance(-1);
		pair.second->set_parent(nullptr);
	}
}

/**
returns the direction we have to go from cur to next.
*/
Direction _316602689_B::get_direction(Vertex* cur, Vertex* next){
	// calculate where is next in relation to cur
	pair<int, int> d = { cur->get_location().first - next->get_location().first, cur->get_location().second - next->get_location().second };
	if (d.first == 0){
		if (d.second == 1){ // next is left to cur
			return Direction::West;
		}
		if (d.second == -1) { // next is right to cur
			return Direction::East;
		}
	}
	if (d.second == 0){
		if (d.first == 1){ // next is above cur
			return Direction::North;
		}
		if (d.first == -1) { // next is below cur
			return Direction::South;
		}
	}
	return Direction::Stay; // will never come to this 
}

/**
performs BFS on the current graph.
returns the shortest distance (in edges) from s to t in the current graph. Meaning, how many steps it takes to get from s to t.
ASSUME: s != t
*/
int _316602689_B::BFS(Vertex* s, Vertex* t){

	// initialize BFS
	init_BFS();

	s->set_color(GREY);
	s->set_distance(0);
	s->set_parent(nullptr);

	// do the BFS
	Vertex *cur = nullptr;
	queue<Vertex*> bfs_queue;

	bfs_queue.push(s);
	bool found_t = false;

	while (!bfs_queue.empty() && !found_t){
		cur = bfs_queue.front();
		for (Vertex *v : cur->get_adj_list()){
			if (v->get_color() == WHITE){
				v->set_color(GREY);
				v->set_distance(cur->get_distance() + 1);
				v->set_parent(cur);
				bfs_queue.push(v);
				if (v == t){
					found_t = true;
				}
			}
		}
		bfs_queue.pop();
		cur->set_color(BLACK);
	}

	return t->get_distance();
}

/**
performs DIJKSTRA on the current graph.
returns length of a path from s to t, in which the sum of dirt on it is maximized.
ASSUME: s != t
*/
int _316602689_B::DIJKSTRA(Vertex* s, Vertex* t){

	init_DIJKSTRA();

	s->set_distance(s->get_dirt_level());
	s->set_parent(nullptr);

	// create a priority queue of all the vertices. now s is the prioritized.
	deque<Vertex*> queue; // priority MAX queue
	for (auto& pair : m_graph){
		queue.push_back(pair.second);
	}

	Vertex *cur;
	int alt;

	while (!queue.empty()){
		sort(queue.begin(), queue.end(), mycomparison());
		cur = queue.front();
		queue.pop_front();
		cur->set_color(BLACK);
		for (Vertex* v : cur->get_adj_list()){
			if (v->get_color() == BLACK){
				continue;
			}
			if (v->get_dirt_level() == -1){
				alt = cur->get_distance();
			}
			else {
				alt = cur->get_distance() + v->get_dirt_level();
			}
			if (alt > v->get_distance()){
				v->set_distance(alt);
				v->set_parent(cur);
			}
		}
	}

	int length = 0;
	cur = t; 
	while (cur != nullptr && cur != s){
		cur = cur->get_parent();
		length++;
	}

	return length;
}

/**
is called right after BFS / DIJKSTRA.
returns a path from s to t which was found during BFS / DIJKSTRA (respectively),
in which the sum of dirt is maximized, and is at most steps_left.
if there is no such path, returns an empty stack.
path_length == the returned value from BFS / DIJKSTRA (respectively).
*/
stack<Direction> _316602689_B::get_stack_directions(Vertex* s, Vertex* t, int path_length, int steps_left){

	int count = steps_left - path_length;
	stack<Direction> directions_stack;

	if (count < 0){
		return directions_stack;
	}

	Vertex* prev = t;
	Vertex* cur = t->get_parent();

	while (cur != nullptr){
		Direction d = get_direction(cur, prev);
		directions_stack.push(d);
		int dirt_level = cur->get_dirt_level();
		while (dirt_level > 0 && count > 0){
			directions_stack.push(Direction::Stay);
			dirt_level--;
			count--;
		}
		prev = cur;
		cur = cur->get_parent();
	}

	return directions_stack;
}


/**
returns the best path from s to t, which its length is at most steps_left.
if urgent == true, returns the shortest path (on edges).
bfs is the result of performing BFS on the graph.
dijkstra is the result of performing DIJKSTRA on the graph.
ASSUME: s != t
*/
stack<Direction> _316602689_B::choose_path(const pair<int, int>& s_location, const pair<int, int>& t_location, int bfs, int dijkstra,
	int steps_left, bool urgent){
	Vertex* s = m_graph[s_location];
	Vertex* t = m_graph[t_location];
	if (urgent){
		return get_stack_directions(s, t, bfs, bfs);
	}
	const stack<Direction>& bfs_stack = get_stack_directions(s, t, bfs, steps_left);
	const stack<Direction>& dijkstra_stack = get_stack_directions(s, t, dijkstra, steps_left);
	if (bfs_stack.empty()){
		return dijkstra_stack;
	}
	if (dijkstra_stack.empty()){
		return bfs_stack;
	}
	if (dijkstra_stack.size() > bfs_stack.size()){
		return dijkstra_stack;
	}
	return bfs_stack;
}

void _316602689_B::get_curr_location(Direction prevStep){
	switch (prevStep)// update the m_relative_docking_location
	{
	case Direction::North:{
		m_relative_docking_location.first -= 1;
		break;
	}
	case Direction::South:{
		m_relative_docking_location.first += 1;
		break;
	}
	case Direction::East:{
		m_relative_docking_location.second += 1;
		break;
	}
	case Direction::West:{
		m_relative_docking_location.second -= 1;
		break;
	}
	default:
		break;
	}
}

/**
setting battery level
*/
void _316602689_B::set_battery_level(){

	// case before two steps the robot was at 'D' (attention - m_relative_docking_location isn't update to the last step taken yet)
	if (IS_DOCKING(m_relative_docking_location)){ // for any LAST step that was taken, there is only a charging of battery
		m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
	}
	else{ // for any LAST step that was taken, there is only a consumption of battery
		m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
	}
}

/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_B::step(Direction prevStep){ 
	
	get_curr_location(prevStep); // get current location
	set_battery_level();
	if (m_about_to_finish_flag){
		m_steps_till_finish--;
	}
	if (m_battery_level >= m_config["BatteryCapacity"]){
		m_has_path = false;
	}

	// update graph for the current location
	SensorInformation s_i = m_sensor->sense();
	Vertex* cur_vertex = get_vertex(m_relative_docking_location);
	cur_vertex->set_dirt_level(s_i.dirtLevel); // set the current vertex's dirt level
	for (int i = 0; i < 4; i++)
	{
		if (!s_i.isWall[i]){
			Vertex* neighbor = nullptr;
			switch ((Direction)i){
			case Direction::North:
				neighbor = get_vertex(pair<int, int>(m_relative_docking_location.first - 1, m_relative_docking_location.second));
				break;
			case Direction::South:
				neighbor = get_vertex(pair<int, int>(m_relative_docking_location.first + 1, m_relative_docking_location.second));
				break;
			case Direction::East:
				neighbor = get_vertex(pair<int, int>(m_relative_docking_location.first, m_relative_docking_location.second + 1));
				break;
			case Direction::West:
				neighbor = get_vertex(pair<int, int>(m_relative_docking_location.first, m_relative_docking_location.second - 1));
				break;
			default:
				break;
			}
			add_edge(cur_vertex, neighbor);
		}
	}

	if (m_has_path2){
		if (m_path.top() != prevStep){
			m_has_path2 = false;
		}
		else {
			m_path.pop();
			if (m_path.empty()){
				m_has_path2 = false;
			}
		}
	}

	if (m_has_path){
		if (m_path.empty()){
			if (IS_DOCKING(m_relative_docking_location)){
				return Direction::Stay;			
			}
			else{
				int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
				int dijkstra = call_DIJKSTRA(m_relative_docking_location, DOCKING_STATION);
				m_path = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, (m_battery_level / m_config["BatteryConsumptionRate"]));
				if (m_path.empty()){
					m_has_path = false;
					return Direction::Stay;
				}
				return m_path.top();
			}
		}
		else { // m_path is not empty
			if (IS_DOCKING(m_relative_docking_location)){
				return Direction::Stay;
			}
			if (prevStep == m_path.top()){
				m_path.pop();
				if (m_path.empty()){
					int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
					int dijkstra = call_DIJKSTRA(m_relative_docking_location, DOCKING_STATION);
					m_path = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, (m_battery_level / m_config["BatteryConsumptionRate"]));
					if (m_path.empty()){
						m_has_path = false;
						return Direction::Stay;
					}
					return m_path.top();
				}
				else{
					return m_path.top();
				}
				
			}
			else { // prevStep != m_path.top()
				int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
				int dijkstra = call_DIJKSTRA(m_relative_docking_location, DOCKING_STATION);
				m_path = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, (m_battery_level / m_config["BatteryConsumptionRate"]));
				if (m_path.empty()){
					m_has_path = false;
					return Direction::Stay;
				}
				return m_path.top();
			}
		}
	}

	bool too_far = false;
	if (!(IS_DOCKING(m_relative_docking_location))){
		// calculate if we are too far from docking
		int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
		if (bfs >= m_config["MaxStepsAfterWinner"] || (m_about_to_finish_flag && bfs >= m_steps_till_finish)){
			too_far = true;
		}

		int dijkstra = call_DIJKSTRA(m_relative_docking_location, DOCKING_STATION);
		int approx = (m_battery_level / m_config["BatteryConsumptionRate"]);
		// check if urgent
		stack<Direction> tmp = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, approx, true);
		if (!tmp.empty() && approx - tmp.size() >= 0 && approx - tmp.size() < 2){
			m_path = tmp;
			m_has_path = true;
			m_has_path2 = false;
			return m_path.top();
		}
		// if not urgent
		tmp = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, approx);
		if (!tmp.empty() && approx - tmp.size() >= 0 && approx - tmp.size() < 4){
			m_path = tmp;
			m_has_path = true;
			m_has_path2 = false;
			return m_path.top();
		}

		if (m_about_to_finish_flag){
			// check if urgent
			stack<Direction> tmp = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, m_steps_till_finish, true);
			if (!tmp.empty() && m_steps_till_finish - tmp.size() >= 0 && m_steps_till_finish - tmp.size() < 2){
				m_path = tmp;
				m_has_path = true;
				m_has_path2 = false;
				return m_path.top();
			}
			// if not urgent
			tmp = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, m_steps_till_finish);
			if (!tmp.empty() && m_steps_till_finish - tmp.size() >= 0 && m_steps_till_finish - tmp.size() < 4){
				m_path = tmp;
				m_has_path = true;
				m_has_path2 = false;
				return m_path.top();
			}
		}
	}

	if (m_has_path2){
		return m_path.top();
	}

	///////////////DEBUG///////////
	if (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 3){
		cout << "";
	}
	///////////////////////////////

	int max = cur_vertex->get_dirt_level();
	int sum_dirt_in_area = max;
	Vertex *max_vertex = cur_vertex;
	for (Vertex* v : cur_vertex->get_adj_list())
	{
		// if the cell is too far from the docking (aka, if aboutToFinish is on, we cant reach docking), dont go there
		if (too_far){ 
			int bfs = call_BFS(v->get_location(), DOCKING_STATION);
			if (bfs >= m_config["MaxStepsAfterWinner"] || (m_about_to_finish_flag && bfs >= m_steps_till_finish)){
				continue;
			}
		}
		// if there is a cell we havent visited yet, go to it
		if (v->get_dirt_level() == -1){ // we havent visited this vertex yet
			return get_direction(cur_vertex, v);
		}
		sum_dirt_in_area += v->get_dirt_level(); // calculate the sum of dust around the current cell
		// find the cell with the maximum dust in the area and go to it
		if (v->get_dirt_level() > max){
			max = v->get_dirt_level();
			max_vertex = v;
		}
	}


	// if there is no dust around the current location
	if (sum_dirt_in_area == 0){
		int max_dirt = 0;
		pair<int, int> max_loc = DOCKING_STATION;
		// find a cell with the most dust in it
		for (auto& pair : m_graph){
			int bfs = call_BFS(pair.second->get_location(), { 0, 0 });
			if (bfs >= m_config["MaxStepsAfterWinner"] || (m_about_to_finish_flag && bfs >= m_steps_till_finish)){
				continue;
			}
			if (pair.second->get_dirt_level() > max_dirt){
				max_dirt = pair.second->get_dirt_level();
				max_loc = pair.first;
			}
		}

		if (IS_DOCKING(max_loc)){ // if did not find such cell
			pair<int, int> planB_loc = DOCKING_STATION;
			bool flag = false;
			// find a cell we haven't visited yet
			for (auto& pair : m_graph){
				for (Vertex* v : pair.second->get_adj_list()){
					int bfs = call_BFS(v->get_location(), { 0, 0 });
					if (bfs >= m_config["MaxStepsAfterWinner"] || (m_about_to_finish_flag && bfs >= m_steps_till_finish)){
						continue;
					}
					if (v->get_dirt_level() == -1){
						planB_loc = v->get_location();
						flag = true;
					}
					if (flag){
						break;
					}
				}
				if (flag){
					break;
				}
			}

			if (flag){ // if we found such cell, go to it
				int bfs = call_BFS(m_relative_docking_location, planB_loc);
				int dijkstra = call_DIJKSTRA(m_relative_docking_location, planB_loc);
				m_path = choose_path(m_relative_docking_location, planB_loc, bfs, dijkstra, m_about_to_finish_flag ? (m_steps_till_finish / 2) : ((m_battery_level / m_config["BatteryConsumptionRate"]) / 2));
				if (m_path.empty()){
					return get_direction(cur_vertex, max_vertex);
				}
				m_has_path2 = true;
				return m_path.top();
			}
			else{ // we have visited in every cell in the house, and there is no dust, so we finished cleaning - return to docking and stay there
				int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
				int dijkstra = call_DIJKSTRA(m_relative_docking_location, DOCKING_STATION);
				m_path = choose_path(m_relative_docking_location, DOCKING_STATION, bfs, dijkstra, (m_battery_level / m_config["BatteryConsumptionRate"]));
				if (m_path.empty()){
					return get_direction(cur_vertex, max_vertex);
				}
				m_has_path = true;
				return m_path.top();
			}
		}
		else { // we found a cell with maximum dust, go to it
			int bfs = call_BFS(m_relative_docking_location, max_loc);
			int dijkstra = call_DIJKSTRA(m_relative_docking_location, max_loc);
			m_path = choose_path(m_relative_docking_location, max_loc, bfs, dijkstra, m_about_to_finish_flag ? (m_steps_till_finish / 2) : ((m_battery_level / m_config["BatteryConsumptionRate"]) / 2));
			if (m_path.empty()){
				return get_direction(cur_vertex, max_vertex);
			}
			m_has_path2 = true;
			return m_path.top();
		}
	}

	// all the neighbors have been visited, so go to the vertex with the maximal dirt in the area
	return get_direction(cur_vertex, max_vertex);
}

extern "C"{
	REGISTER_ALGORITHM(_316602689_B)
}