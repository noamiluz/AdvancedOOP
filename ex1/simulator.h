/* nadavkareen 316602689; noamiluz 201307436 */

#include <stdio.h>
#include <iostream>

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"

using namespace std;

class House;

class Sensor : public AbstractSensor{
	House* m_house;
	pair<int, int> m_curr_location;

public:

	Sensor() : m_house(nullptr), m_curr_location(0,0){
	}

	~Sensor(){}

	// returns the sensor's information of the current location of the robot
	virtual SensorInformation sense() const;

	House* get_house() const{
		return m_house;
	}

	void set_house(House* house){
		m_house = house;
	}

	const pair<int, int>& get_curr_location() const {
		return m_curr_location;
	}

	void set_curr_location(pair<int, int>& location){
		m_curr_location = location;
	}

};


class OurAlgorithm : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties

public:

	OurAlgorithm(const AbstractSensor& sensor, map<string, int>& config) {
		setSensor(sensor);
		setConfiguration(config);
	}

	~OurAlgorithm() {
		delete m_sensor;
	}

	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = const_cast<AbstractSensor*>(&sensor);
	}

	// setConfiguration is called once when the Algorithm is initialized - see below 
	virtual void setConfiguration(map<string, int> config){
		m_config = config;
	}

	// step is called by the simulation for each time unit
	virtual Direction step();

	// this method is called by the simulation either when there is a winner or 
	// when steps == MaxSteps - MaxStepsAfterWinner 
	// parameter stepsTillFinishing == MaxStepsAfterWinner 
	virtual void aboutToFinish(int stepsTillFinishing){

	}

	const AbstractSensor* get_sensor(){
		return m_sensor;
	}
};

class House{
	const string m_short_name;
	const string m_long_description;
	const int m_rows;
	const int m_cols;
	const pair<int, int> m_docking_station;
	string* m_house_matrix;
	const int m_sum_dirt; // sum of the dirt in the house

	// Returns the sum of all the dust in the house. <private method, called once by the constructor>
	int count_dirt() const;

public:
	House(const string& short_name, const string& long_description, const int& rows, const int& cols,
		const pair<int, int>& docking_station, string* house_matrix) :
		m_short_name(short_name), m_long_description(long_description), m_rows(rows), m_cols(cols),
		m_docking_station(docking_station), m_house_matrix(house_matrix), m_sum_dirt(count_dirt()) {}

	~House();

	House(const House& house);

	House& operator=(const House& house) = delete;

	string* get_house_matrix() const{
		return m_house_matrix;
	}

	const int& get_house_matrix_rows() const{
		return m_rows;
	}

	const int& get_house_matrix_cols() const{
		return m_cols;
	}

	const pair<int, int>& get_house_docking_station() const{
		return m_docking_station;
	}

	const string get_house_short_name() const {
		return m_short_name;
	}

	const string get_house_long_description() const {
		return m_long_description;
	}

	const int get_sum_dirt_in_house() const {
		return m_sum_dirt;
	}
};


class Robot{
	House* m_house; // the house 
	int m_curr_battary_level; // the current battary level of the robot

	pair<int, int> m_curr_location;// curr location of the robot
	bool m_is_active; // is the robot currently active in the simulation
	int m_this_num_of_steps; // how much steps this robot has done during the simulation (for the score formula)
	int m_dirt_collected; // how much dirt collected during the simulation of this robot
	int m_position_in_competition; // the position of this robot in the competition. could be 1,2,3,4 or 10.
	bool m_is_valid; // false iff the robot got stuck in a wall (bad behavior)

public:

	Robot(int init_battary_level, House* house) :
		m_house(house), m_curr_battary_level(init_battary_level), m_curr_location(m_house->get_house_docking_station()), m_is_active(true),
		m_this_num_of_steps(0), m_dirt_collected(0), m_position_in_competition(0), m_is_valid(true) { }

	~Robot(){
		delete m_house;
	}

	Robot(const Robot& robot) = delete;

	Robot& operator=(const Robot& other_robot) = delete;


	House* get_house(){
		return m_house;
	}

	const int& get_curr_battary_level() const{
		return m_curr_battary_level;
	}

	const bool is_active() const {
		return m_is_active;
	}

	void set_curr_battery_level(const int new_level){
		m_curr_battary_level = new_level;
	}


	void set_active(bool active){
		m_is_active = active;
	}

	void increment_num_of_steps(){
		m_this_num_of_steps++;
	}

	void set_num_of_steps(int steps){
		m_this_num_of_steps = steps;
	}

	void increment_dirt_collected(){
		m_dirt_collected++;
	}

	const int get_num_of_steps() const {
		return m_this_num_of_steps;
	}

	const int get_dirt_collected() const {
		return m_dirt_collected;
	}

	const int get_position_in_competition() const {
		return m_position_in_competition;
	}

	const bool is_valid() const{
		return m_is_valid;
	}

	void set_position_in_competition(int position){
		m_position_in_competition = position;
	}

	void set_valid(bool valid){
		m_is_valid = valid;
	}

	pair<int, int>& get_curr_location(){
		return m_curr_location;
	}

	void set_curr_location(pair<int, int>& location){
		m_curr_location = location;
	}
};



class Simulator{

	map<string, int> m_config; // configuration properties

	int m_steps; // num of steps the simulator did
	Robot*** m_robots_matrix; // matrix of pointers to Robots. each row represents an algorithm and each column represents a house
	AbstractAlgorithm** m_algorithm_arr;
	Sensor** m_sensor_arr;
	const int m_num_of_houses;
	const int m_num_of_algorithms;
	int m_winner_num_steps; // num of steps the winner has done during the simulation. (if there is no winner, remains MaxSteps)
	int m_not_active; // num of robots that are not active any more

public:
	Simulator(map<string, int>& config, AbstractAlgorithm** algorithm_arr,Sensor** sensor_arr, const int& num_of_houses,
		const int& num_of_algorithms, House** house_arr) :
		m_config(config), m_steps(0), m_robots_matrix(nullptr), m_algorithm_arr(algorithm_arr),m_sensor_arr(sensor_arr),
		m_num_of_houses(num_of_houses), m_num_of_algorithms(num_of_algorithms), m_winner_num_steps(config["MaxSteps"]), m_not_active(0) {
		init_robots_matrix(house_arr);
	}

	~Simulator();

	Simulator(const Simulator& simulator) = delete;
	Simulator& operator=(const Simulator&) = delete;

	// Initiate the field m_robots_matrix 
	void init_robots_matrix(House** house_arr);

	const int get_steps() const{
		return m_steps;
	}

	const int get_winner_num_steps() const {
		return m_winner_num_steps;
	}

	void set_winner_num_steps(int steps) {
		m_winner_num_steps = steps;
	}

	Robot*** get_robots_matrix(){
		return m_robots_matrix;
	}

	int get_not_active(){
		return m_not_active;
	}

	void increment_not_active(){
		m_not_active++;
	}

	Sensor** get_sensor_arr(){
		return m_sensor_arr;
	}

	AbstractAlgorithm** get_algorithm_arr(){
		return m_algorithm_arr;
	}
	// simulate a single steps of the simulation:
	// iterate over all houses, for each house runs a single steps of all the algorithms (in parallel).
	// if the flag about_to_finish == true, the function aboutToFinish(int stepsTillFinishing) of each algorithm is called.
	// rank_in_competition == 1,2,3,4 
	int simulate_step(int& rank_in_competition, bool about_to_finish);

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

// returns vector of the full paths to all the files in the 'path_of_directory' directory which their names contains the suffix 'suffix' 
// path_of_directory could be relative or absolute (and with or without '/')
vector<string> get_file_paths(string path_of_directory, string suffix);

// splits a string according to a delimiter. (from recitation)
vector<string> split(const string &s, char delim);

// cleans a string from unwanted whitespaces. (from recitation)
string trim(string& str);

// given a line read from the configuration file, update the configuration map. (from recitation)
void processLine(const string& line, map<string, int> &config);

// read from configuration file and return the configurations map
map<string, int> get_configurations(string path);

// read from .house files and parse them into a house array;
House** get_houses(char* path);

// check if house matrix has boundaries, otherwise, add walls. changing 
//found_docking to false in case docking station is at the the end
void fix_house_matrix(string *matrix, int rows, int cols, int& found_docking);