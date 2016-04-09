/* nadavkareen 316602689; noamiluz 201307436 */

#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <time.h>
#include <iomanip>

#include "simulator.h"

#define PRINT_USAGE cout << "Usage: simulator [­config <config path>] [­house_path <house path>] [­algorithm_path <algorithm path>]" << endl

using namespace std;


// returns the sensor's information of the current location of the robot
// Assumption: m_matrix[x][y] != 'W'
SensorInformation Sensor::sense() const {
	SensorInformation result;
	int x = m_curr_location.first;
	int y = m_curr_location.second;

	if (x - 1 >= 0){
		if (this->m_house->get_house_matrix()[x - 1][y] == 'W'){
			result.isWall[(int)Direction::North] = true;
		}
		else {
			result.isWall[(int)Direction::North] = false;
		}
	}
	if (x + 1 < this->m_house->get_house_matrix_rows()){
		if (this->m_house->get_house_matrix()[x + 1][y] == 'W'){
			result.isWall[(int)Direction::South] = true;
		}
		else {
			result.isWall[(int)Direction::South] = false;
		}
	}
	if (y - 1 >= 0){
		if (this->m_house->get_house_matrix()[x][y - 1] == 'W'){
			result.isWall[(int)Direction::West] = true;
		}
		else {
			result.isWall[(int)Direction::West] = false;
		}
	}
	if (y + 1 < this->m_house->get_house_matrix_cols()){
		if (this->m_house->get_house_matrix()[x][y + 1] == 'W'){
			result.isWall[(int)Direction::East] = true;
		}
		else {
			result.isWall[(int)Direction::East] = false;
		}
	}
	result.dirtLevel = this->m_house->get_house_matrix()[x][y] == ' ' || this->m_house->get_house_matrix()[x][y] == 'D' ? 0 : this->m_house->get_house_matrix()[x][y] - '0';
	return result;
}


// step is called by the simulation for each time unit
// Naive step : chooses randomly where to go (from the current valid directions)
Direction OurAlgorithm::step() {
	SensorInformation s_i = m_sensor->sense();
	int i = rand() % 5;
	while (i != (int)Direction::Stay && s_i.isWall[i]){
		i = rand() % 5;
	}
	return (Direction)i;
}

House::~House(){
	delete[] m_house_matrix;
}

House::House(const House& house) :
m_short_name(house.m_short_name), m_max_steps(house.m_max_steps), m_rows(house.m_rows), m_cols(house.m_cols),
m_docking_station(house.m_docking_station), m_sum_dirt(house.m_sum_dirt){

	m_house_matrix = new string[m_rows];
	for (int i = 0; i < m_rows; i++){
		m_house_matrix[i] = house.m_house_matrix[i];
	}

}

// Returns the sum of all the dust currently in the house.
int House::count_dirt() const{
	int result = 0;
	for (int i = 0; i < m_rows; i++)
	{
		if (m_house_matrix[i].empty()){
			continue;
		}
		for (int j = 0; j < m_cols; j++)
		{

			if ((unsigned)j < m_house_matrix[i].length() && m_house_matrix[i][j] > 48 && m_house_matrix[i][j] < 58){ // in 1,...,9
				result += (int)(m_house_matrix[i][j] - '0');
			}
		}
	}
	return result;
}

Simulator::~Simulator(){
	// delete robots array
	for (int i = 0; i < m_num_of_algorithms; i++){
		for (int j = 0; j < m_num_of_houses; j++){
			delete m_robots_matrix[i][j];
		}
		delete[] m_robots_matrix[i];
	}
	delete[] m_robots_matrix;
}

//function inits the maiin robots matrix - Initializes each robot in
//the matrix with his specific house and algorithm.
void Simulator::init_robots_matrix(const vector<House*>& house_arr) {
	m_robots_matrix = new Robot**[m_num_of_algorithms];

	for (int i = 0; i < m_num_of_algorithms; i++) {
		m_robots_matrix[i] = new Robot*[m_num_of_houses];
		for (int j = 0; j < m_num_of_houses; j++) {	
			House* robot_house = new House(*(house_arr[j]));

			m_robots_matrix[i][j] = new Robot(m_config["BatteryCapacity"], robot_house);
			// new Robot is deleted in the destructor of Simulator, new House is deleted in the destructor of Robot
		}
	}
}


//simulator step function - iterate over robots matrix, and
//simulates a step for the active ones. Returns cuurent rankInCompetition
//(an int) achieved.
int Simulator::simulate_step(int& rank_in_competition, bool about_to_finish){
	m_steps++; // increment the number of steps
	Robot * cur_robot;
	bool is_someone_finished = false;
	int num_of_winners_this_step = 0;
	for (int j = 0; j < m_num_of_houses; j++)
	{
		for (int i = 0; i < m_num_of_algorithms; i++)
		{
			cur_robot = m_robots_matrix[i][j];
			//init sensor house before activating algorithm
			m_sensor_arr[i]->set_house(cur_robot->get_house());
			m_sensor_arr[i]->set_curr_location(cur_robot->get_curr_location());

			if (!cur_robot->is_active()){ // if the robot is not active any more, continue
				continue;
			}

			if (m_steps > cur_robot->get_house()->get_max_steps()){
				cur_robot->set_active(false);
				continue;
			}

			if (about_to_finish){
				m_algorithm_arr[i]->aboutToFinish(m_config["MaxStepsAfterWinner"]);
			}

			Direction d = m_algorithm_arr[i]->step(); // ask the algorithm what direction to go
			pair<int, int> next_loc;
			House* cur_house = cur_robot->get_house(); // get the house of the current robot
			const pair<int, int>& cur_loc = cur_robot->get_curr_location(); // get the current location of the robot

			switch (d){ // calculate the next location
			case Direction::North:
				next_loc = { cur_loc.first - 1, cur_loc.second };
				break;
			case Direction::East:
				next_loc = { cur_loc.first, cur_loc.second + 1 };
				break;
			case Direction::South:
				next_loc = { cur_loc.first + 1, cur_loc.second };
				break;
			case Direction::West:
				next_loc = { cur_loc.first, cur_loc.second - 1 };
				break;
			default:
				next_loc = cur_loc;
				break;
			}
			char ch = cur_house->get_house_matrix()[next_loc.first][next_loc.second]; // get the matrix item in the next location
			cur_robot->set_curr_location(next_loc); // set the next location as the current one
			cur_robot->increment_num_of_steps(); // increment the number of steps the robot has done

			if (ch == 'W'){ // if there is a wall in the next location
				cur_robot->set_active(false);
				m_not_active++;
				cur_robot->set_valid(false);
				cout << "Algorithm [" << i << "] on House [" << cur_house->get_house_short_name() <<
					"] has made an invalid step: Moved to a wall in the position (" << next_loc.first <<
					"," << next_loc.second << ")." << endl << "Simulation stopped for this robot." << endl;
				continue;
			}

			// if the current location is a docking station, increment the curr_battery_level by RechargeRate.
			// if the battery is full do not increment.
			if (cur_house->get_house_matrix()[next_loc.first][next_loc.second] == 'D'){
				// if the current move was 'stay in docking station' do not decrement battery
				if (d == Direction::Stay){ // if the current move was 'stay in docking station' do not decrement battery
					cur_robot->set_curr_battery_level(min(m_config["BatteryCapacity"], cur_robot->get_curr_battary_level() + m_config["BatteryRechargeRate"]));
				}

				else { // if the current move was to docking station decrement battery.
					cur_robot->set_curr_battery_level(min(m_config["BatteryCapacity"], cur_robot->get_curr_battary_level() - m_config["BatteryConsumptionRate"]));
				}

				// if the robot has finished cleaning the house
				if (cur_robot->get_dirt_collected() == cur_robot->get_house()->get_sum_dirt_in_house()){
					cur_robot->set_position_in_competition(rank_in_competition); // update the cur_robot's position in the competition
					if (m_winner_num_steps == -1) { // the first robot to finish has to update the winner_num_steps field
						m_winner_num_steps = m_steps;
					}
					is_someone_finished = true;
					cur_robot->set_active(false);
					m_not_active++;
					num_of_winners_this_step++;
				}
				continue;
			}
			// if the current move was  docking station, and the next one isn't docking station, charge the battary and do not decrement.
			else if (cur_house->get_house_matrix()[next_loc.first][next_loc.second] != 'D' && cur_house->get_house_matrix()[cur_loc.first][cur_loc.second] == 'D'){
				cur_robot->set_curr_battery_level(min(m_config["BatteryCapacity"], cur_robot->get_curr_battary_level() + m_config["BatteryRechargeRate"]));
			}
			else { // if the current location is not on a docking station, decrement the curr_battery_level by ConsumptionRate.
				// if the battery level is 0 do not decrement.
				cur_robot->set_curr_battery_level(max(0, cur_robot->get_curr_battary_level() - m_config["BatteryConsumptionRate"]));
			}

			// clean one dust level in the next spot and update the matrix. If there is no dust, do nothing.
			cur_house->get_house_matrix()[next_loc.first][next_loc.second] = ch < 49 || ch > 57 ? ' ' : (char)((cur_house->get_house_matrix()[next_loc.first][next_loc.second] - 1));
			if (ch > 48 && ch < 58){ // ch = 1,...,9
				cur_robot->increment_dirt_collected(); // increment the dirt collected by this robot
			}

			if (cur_robot->get_curr_battary_level() == 0){ // the robot got stuck with battery 0
				cur_robot->set_active(false);
				m_not_active++;
				cur_robot->set_position_in_competition(10);
				cout << "Algorithm [" << i << "] on House [" << cur_house->get_house_short_name() <<
					"] has run out of battery." << endl << "Simulation stopped for this robot." << endl;
				continue;

			}

		}
	}

	if (is_someone_finished){ // if there were robots who finished in this step increment the rank
		int ret = rank_in_competition;
		rank_in_competition += num_of_winners_this_step;
		return min(4, ret);
	}
	return rank_in_competition;
}

// update all the robots that did not finish cleaning
void Simulator::finish_simulation(){
	Robot * cur_robot;
	for (int j = 0; j < m_num_of_houses; j++)
	{
		for (int i = 0; i < m_num_of_algorithms; i++)
		{
			cur_robot = m_robots_matrix[i][j];
			// if the robot is not active any more or it didnt run out of battery, meaning it either finished, or invalid, we continue
			if (!cur_robot->is_valid() || (cur_robot->get_position_in_competition() > 0 && cur_robot->get_position_in_competition() < 10)){
				continue;
			}
			// disactive the robot and set its position to be 10
			cur_robot->set_active(false);
			cur_robot->set_position_in_competition(10);
			cur_robot->set_num_of_steps(m_steps); // set this_num_steps to be simulation_stpes
		}
	}
}


// returns vector of the full paths to all the files in the 'path_of_directory' directory which
// their names contains the suffix 'suffix' 
// path_of_directory could be relative or absolute (and with or without '/')
vector<string> FileParser::get_file_paths(const string& path_of_directory, const string&& suffix){
	string str_command = "find " + path_of_directory + " -name \"" + suffix + "\" > ./\"out.txt\"";
	const char * command = str_command.c_str();
	system(command); // execute the command on linux. 
	//will create a file out.txt in the current directory with the output of the command.
	ifstream fin;
	fin.open("./out.txt");
	string line;
	vector<string> paths; // deleted in the caller function
	while (getline(fin, line)){
		paths.push_back(line);
	}
	fin.close();
	system("rm ./out.txt"); // delete the out.txt file
	return paths;
}

// receive a full path to a file and returns its base name
string FileParser::get_file_name(const string& full_path){
	return string(find_if(full_path.rbegin(), full_path.rend(),
		[](const char c){return c == '/'; }).base(), full_path.end());
}


// splits a string according to a delimiter. (from recitation)
vector<string> FileParser::split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

// cleans a string from unwanted whitespaces. (from recitation)
string FileParser::trim(string& str) {
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);         //suffixing spaces
	return str;
}

// given a line read from the configuration file, update the configuration map. (from recitation)
void FileParser::processLine(const string& line, map<string, int> &config)
{
	vector<string> tokens = split(line, '=');
	if (tokens.size() != 2)
	{
		return;
	}
	config[trim(tokens[0])] = stoi(trim(tokens[1]));
}


// prints error list
void Main::print_errors(vector<string>& error_list){
	for (int i = 0; i < error_list.size(); i++)
	{
		cout << error_list[i] << endl;
	}
}

// checking if the content of configuration file is valid. If not, print message and return false.
bool Main::check_configurations_validity(const map<string, int>& config){
	string temp;
	int count_missing = 0;
	if (config.find("MaxStepsAfterWinner") == config.end()){
		count_missing++;
		temp += "MaxStepsAfterWinner, ";
	} 
	if (config.find("BatteryCapacity") == config.end()){
		count_missing++;
		temp += "BatteryCapacity, ";
	}
	if (config.find("BatteryConsumptionRate") == config.end()){
		count_missing++;
		temp += "BatteryConsumptionRate, ";
	}
	if (config.find("BatteryRechargeRate") == config.end()){
		count_missing++;
		temp += "BatteryRechargeRate, ";
	}
	if (count_missing == 0){
		return true;
	}
	cout << "config.ini missing " << count_missing << " parameter(s): " << string(temp, 0, temp.length() - 2) << endl;
	return false;
}


// read from configuration file and return the configurations map
map<string, int> Main::get_configurations(const string path){
	FileParser fp;
	map<string, int> config;
	map<string, int> empty_config;
	string full_path;
	// getting the full path
	vector<string> result = fp.get_file_paths(path, "config.ini");
	if (result.empty()){
		PRINT_USAGE;
		return empty_config;
	}
	else {
		full_path = result[0];
	}

	// read from file
	ifstream fin;
	fin.open(full_path);
	if (!fin.is_open()){
		cout << "config.ini exists in '" << full_path << "' but cannot be opened" << endl;
		return empty_config;
	}
	string line;
	while (getline(fin, line)){
		fp.processLine(line, config);
	}
	fin.close();
	if (!check_configurations_validity(config)){
		return empty_config;
	}
	return config;
}

// read from .house files and parse them into a house array;
vector<House*> Main::get_houses(string path) {
	FileParser fp;
	vector<House*> house_arr; // deleted in the end of main()
	vector<House*> empty;

	// getting the full path
	vector<string> result = fp.get_file_paths(path, "*.house");
	if (result.empty()){
		PRINT_USAGE;
		return empty;
	}

	// sort result vector according to file name
	sort(result.begin(), result.end(), [&fp](const string s1, const string s2){ return fp.get_file_name(s1).compare(fp.get_file_name(s2)) < 0; });

	int num_of_houses = result.size();

	ifstream fin;
	string name, line;
	int r, c, max_steps;
	string* matrix;
	pair<int, int> docking;
	string::size_type index;
	for (int i = 0; i < num_of_houses; i++) // iterate on the houses in sorted order
	{
		fin.open(result[i]);
		if (!fin.is_open()){
			error_list.push_back(fp.get_file_name(result[i]) + ": cannot open file");
			continue;
		}
		getline(fin, name); // getting name
		getline(fin, line); 
		max_steps = atoi(line.c_str()); // getting max_steps
		if (max_steps < 0){
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 2 in house file shall be a positive number, found: " + line);
			continue;
		}
		getline(fin, line);
		r = atoi(line.c_str()); // getting r
		if (r < 0){
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 3 in house file shall be a positive number, found: " + line);
			continue;
		}
		getline(fin, line);
		c = atoi(line.c_str()); // getting c
		if (c < 0){
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 4 in house file shall be a positive number, found: " + line);
			continue;
		}

		matrix = new string[r]; // deleted in the destructor of House
		for (int j = 0; j < r && getline(fin, line); j++)
		{
			matrix[j] = string(line, 0, c);
			//found_docking += count(matrix[j].begin(), matrix[j].end(), 'D');
		}
		int count_docking = fix_house_matrix(matrix, r, c);
		if (count_docking == 0 || count_docking > 1){
			if (count_docking == 0){
				error_list.push_back(fp.get_file_name(result[i]) + ": missing docking station (no D in house)");
			}
			else if (count_docking > 1){
				error_list.push_back(fp.get_file_name(result[i]) + ": too many docking stations (more than one D in house)");
			}
			delete[] matrix;
			continue;
		}

		// the house is valid. There is one docking station
		for (int j = 0; j < r; j++){
			index = matrix[j].find("D"); // finding whether the docking station is in this line
			if (index != string::npos){
				docking.first = j;
				docking.second = index;
			}
		}
		house_names.push_back(fp.get_file_name(string(result[i], 0, result[i].length() - 6))); // push valid houses file name 
		house_arr.push_back(new House(name, max_steps, r, c, docking, matrix)); // deleted in the end of main()
	}

	if (house_arr.empty()){ // ALL houses are invalid
		cout << "All house files in target folder '" << path << "' cannot be opened or are invalid :" << endl;
		print_errors(error_list);
		return empty;
	}

	return house_arr;
}

//function for completing miising cells in house matrix by ' '.
//In addition, surranding the matrix with wall if needed. Also,
//returns how many docking stations in the house, AFTER fixing
static int fix_house_matrix(string *matrix, int rows, int cols){
	int count_docking = 0;
	string docking_str("D");
	string wall_str("W");
	string wall_line(cols, 'W');
	string empty_str(cols, ' ');
	//first - check if all rows and columns are complete
	//if not- complete them with ' '
	for (int i = 0; i < rows; i++)
	{
		if (matrix[i].empty()){
			matrix[i] = empty_str;
		}
		else if (matrix[i].size() < (unsigned)cols){
			matrix[i].append((cols - matrix[i].size()), ' ');
		}
	}
	//second - check bounderis to be 'W', if 'D' has overwrite, change found_docking
	for (int j = 0; j < rows; j++)
	{
		if (j == 0 || (j == rows - 1)){
			matrix[j] = wall_line;
		}
		else{
			if (matrix[j].at(0) != 'W'){
				matrix[j].replace(0, 1, wall_str);
			}
			if (matrix[j].at(cols - 1) != 'W'){
				matrix[j].replace((cols - 1), 1, wall_str);
			}
			count_docking += count(matrix[j].begin(), matrix[j].end(), 'D');
		}
	}
	return count_docking;
}


// load .so files that represent algorithms
// creates vector of algorithms (one of each type), and vector of sensors (one for every algorithm)
tuple<vector<AbstractAlgorithm*>, vector<Sensor*>> get_algorithms_and_sensors(string path){
	srand(time(NULL));

	// creating sensor array (one for each algorithm)
//	Sensor** sensor_arr = new Sensor*[num_of_algorithms];
//	for (int i = 0; i < num_of_algorithms; i++){
//		sensor_arr[i] = new Sensor();
//	}

	// building algorithms
	//AbstractAlgorithm** algorithm_arr = new AbstractAlgorithm*[4];
	//algorithm_arr[0] = new OurAlgorithm(*sensor_arr[0], config);
	//algorithm_arr[1] = new EastPrefAlgorithm(*sensor_arr[1], config);
	//algorithm_arr[2] = new WestPrefAlgorithm(*sensor_arr[2], config);
	//algorithm_arr[3] = new SouthPrefAlgorithm(*sensor_arr[3], config);
}

// parse the command line arguments
// returns a tuple <config_path, house_path, algorithm_path>
tuple<string, string, string> Main::command_line_arguments(int argc, char* argv[]){
	// number of arguments has to be odd
	if (argc % 2 == 0){
		PRINT_USAGE;
		return;
	}
	int config_index = -1, house_index = -1, algorithm_index = -1;
	string config_path, house_path, algorithm_path;
	int i = 1;	// starting from i=1 (excluding the name of the program)
	while (i < argc){
		if (argv[i] == "-config" && config_index == -1){ // if at the current there is '-config' and its the first time
			config_index = i;
			if (i + 1 < argc && argv[i + 1] != "-config" && argv[i + 1] != "-house_path" && argv[i + 1] != "-algorithm_path"){ // make sure that there is a path
				config_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return;
			}
			i += 2;
			continue;
		}
		if (argv[i] == "-house_path" && house_index == -1){ // if at the current there is '-house_path' and its the first time
			house_index = i;
			if (i + 1 < argc && argv[i + 1] != "-config" && argv[i + 1] != "-house_path" && argv[i + 1] != "-algorithm_path"){ // make sure that there is a path
				house_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return;
			}
			i += 2;
			continue;
		}
		if (argv[i] == "-algorithm_path" && algorithm_index == -1){ // if at the current there is '-algorithm_path' and its the first time
			algorithm_index = i;
			if (i + 1 < argc && argv[i + 1] != "-config" && argv[i + 1] != "-house_path" && argv[i + 1] != "-algorithm_path"){ // make sure that there is a path
				algorithm_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return;
			}
			i += 2;
			continue;
		}
		else{
			PRINT_USAGE;
			return;
		}
	}


	// getting configurations
	if (config_index != -1){
		if (config_path[config_path.length() - 1] != '/'){
			config_path += "/";
		}
	}
	else {
		config_path = "./";
	}

	// getting houses
	if (house_index != -1){
		if (house_path[house_path.length() - 1] != '/'){
			house_path += "/";
		}
	}
	else {
		house_path = "./";
	}

	// getting algorithms
	if (algorithm_index != -1){
		if (algorithm_path[algorithm_path.length() - 1] != '/'){
			algorithm_path += "/";
		}
	}
	else {
		algorithm_path = "./";
	}

	return make_tuple(config_path, house_path, algorithm_path);
}

// simulate the simulator
void Main::simulate(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	bool winner = false, about_to_finish = false;
	int num_steps_after_winning = 0;
	int rank_in_competition = 1;
	
	// compute the maximum of all the houses 'max_step' fields
	vector<int> max_steps_vec; 
	for (int i = 0; i < num_of_houses; i++){
		max_steps_vec.push_back(sim.get_robots_matrix()[0][i]->get_house()->get_max_steps());
	}
	int max_steps = *max_element(max_steps_vec.begin(), max_steps_vec.end());

	// the simulation
	for (; sim.get_steps() < max_steps && num_steps_after_winning <= config["MaxStepsAfterWinner"] && sim.get_not_active() < num_of_algorithms * num_of_houses;)
	{
		if (rank_in_competition > 1){ // if there is a winner, start counting
			num_steps_after_winning++;
		}
		if (!winner && rank_in_competition > 1 && sim.get_steps() <= max_steps - config["MaxStepsAfterWinner"]) { // the first robot has finished
			winner = true;
			about_to_finish = true;
		}
		if (rank_in_competition == 1 && sim.get_steps() == max_steps - config["MaxStepsAfterWinner"]){ // if steps == MaxSteps - MaxStepsAfterWinner
			about_to_finish = true;
		}
		rank_in_competition = sim.simulate_step(rank_in_competition, about_to_finish); // do a simulation step
		about_to_finish = false;
	}
	// finish the simulation
	sim.finish_simulation();
}

// calculates the score matrix and prints it
void Main::score_simulation(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	// creating score matrix
	int** score_matrix = new int*[num_of_algorithms]; // deleted in the end of main()
	for (int i = 0; i < num_of_algorithms; i++)
	{
		score_matrix[i] = new int[num_of_houses]; // deleted in the end of main()
	}

	if (sim.get_winner_num_steps() == -1){ // if there is no winner, set the winner_num_steps to be simulation_steps
		sim.set_winner_num_steps(sim.get_steps());
	}

	// calculate the score matrix
	Robot * cur_robot;
	for (int i = 0; i < num_of_algorithms; i++)
	{
		for (int j = 0; j < num_of_houses; j++)
		{
			cur_robot = sim.get_robots_matrix()[i][j];
			if (!cur_robot->is_valid()){
				score_matrix[i][j] = 0;
				continue;
			}
			auto loc = cur_robot->get_curr_location();
			score_matrix[i][j] = sim.calculate_score(cur_robot->get_position_in_competition(), sim.get_winner_num_steps(),
				cur_robot->get_num_of_steps(), cur_robot->get_dirt_collected(), cur_robot->get_house()->get_sum_dirt_in_house(),
				cur_robot->get_house()->get_house_matrix()[loc.first][loc.second] == 'D');
		}
	}

	print_score_and_errors(sim, score_matrix);

	// delete score_matrix
	for (int i = 0; i < num_of_algorithms; ++i){
		delete[] score_matrix[i];
	}
	delete[] score_matrix;
}

// trim title in the score matrix to be up to 9 chars and aligned to left
string Main::trim_title(string& title){
	string temp(title, 0, 9);
	return temp + string(10 - temp.length(), ' ');
}

// calculate average score (on all houses) of an algorithm with index 'index'
double Main::avg(int** score_matrix, int index, int num_of_houses){
	int sum = 0;
	for (int i = 0; i < num_of_houses; i++){
		sum += score_matrix[index][i];
	}
	return (double)sum / num_of_houses;
}

// prints the score matrix according to given format.
// prints errors after that, if exist.
// ASSUMPTION: the algorithm_arr and house_arr are sorted, so robot_matrix as well
void Main::print_score_and_errors(Simulator& sim, int** score_matrix){

	const int num_of_chars_cols = 14 + 11 * (sim.get_num_of_houses() + 1) + 1;
	const int num_of_chars_rows = 2 * (sim.get_num_of_algorithms() + 1) + 1;
	string dashes_line(num_of_chars_cols, '-');
	for (int i = 0; i < num_of_chars_rows; i++)
	{
		if (i % 2 == 0){
			cout << dashes_line << endl;
			continue;
		}
		if (i == 1){
			string tmp("|             ");
			for (int j = 0; j < sim.get_num_of_houses(); j++){
				tmp += "|" + trim_title(house_names[j]);
			}
			tmp += "|AVG       |";
			cout << tmp << endl;
			continue;
		}

		int algo_index = ((i - 1) / 2) - 1;
		string tmp = "|" + algorithm_names[algo_index] + " ";
		for (int j = 0; j < sim.get_num_of_houses(); j++)
		{
			string score = to_string(score_matrix[algo_index][j]);
			tmp += "|" + string(10 - score.length(), ' ') + score;
		}
		double d = avg(score_matrix, algo_index, sim.get_num_of_houses());
		stringstream stream;
		stream << fixed << setprecision(2) << d;
		string s = stream.str();
		tmp += "|" + string(10 - s.length(), ' ') + s + "|";
		cout << tmp << endl;
	}

	if (!error_list.empty()){ // if there are errors, print them
		cout << endl;
		cout << "Errors:" << endl;
		print_errors(error_list);
	}

}

// freeing all the memory left to free in the program
void Main::deleting_memory(vector<House*>& house_arr, vector<AbstractAlgorithm*>& algorithm_arr, vector<Sensor*>& sensor_arr,
	int num_of_houses, int num_of_algorithms){
	// delete houses array
	for (int i = 0; i < num_of_houses; ++i){
		delete house_arr[i];
	}

	// delete algorithms array
	for (int i = 0; i < num_of_algorithms; ++i){
		delete algorithm_arr[i];
	}

	// delete sensor array pointer only - all sensor are being deleted in algorithm destructor
	//for (int i = 0; i < num_of_algorithms; i++)
	//{
	//	delete sensor_arr[i];
	//}

}


//main function, initialize config arguments, and given houses descriptions,
//and run the simulator for each house and for each algorithm. Returns 0 on success, 
//1 on failure.
int main(int argc, char* argv[])
{
	Main main;
	auto tup = main.command_line_arguments(argc, argv);
	string config_path = get<0>(tup);
	string house_path = get<1>(tup);
	string algorithm_path = get<2>(tup);

	map<string, int> config = main.get_configurations(config_path); // getting configurations map
	if (config.empty()){
		return 1;
	}
	vector<House*> house_arr = main.get_houses(house_path); // getting houses arr 
	if (house_arr.empty()){
		return 1;
	}

	auto algorithms_and_sensors = get_algorithms_and_sensors(algorithm_path);
	vector<AbstractAlgorithm*> algorithm_arr = get<0>(algorithms_and_sensors); // getting algorithms arr
	vector<Sensor*> sensor_arr = get<1>(algorithms_and_sensors);
	if (algorithm_arr.empty() || sensor_arr.empty()){
		return 1;
	}
	
	const int num_of_houses = house_arr.size();
	const int num_of_algorithms = algorithm_arr.size();

	// creating simulator
	Simulator sim(config, algorithm_arr, sensor_arr, house_arr);

	// simulate the simulator
	main.simulate(sim, config, num_of_houses, num_of_algorithms);
	
	// calculate the score of each robot
	main.score_simulation(sim, config, num_of_houses, num_of_algorithms);

	// freeing memory
	main.deleting_memory(house_arr, algorithm_arr, sensor_arr, num_of_houses, num_of_algorithms);
	
	return 0;
}


// implenentionn if the three algorithms for ex2 - temporary located here
//enum class Direction { East, West, South, North, Stay

// step is called by the simulation for each time unit
Direction _316602689_A::step() {
	SensorInformation s_i = m_sensor->sense();
	if (s_i.dirtLevel > 0){// current position still dirty
		return Direction::Stay;
	}
	else{// current position is clean
		// go to the following direction, in case there isn't a wall there by this order of prefernce: East, West, South, North
		for (int i = 0; i < 4; i++)
		{
			if (!s_i.isWall[i]) return (Direction)i;
		}		
		return Direction::Stay;//never should get here
	}
}

// step is called by the simulation for each time unit
Direction _316602689_B::step() {
	SensorInformation s_i = m_sensor->sense();
	if (s_i.dirtLevel > 0){// current position still dirty
		return Direction::Stay;
	}
	else{// current position is clean
		// go to the following direction, in case there isn't a wall there by this order of prefernce: West, South, North, East
		for (int i = 1; i < 5; i++)
		{
			if (!s_i.isWall[i%4]) return (Direction)i;
		}
		return Direction::Stay;//never should get here
	}
}

// step is called by the simulation for each time unit
Direction _316602689_C::step() {
	SensorInformation s_i = m_sensor->sense();
	if (s_i.dirtLevel > 0){// current position still dirty
		return Direction::Stay;
	}
	else{// current position is clean
		// go to the following direction, in case there isn't a wall there by this order of prefernce: South, North, East, West
		for (int i = 2; i < 6; i++)
		{
			if (!s_i.isWall[i % 4]) return (Direction)i;
		}
		return Direction::Stay;//never should get here
	}
}