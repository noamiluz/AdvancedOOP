/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_B_H 
#define __316602689_B_H 

#include <stdio.h>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <iostream>
#include <limits.h>

#include "AbstractAlgorithm.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"

#define WHITE 'w'
#define GREY 'g'
#define BLACK 'b'
#define DOCKING_STATION pair<int, int>(0, 0)
#define IS_DOCKING(a) a.first == 0 && a.second == 0

using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making algorithms 
extern map<string, maker_t *, less<string> > factory;


/**
* _316602689_B class. A determenistic algorithm,
* that activate  by this order of prefernce:  West, South,
* North and East(if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_B : public AbstractAlgorithm{

	class Vertex{

		const pair<int, int> m_location; // location relative to docking
		vector<Vertex*> m_adj_list; // neighbors list
		int m_dirt_level; // current level of dirt. -1 if unknown.
		// for BFS & DIJKSTRA
		char m_color; // BLACK / GREY / WHITE (only for BFS)
		int m_distance; // edge distance (in BFS); max sum of dirt from source to this vertex (in DIJKSTRA)
		Vertex* m_parent; // pointer to previous vertex in the returned path.

	public:

		Vertex(const pair<int, int>& location, int dirt_level = -1) : m_location(location), m_dirt_level(dirt_level),
			m_color(WHITE), m_distance(INT_MAX), m_parent(nullptr) {}

		void add_edge(Vertex* v){
			if (find(m_adj_list.begin(), m_adj_list.end(), v) == m_adj_list.end()){ // check if exists already in adj_list
				m_adj_list.push_back(v);
			}
		}

		const pair<int, int>& get_location() const{
			return m_location;
		}

		const int get_dirt_level() const{
			return m_dirt_level;
		}

		void set_dirt_level(int dirt_level){
			m_dirt_level = dirt_level;
		}

		const char get_color() const{
			return m_color;
		}

		void set_color(char color){
			m_color = color;
		}

		const int get_distance() const{
			return m_distance;
		}

		void set_distance(int distance){
			m_distance = distance;
		}

		vector<Vertex*>& get_adj_list(){
			return m_adj_list;
		}

		Vertex* get_parent(){
			return m_parent;
		}

		void set_parent(Vertex* parent){
			m_parent = parent;
		}
	};

	class mycomparison { // function object for priority queue
	public:

		bool operator() (Vertex* lhs, Vertex* rhs) const {
			return lhs->get_distance() > rhs->get_distance();
		}
	};


	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	pair<int, int> m_relative_docking_location; // the PREV location(relative to docking location)
	bool m_about_to_finish_flag; // true iff the aboutToFinish has been called by the simulation
	bool m_has_path; // true iff the algorithm is in his way to return to docking.
	bool m_has_path2; // true iff the algorithm is in his way to another cell with positive dirt.
	int m_battery_level; // current battery level
	int m_steps_till_finish; // relevant if m_about_to_finish_flag is turned on. represents the number of steps until the simulator finishes.
	stack<Direction> m_path; // relevant if m_has_path is turned on. represent the stack of directions which the algorithm has to go until the flag is off.
	map<pair<int, int>, Vertex*> m_graph; // the graph that is being constructed during the simulation.


	void add_edge(Vertex* u, Vertex* v){
		if (u != nullptr && v != nullptr){
			u->add_edge(v);
			v->add_edge(u);
		}
	}

	/**
	returns pointer to the wanted vertex. if does not exist, create it.
	*/
	Vertex* get_vertex(const pair<int, int>& location){
		if (m_graph.find(location) == m_graph.end()){
			m_graph[location] = new Vertex(location, IS_DOCKING(location) ? 0 : -1); // will be deleted in delete_graph func
		}
		return m_graph[location];
	}


	/**
	init vertices values for BFS
	*/
	void init_BFS();

	/** 
	performs BFS on the current graph.
	returns the shortest distance (in edges) from s to t in the current graph. Meaning, how many steps it takes to get from s to t.
	ASSUME: s != t
	*/
	int BFS(Vertex* s, Vertex* t);

	/**
	init vertices values for DIJKSTRA
	*/
	void init_DIJKSTRA();

	/**
	performs DIJKSTRA on the current graph.
	returns the sum of steps in a path from s to t, in which the sum of dirt on it is maximized.
	ASSUME: s != t
	*/
	int DIJKSTRA(Vertex* s, Vertex* t);

	/**
	performs BFS on the current graph.
	*/
	int call_BFS(const pair<int, int>& s_location, const pair<int, int>& t_location){
		Vertex* s = m_graph[s_location];
		Vertex* t = m_graph[t_location];
		return BFS(s, t);
	}

	/**
	performs DIJKSTRA on the current graph.
	*/
	int call_DIJKSTRA(const pair<int, int>& s_location, const pair<int, int>& t_location){
		Vertex* s = m_graph[s_location];
		Vertex* t = m_graph[t_location];
		return DIJKSTRA(s, t);
	}


	/**
	returns the direction we have to go from cur to next.
	*/
	Direction get_direction(Vertex* prev, Vertex* cur);

	/**
	is called right after BFS / DIJKSTRA.
	returns a path from s to t which was found during BFS / DIJKSTRA (respectively),
	in which the sum of dirt is maximized, and is at most steps_left.
	if there is no such path, returns an empty stack.
	path_length == the returned value from BFS / DIJKSTRA (respectively).
	*/
	stack<Direction> get_stack_directions(Vertex* s, Vertex* t, int path_lenght, int steps_left);

	/**
	returns the best path from s to t, which its length is at most steps_left.
	if urgent == true, returns the shortest path (on edges).
	bfs is the result of performing BFS on the graph.
	dijkstra is the result of performing DIJKSTRA on the graph.
	ASSUME: s != t
	*/
	stack<Direction> choose_path(const pair<int, int>& s_location, const pair<int, int>& t_location, int bfs, int dijkstra,
		int steps_left, bool urgent = false);

	/**
	free graph memory
	*/
	void delete_graph();

	/**
	calculates the current location of the robot, relative to the docking station. 
	*/
	void get_curr_location(Direction prevStep);

	/**
	setting battery level.
	*/
	void set_battery_level();




public:

	_316602689_B() : m_relative_docking_location(DOCKING_STATION), m_about_to_finish_flag(false),
		m_has_path(false), m_has_path2(false), m_steps_till_finish(-1) {
		get_vertex(DOCKING_STATION);
	}

	~_316602689_B(){
		delete_graph();
	}

	_316602689_B(const _316602689_B&) = delete;

	_316602689_B& operator=(const _316602689_B&) = delete;

	// setSensor is called once(for each house) when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
		m_battery_level = m_config["BatteryCapacity"];
		m_relative_docking_location = DOCKING_STATION;
	}

	// setConfiguration is called once(for each house) when the Algorithm is initialized - see below 
	virtual void setConfiguration(map<string, int> config){
		m_config = config;
		m_battery_level = m_config["BatteryCapacity"];
	}

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep) override;

	// this method is called by the simulation either when there is a winner or 
	// when steps == MaxSteps - MaxStepsAfterWinner 
	// parameter stepsTillFinishing == MaxStepsAfterWinner 
	virtual void aboutToFinish(int stepsTillFinishing) override{
		m_about_to_finish_flag = true;
		m_steps_till_finish = stepsTillFinishing + 1;
	}

};

#endif // __316602689_B_H 
