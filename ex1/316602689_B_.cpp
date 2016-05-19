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
performs BFS on the current graph.
*/
int _316602689_B::call_BFS(const pair<int, int>& s_location, const pair<int, int>& t_location){
	Vertex* s = m_graph[s_location];
	Vertex* t = m_graph[t_location];
	return BFS(s, t);
}

void _316602689_B::add_edge(Vertex* u, Vertex* v){
	if (u != nullptr && v != nullptr){
		u->add_edge(v);
		v->add_edge(u);
	}
}

/**
returns pointer to the wanted vertex. if does not exist, create it.
*/
_316602689_B::Vertex* _316602689_B::get_vertex(const pair<int, int>& location){
	if (m_graph.find(location) == m_graph.end()){
		m_graph[location] = new Vertex(location, IS_DOCKING(location) ? 0 : -1); // will be deleted in delete_graph func
	}
	return m_graph[location];
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
is called right after BFS.
returns a path from s to t which was found during BFS,
if there is no such path, returns an empty stack.
path_length == the returned value from BFS.
*/
deque<Direction> _316602689_B::get_stack_directions(const pair<int, int>& s_location, const pair<int, int>& t_location){

	Vertex* t = m_graph[t_location];

	deque<Direction> directions_stack;

	Vertex* prev = t;
	Vertex* cur = t->get_parent();

	while (cur != nullptr){
		Direction d = get_direction(cur, prev);
		directions_stack.push_front(d);
		prev = cur;
		cur = cur->get_parent();
	}

	return directions_stack;
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
returns the closest vertex to cur_vertex which satisfy pred.
if there is no vertex satisfies the pred, returns DOCKING_STATION
*/
_316602689_B::Vertex* _316602689_B::find_closest(Vertex* cur_vertex, function<bool(Vertex*)> pred){
	// initialize BFS
	init_BFS();

	cur_vertex->set_color(GREY);

	// do the BFS
	Vertex *cur = nullptr;
	Vertex* res = this->get_vertex(DOCKING_STATION);
	queue<Vertex*> bfs_queue;

	bfs_queue.push(cur_vertex);
	bool found_t = false;
	while (!bfs_queue.empty() && !found_t){
		cur = bfs_queue.front();
		for (Vertex *v : cur->get_adj_list()){
			if (v->get_color() == WHITE){
				v->set_color(GREY);
				bfs_queue.push(v);
				if (pred(v)){
					found_t = true;
					res = v;
				}
			}
		}
		bfs_queue.pop();
		cur->set_color(BLACK);
	}

	//cout << "getting to closest: (" << res->get_location().first << "," << res->get_location().second << ")" << endl;
	return res;
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

	// set the neighbors of the current vertex
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
		//cout << "has path2" << endl;
		if (m_path.front() != prevStep){
			m_has_path2 = false;
		}
		else {
			m_path.pop_front();
			if (m_path.empty()){
				m_has_path2 = false;
			}
		}
	}

	if (m_has_path){
		//cout << "has path1" << endl;
		if (m_path.empty()){
			if (IS_DOCKING(m_relative_docking_location)){
				return Direction::Stay;			
			}
			else{
				call_BFS(m_relative_docking_location, DOCKING_STATION);
				m_path = get_stack_directions(m_relative_docking_location, DOCKING_STATION);
				if (m_path.empty()){
					m_has_path = false;
					return Direction::Stay;
				}
				return m_path.front();
			}
		}
		else { // m_path is not empty
			if (IS_DOCKING(m_relative_docking_location)){
				return Direction::Stay;
			}
			if (prevStep == m_path.front()){
				m_path.pop_front();
				if (m_path.empty()){
					call_BFS(m_relative_docking_location, DOCKING_STATION);
					m_path = get_stack_directions(m_relative_docking_location, DOCKING_STATION);
					if (m_path.empty()){
						m_has_path = false;
						return Direction::Stay;
					}
					return m_path.front();
				}
				else{
					return m_path.front();
				}
				
			}
			else { // prevStep != m_path.top()
				call_BFS(m_relative_docking_location, DOCKING_STATION);
				m_path = get_stack_directions(m_relative_docking_location, DOCKING_STATION);
				if (m_path.empty()){
					m_has_path = false;
					return Direction::Stay;
				}
				return m_path.front();
			}
		}
	}

	if (!(IS_DOCKING(m_relative_docking_location))){
		int need_to_return = m_about_to_finish_flag ? min(m_steps_till_finish, (m_battery_level / m_config["BatteryConsumptionRate"])) : (m_battery_level / m_config["BatteryConsumptionRate"]);
		int bfs = call_BFS(m_relative_docking_location, DOCKING_STATION);
		deque<Direction> tmp = get_stack_directions(m_relative_docking_location, DOCKING_STATION);
		//cout << "check battery coond. bfs=" << bfs << ", needtoreturn=" << need_to_return << ", tmp.size=" << tmp.size() << endl;
		if (!tmp.empty() && need_to_return >= bfs && need_to_return <= bfs + 2){
			m_path = tmp;
			m_has_path = true;
			m_has_path2 = false;
			return m_path.front();
		}
	}

	if (m_has_path2){
		return m_path.front();
	}


	int max = cur_vertex->get_dirt_level();
	if (max > 0){
		return Direction::Stay;
	}
	int sum_dirt_in_area = max;
	Vertex *max_vertex = cur_vertex;
	for (Vertex* v : cur_vertex->get_adj_list())
	{
		// if there is a cell we havent visited yet, and we are not in a rush to finish, go to it
		if (v->get_dirt_level() == -1 && !m_about_to_finish_flag){ // we havent visited this vertex yet
			//cout << "getting first unvisited" << endl;
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
		pair<int, int> max_loc = find_closest(cur_vertex, [](Vertex* v){ return v->get_dirt_level() > 0; })->get_location();

		if (IS_DOCKING(max_loc)){ // if did not find such cell
			pair<int, int> planB_loc = find_closest(cur_vertex, [](Vertex* v){ return v->get_dirt_level() == -1; })->get_location();

			if (IS_DOCKING(planB_loc)){ // we have visited in every cell in the house, and there is no dust, so we finished cleaning - return to docking and stay there
				//cout << "finished simulation" << endl;
				call_BFS(m_relative_docking_location, DOCKING_STATION);
				m_path = get_stack_directions(m_relative_docking_location, DOCKING_STATION);
				if (m_path.empty()){
					return get_direction(cur_vertex, max_vertex);
				}
				m_has_path = true;
				return m_path.front();
			}

			else { // if we found such cell, go to it
				//cout << "go to the closest unvisited cell" << endl;
				call_BFS(m_relative_docking_location, planB_loc);
				m_path = get_stack_directions(m_relative_docking_location, planB_loc);
				if (m_path.empty()){
					return get_direction(cur_vertex, max_vertex);
				}
				m_has_path2 = true;
				return m_path.front();
			}
		}
		else { // we found a cell with maximum dust, go to it
			//cout << "getting closest vertex with dirt" << endl;
			call_BFS(m_relative_docking_location, max_loc);
			m_path = get_stack_directions(m_relative_docking_location, max_loc);
			if (m_path.empty()){
				return get_direction(cur_vertex, max_vertex);
			}
			m_has_path2 = true;
			return m_path.front();
		}
	}
	//cout << "getting max dirt neighbor" << endl;
	// all the neighbors have been visited, so go to the vertex with the maximal dirt in the area
	return get_direction(cur_vertex, max_vertex);
}

extern "C"{
	REGISTER_ALGORITHM(_316602689_B)
}