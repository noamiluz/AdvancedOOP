/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_C_H 
#define __316602689_C_H 

#include "AbstractAlgorithm.h"
#include "GenericAlgorithm.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"


using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making algorithms 
extern map<string, maker_t *, less<string> > factory;

/**
* _316602689_A class. A determenistic algorithm,
* finds the closest unscanned node using bfs and clean it till amout_of_dirt - 1.
* Uses the battery fully, and come back to 'D'
* to charge it till the CapacityAmount if needed.
**/
class _316602689_C : public GenericAlgorithm {

	int m_prev_dirt; // maintains the last dirt amount sensed, to update the scanned house map. 
	pair<int, int> m_scan_map_location; // maintains the current location of the algorithm.
	pair<int, int> m_docking_station_location_in_scan_map; // keeps the 'D' location at the scaaned house map.
	bool m_needs_to_return; // a flag that turns on if there is a need to return to 'D' - finish/battery reasons.
	bool m_finished_cleaning; // a flag for finishing the house cleaning.
	vector<pair<int, int>> m_one_dirt_left_locations;
	bool m_needs_to_trap_left_dirt_on_prev_path; // a flag that turns on if we used half battery capacity, to try catch left dirt on prev path.
	map<pair<int, int>, char> m_scan_house_map;

	// an inner class to convert the scaaned house to a graph and 
	// to find shortest paths using bfs.
	class Vertex {

	public:
		vector<Vertex*> neighbors;
		Vertex* m_parent;
		char m_color; // 'w' = white ; 'g' = grey; 'b' = black
		int m_distance;

		Vertex() : m_parent(nullptr), m_color('w'), m_distance(UINT_MAX) {}

		~Vertex() {}
	};

	map<pair<int, int>, Vertex*> create_graph_from_scan_map(map<pair<int, int>, char>& scan_map);

	void delete_graph(map<pair<int, int>, Vertex*> graph);

	Direction bfs(map<pair<int, int>, Vertex*> graph, pair<int, int> s, Vertex* t, bool only_un_scanned_nodes);

	void set_battery_level();

	void update_relative_locations_and_stack(Direction prevStep);

	void init_map();

	void update_scan_house_map(SensorInformation info);

	map<pair<int, int>, char>& get_scan_house_map() {
		return m_scan_house_map;
	}

	const pair<int, int>& get_docking_station_location_in_scan_map() const{
		return m_docking_station_location_in_scan_map;
	}
	const pair<int, int>& get_scan_map_location() const{
		return m_scan_map_location;
	}

	Direction check_if_needs_to_return(int batteryLeft_or_maxStepsAfterWinner);

public:

	_316602689_C() : GenericAlgorithm(), m_prev_dirt(0), m_scan_map_location(0, 0),
		m_docking_station_location_in_scan_map(0, 0), m_needs_to_return(false), m_finished_cleaning(false),
		m_needs_to_trap_left_dirt_on_prev_path(false) {
		init_map();
	}

	~_316602689_C() {}

	_316602689_C(const _316602689_C&) = delete;

	_316602689_C& operator=(const _316602689_C&) = delete;

	virtual Direction step(Direction prevStep);

};



#endif // __316602689_C_H 
