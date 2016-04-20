/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef _SIMULATOR_H 
#define _SIMULATOR_H 

#include <stdio.h>
#include <iostream>

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"
#include "Sensor.h"
#include "House.h"
#include "Robot.h"
#include "FilesInfrastructure.h"

using namespace std;

#define PRINT_USAGE cout << "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>]" << endl

typedef AbstractAlgorithm *maker_t();
// our global factory for making algorithms 
map<string, maker_t *, less<string> > factory;


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
	const int m_num_of_algorithms;
	const int m_max_steps; // the max steps of the house this robot cleans.
	int m_winner_num_steps; // num of steps the winner has done during the simulation. (if there is no winner, remains MaxSteps)
	int m_not_active; // num of robots that are not active any more

public:
	Simulator(map<string, int>& config, vector<AbstractAlgorithm*>& algorithm_arr, vector<Sensor*>& sensor_arr, House* house) :
		m_config(config), m_steps(0), m_algorithm_arr(algorithm_arr), m_sensor_arr(sensor_arr),
		m_num_of_algorithms(algorithm_arr.size()), m_max_steps(house->get_max_steps()), m_winner_num_steps(house->get_max_steps()), m_not_active(0) {
		init_robot_arr(house);
	}

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


/**
* Main class. Contains functions that opens and loads
* files(config.ini, *.house, *.so), and check their validity.
* Also contains the command line proccesing function and the 
* output functions(printing scores and errors).
**/
class Main {

	vector<string> error_list; // error list to be printed at the end
	vector<string> house_names; // SORTED names of VALID houses files without '.house'
	vector<string> algorithm_names; // SORTED names of VALID algorithms files without '.so'
	vector<void*> dl_arr; // vector to hold handles for dynamic libs 
	
public:

	vector<string> get_house_names() const{
		return house_names;
	}

	// prints error list
	void print_errors(vector<string>& error_list);

	// checking if the content of configuration file is valid. If not, print message and return false.
	bool check_configurations_validity(const map<string, int>& config);

	// read from configuration file and return the configurations map
	map<string, int> get_configurations(const string path);

	// read from .house files and parse them into a house array;
	vector<House*> get_houses(string path);

	//function for completing miising cells in house matrix by ' '.
	//In addition, surranding the matrix with wall if needed. Also,
	//returns how many docking stations in the house, AFTER fixing
	int fix_house_matrix(string *matrix, int rows, int cols);

	// load .so files that represent algorithms
	// creates vector of algorithms (one of each type), and vector of sensors (one for each algorithm)
	tuple<vector<AbstractAlgorithm*>, vector<Sensor*>> get_algorithms_and_sensors(string path, map<string, int>& config);

	// parse the command line arguments
	// returns a tuple <config_path, house_path, algorithm_path>
	tuple<string, string, string> command_line_arguments(int argc, char* argv[]);

	// simulate the simulator
	void simulate(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms, string& house_name);

	// calculates the score matrix and prints it
	void score_simulation(vector<Simulator*>& sim_arr, map<string, int>& config, int num_of_houses, int num_of_algorithms);

	// trim title in the score matrix to be up to 9 chars and aligned to left
	string trim_title(string& title);

	// calculate average score (on all houses) of an algorithm with index 'index'
	double calc_avg(int** score_matrix, int index, int num_of_houses);
	
	// prints the score matrix according to given format.
	// prints errors after that, if exist.
	void print_score_and_errors(vector<Simulator*>& sim_arr, int** score_matrix);

	// freeing all the memory left to free in the program
	void deleting_memory(vector<Simulator*>& sim_arr, vector<House*>& house_arr, vector<AbstractAlgorithm*>& algorithm_arr, vector<Sensor*>& sensor_arr,
		int num_of_houses, int num_of_algorithms);
};

#endif // _SIMULATOR_H 
