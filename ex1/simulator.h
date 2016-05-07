/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef _SIMULATOR_H 
#define _SIMULATOR_H 

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <functional>

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"
#include "Sensor.h"
#include "House.h"
#include "Robot.h"

using namespace std;

/**
* Simulator class. Represents a simulation
* that simulate an house cleaning by a few algorithms,
* and score them according to their performance. 
**/
class Simulator{

	map<string, int> m_config; // configuration properties

	int m_steps; 
	vector<Robot*> m_robot_arr; // vectors of Robots. one for each algorithm.
	vector<AbstractAlgorithm*> m_algorithm_arr;
	vector<Sensor*> m_sensor_arr;
	vector<Direction> m_prev_steps; // vector of prev steps, i.e m_prev_steps[i] is the last step simuletion done using m_algorithm_arr[i] 
	const int m_num_of_algorithms;
	const int m_max_steps; // the max steps of the house this robot cleans.
	int m_winner_num_steps; // num of steps the winner has done during the simulation. (if there is no winner, remains MaxSteps)
	int m_not_active; // num of robots that are not active any more

public:
	Simulator(map<string, int>& config, vector<AbstractAlgorithm*>& algorithm_arr, House* house);

	~Simulator();

	Simulator(const Simulator& other) : m_config(other.m_config), m_steps(other.m_steps), m_robot_arr(other.m_robot_arr),
		m_algorithm_arr(other.m_algorithm_arr), m_sensor_arr(other.m_sensor_arr), m_num_of_algorithms(other.m_algorithm_arr.size()),
		m_max_steps(other.m_max_steps), m_winner_num_steps(other.m_winner_num_steps), m_not_active(other.m_not_active) {}

	Simulator& operator=(const Simulator& other) = delete;

	// Initiate the field m_robots_matrix 
	void init_robot_arr(House* house_arr);

	const int get_steps() const{
		return m_steps;
	}

	const int get_winner_num_steps() const {
		return m_winner_num_steps;
	}

	void set_winner_num_steps(int steps) {
		m_winner_num_steps = steps;
	}

	vector<Robot*>& get_robot_arr(){
		return m_robot_arr;
	}

	int get_not_active(){
		return m_not_active;
	}

	void increment_not_active(){
		m_not_active++;
	}

	vector<Sensor*> get_sensor_arr(){
		return m_sensor_arr;
	}

	vector<AbstractAlgorithm*> get_algorithm_arr(){
		return m_algorithm_arr;
	}

	const int get_max_steps() const {
		return m_max_steps;
	}

	const int get_num_of_algorithms() const {
		return m_num_of_algorithms;
	}

	// simulate a single steps of the simulation:
	// iterate over all houses, for each house runs a single steps of all the algorithms (in parallel).
	// if the flag about_to_finish == true, the function aboutToFinish(int stepsTillFinishing) of each algorithm is called.
	// rank_in_competition == 1,2,3,4 
	int simulate_step(int& rank_in_competition, bool about_to_finish, string& message);

	// update all the robots that did not finish cleaning
	void finish_simulation();

	// calculates the score according to the given robot's properties
	int calculate_score(int position_in_competition, int winner_num_steps, int this_num_steps,
		int dirt_collected, int sum_dirt_in_house, bool is_back_in_docking) {
		return max(0,
			2000
			- (position_in_competition - 1) * 50
			+ (winner_num_steps - this_num_steps) * 10
			- (sum_dirt_in_house - dirt_collected) * 3
			+ (is_back_in_docking ? 50 : -200));
	}
};

#endif // _SIMULATOR_H 
