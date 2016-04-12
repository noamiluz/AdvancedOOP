/* nadavkareen 316602689; noamiluz 201307436 */

#include <stdio.h>
#include <iostream>
#include <stack>

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"

using namespace std;


class House{
	const string m_short_name;
	const int m_max_steps;
	const int m_rows;
	const int m_cols;
	const pair<int, int> m_docking_station;
	string* m_house_matrix;
	const int m_sum_dirt; // sum of the dirt in the house
	bool m_flag; // true iff the aboutToFinish was called for this house

	// Returns the sum of all the dust in the house. <private method, called once by the constructor>
	int count_dirt() const;

public:
	House(const string& short_name, const int& max_steps, const int& rows, const int& cols,
		const pair<int, int>& docking_station, string* house_matrix) :
		m_short_name(short_name), m_max_steps(max_steps), m_rows(rows), m_cols(cols),
		m_docking_station(docking_station), m_house_matrix(house_matrix), m_sum_dirt(count_dirt()), m_flag(false) {}

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

	const int get_max_steps() const{
		return m_max_steps;
	}

	const int get_sum_dirt_in_house() const {
		return m_sum_dirt;
	}

	bool get_flag(){
		return m_flag;
	}

	void set_flag(bool b){
		m_flag = b;
	}
};


class Sensor : public AbstractSensor {
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

	SensorInformation my_sense(const pair<int, int>& position) const;
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
		m_curr_battary_level(init_battary_level), m_curr_location(house->get_house_docking_station()), m_is_active(true),
		m_this_num_of_steps(0), m_dirt_collected(0), m_position_in_competition(0), m_is_valid(true) {
		m_house = new House(*house);
	}

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

class FileParser {

public:

	// returns vector of the full paths to all the files in the 'path_of_directory' directory which
	// their names contains the suffix 'suffix' 
	// path_of_directory could be relative or absolute (and with or without '/')
	vector<string> get_file_paths(const string& path_of_directory, const string&& suffix);

	// receive a full path to a file and returns its base name
	string get_file_name(const string& full_path);

	// splits a string according to a delimiter. (from recitation)
	vector<string> split(const string &s, char delim);

	// cleans a string from unwanted whitespaces. (from recitation)
	string trim(string& str);

	// given a line read from the configuration file, update the configuration map. (from recitation)
	void processLine(const string& line, map<string, int> &config);
};

class Main {

	vector<string> error_list; // error list to be printed at the end
	vector<string> house_names; // SORTED names of VALID houses files without '.house'
	vector<string> algorithm_names; // SORTED names of VALID algorithms files without '.so'
	
public:

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
	// creates vector of algorithms (one of each type), and vector of sensors (one for every algorithm)
	tuple<vector<AbstractAlgorithm*>, vector<Sensor*>> get_algorithms_and_sensors(string path, map<string, int>& config);

	// parse the command line arguments
	// returns a tuple <config_path, house_path, algorithm_path>
	tuple<string, string, string> command_line_arguments(int argc, char* argv[]);

	// simulate the simulator
	void simulate(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms);

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
	void deleting_memory(vector<Simulator*> sim_arr, vector<House*>& house_arr, vector<AbstractAlgorithm*>& algorithm_arr, vector<Sensor*>& sensor_arr,
		int num_of_houses, int num_of_algorithms);
};

class FilesLister {

protected:
	vector<string> filesList_;
	string basePath_;

private:
	static string concatenateAbsolutePath(const string& dirPath, const string& fileName)
	{
		if (dirPath.empty())
			return fileName;
		if (dirPath.back() == '/')
			return dirPath + fileName;
		return dirPath + "/" + fileName;
	}

public:
	FilesLister(const string& basePath)
		: basePath_(basePath)
	{
		this->refresh();
	}

	virtual void refresh();

	vector<string> getFilesList(){
		return this->filesList_;
	}
};

class FilesListerWithSuffix : public FilesLister {
protected:
	void filterFiles();

	static inline bool endsWith(std::string value, std::string ending){
		if (ending.size() > value.size())
			return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	string suffix_;

public:
	FilesListerWithSuffix(const string& basePath, const string& suffix) : FilesLister(basePath)	, suffix_(suffix){
		this->filterFiles();
	}

	virtual void refresh() {
		FilesLister::refresh();
		this->filterFiles();
	}
};

class HousesLister : public FilesListerWithSuffix {
public:
	HousesLister(const string& basePath) : FilesListerWithSuffix(basePath, ".house"){}
};

class AlgorithmsLister : public FilesListerWithSuffix {
public:
	AlgorithmsLister(const string& basePath) : FilesListerWithSuffix(basePath, ".so"){}
};

class ConfigLister : public FilesListerWithSuffix {
public:
	ConfigLister(const string& basePath) : FilesListerWithSuffix(basePath, "config.ini"){}
};





// ex2 #1 algorithm
class _316602689_A : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	bool m_about_to_finish_flag;
public:

	_316602689_A(const AbstractSensor& sensor, map<string, int>& config) : m_about_to_finish_flag(false){
		setSensor(sensor);
		setConfiguration(config);
	}

	~_316602689_A() {
		delete m_sensor;
	}

	_316602689_A(const _316602689_A&) = delete;

	_316602689_A& operator=(const _316602689_A&) = delete;

	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
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
		m_about_to_finish_flag = true;
	}

};

// ex2 #2 algorithm
class _316602689_B : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	bool m_about_to_finish_flag;
	

public:

	_316602689_B(const AbstractSensor& sensor, map<string, int>& config) : m_about_to_finish_flag(false){
		setSensor(sensor);
		setConfiguration(config);
	}

	~_316602689_B() {
		delete m_sensor;
	}

	_316602689_B(const _316602689_B&) = delete;

	_316602689_B& operator=(const _316602689_B&) = delete;

	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
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
		m_about_to_finish_flag = true;
	}

};


// ex2 #3 algorithm
class _316602689_C : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	bool m_about_to_finish_flag;
	
public:

	_316602689_C(const AbstractSensor& sensor, map<string, int>& config): m_about_to_finish_flag(false) {
		setSensor(sensor);
		setConfiguration(config);
	}

	~_316602689_C() {
		delete m_sensor;
	}

	_316602689_C(const _316602689_C&) = delete;

	_316602689_C& operator=(const _316602689_C&) = delete;

	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
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
		m_about_to_finish_flag = true;
	}

};