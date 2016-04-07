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

#include "simulator.h"

using namespace std;


Sensor::Sensor(string* matrix, const int& rows, const int& cols, const pair<int, int>& init_location) :
m_matrix_rows(rows), m_matrix_cols(cols), m_curr_location(init_location) {
	m_matrix = new string[rows]; // deleted in the corresponding destructor

	for (int i = 0; i < rows; i++) {
		m_matrix[i] = matrix[i];
	}
}

Sensor::~Sensor(){
	delete[] m_matrix;
}


Sensor& Sensor::operator=(const Sensor& other_sensor){
	if (this == &other_sensor) {
		return *this;
	}

	delete[] m_matrix;

	this->m_matrix = new string[other_sensor.m_matrix_rows];

	for (int i = 0; i < other_sensor.m_matrix_rows; ++i){
		this->m_matrix[i] = other_sensor.m_matrix[i];
	}
	this->m_matrix_rows = other_sensor.m_matrix_rows;
	this->m_matrix_cols = other_sensor.m_matrix_cols;

	this->m_curr_location.first = other_sensor.m_curr_location.first;
	this->m_curr_location.second = other_sensor.m_curr_location.second;
	return *this;
}

// returns the sensor's information of the current location of the robot
// Assumption: m_matrix[x][y] != 'W'
SensorInformation Sensor::sense() const {
	SensorInformation result;
	int x = m_curr_location.first;
	int y = m_curr_location.second;

	if (x - 1 >= 0){
		if (m_matrix[x - 1][y] == 'W'){
			result.isWall[(int)Direction::North] = true;
		}
		else {
			result.isWall[(int)Direction::North] = false;
		}
	}
	if (x + 1 < m_matrix_rows){
		if (m_matrix[x + 1][y] == 'W'){
			result.isWall[(int)Direction::South] = true;
		}
		else {
			result.isWall[(int)Direction::South] = false;
		}
	}
	if (y - 1 >= 0){
		if (m_matrix[x][y - 1] == 'W'){
			result.isWall[(int)Direction::West] = true;
		}
		else {
			result.isWall[(int)Direction::West] = false;
		}
	}
	if (y + 1 < m_matrix_cols){
		if (m_matrix[x][y + 1] == 'W'){
			result.isWall[(int)Direction::East] = true;
		}
		else {
			result.isWall[(int)Direction::East] = false;
		}
	}
	result.dirtLevel = m_matrix[x][y] == ' ' || m_matrix[x][y] == 'D' ? 0 : m_matrix[x][y] - '0';
	return result;
}


// step is called by the simulation for each time unit
// Naive step : chooses randomly where to go (from the current valid directions)
Direction OurAlgorithm::step() {
	SensorInformation s_i = m_sensor->sense();

	int count_walls = 0;
	for (int i = 0; i < 4; i++) // count how much walls surrounding the current location
	{
		if (s_i.isWall[i]){
			count_walls++;
		}
	}
	mt19937 rng;
	rng.seed(random_device()());
	uniform_int_distribution<mt19937::result_type> rnd(0, 4 - count_walls); // choose randomly between all valid directions

	int choice = rnd(rng);
	int tmp = 0;
	for (int i = 0; i < 4; i++)
	{
		if (s_i.isWall[i]){
			continue;
		}
		if (tmp == choice){
			Direction d = (Direction)(i);
			cout << i << endl;
			return d;
		}
		tmp++;
	}/*
	 srand(time(NULL));
	 int i = rand() % 5;
	 while (i != (int)Direction::Stay && s_i.isWall[i]){
	 i = rand() % 5;
	 }*/

	return (Direction::Stay);
}

House::~House(){
	delete[] m_house_matrix;
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
void Simulator::init_robots_matrix() {
	m_robots_matrix = new Robot**[m_num_of_algorithms];
	for (int i = 0; i < m_num_of_algorithms; i++) {
		m_robots_matrix[i] = new Robot*[m_num_of_houses];
		for (int j = 0; j < m_num_of_houses; j++) {
			Sensor *sensor = new Sensor(m_house_arr[j]->get_house_matrix(), m_house_arr[j]->get_house_matrix_rows(),
				m_house_arr[j]->get_house_matrix_cols(), m_house_arr[j]->get_house_docking_station()); // deleted in the destructor of OurAlgorithm
			m_robots_matrix[i][j] = new Robot(m_config["BatteryCapacity"], new OurAlgorithm(*sensor, m_config), m_house_arr[j],
				sensor); // new OurAlgorithm is deleted in the destructor of Robot
			// new Robot is deleted in the destructor of Simulator
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
			if (!cur_robot->is_active()){ // if the robot is not active any more, continue
				continue;
			}

			if (about_to_finish){
				cur_robot->get_algorithm()->aboutToFinish(m_config["MaxStepsAfterWinner"]);
			}

			Direction d = cur_robot->get_algorithm()->step(); // ask the algorithm what direction to go
			pair<int, int> next_loc;
			Sensor* cur_sensor = cur_robot->get_sensor(); // get the sensor of the current robot
			const pair<int, int>& cur_loc = cur_sensor->get_curr_location(); // get the current location of the robot

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
			char ch = cur_sensor->get_matrix()[next_loc.first][next_loc.second]; // get the matrix item in the next location
			cur_sensor->set_curr_location(next_loc); // set the next location as the current one
			cur_robot->increment_num_of_steps(); // increment the number of steps the robot has done

			if (ch == 'W'){ // if there is a wall in the next location
				cur_robot->set_active(false);
				m_not_active++;
				cur_robot->set_valid(false);
				cout << "Algorithm [" << i << "] on House [" << m_house_arr[j]->get_house_short_name() <<
					"] has made an invalid step: Moved to a wall in the position (" << next_loc.first <<
					"," << next_loc.second << ")." << endl << "Simulation stopped for this robot." << endl;
				continue;
			}

			// if the current location is a docking station, increment the curr_battery_level by RechargeRate.
			// if the battery is full do not increment.
			if (cur_sensor->get_matrix()[next_loc.first][next_loc.second] == 'D'){
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
					if (m_winner_num_steps == m_config["MaxSteps"]) { // the first robot to finish has to update the winner_num_steps field
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
			else if (cur_sensor->get_matrix()[next_loc.first][next_loc.second] != 'D' && cur_sensor->get_matrix()[cur_loc.first][cur_loc.second] == 'D'){
				cur_robot->set_curr_battery_level(min(m_config["BatteryCapacity"], cur_robot->get_curr_battary_level() + m_config["BatteryRechargeRate"]));
			}
			else { // if the current location is not on a docking station, decrement the curr_battery_level by ConsumptionRate.
				// if the battery level is 0 do not decrement.
				cur_robot->set_curr_battery_level(max(0, cur_robot->get_curr_battary_level() - m_config["BatteryConsumptionRate"]));
			}

			// clean one dust level in the next spot and update the matrix. If there is no dust, do nothing.
			cur_sensor->get_matrix()[next_loc.first][next_loc.second] = ch < 49 || ch > 57 ? ' ' : (char)((cur_sensor->get_matrix()[next_loc.first][next_loc.second] - 1));
			if (ch > 48 && ch < 58){ // ch = 1,...,9
				cur_robot->increment_dirt_collected(); // increment the dirt collected by this robot
			}

			if (cur_robot->get_curr_battary_level() == 0){ // the robot got stuck with battery 0
				cur_robot->set_active(false);
				m_not_active++;
				cur_robot->set_position_in_competition(10);
				cout << "Algorithm [" << i << "] on House [" << m_house_arr[j]->get_house_short_name() <<
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
vector<string> get_file_paths(string path_of_directory, string suffix){
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

// splits a string according to a delimiter. (from recitation)
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

// cleans a string from unwanted whitespaces. (from recitation)
string trim(string& str) {
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);         //suffixing spaces
	return str;
}

// given a line read from the configuration file, update the configuration map. (from recitation)
void processLine(const string& line, map<string, int> &config)
{
	vector<string> tokens = split(line, '=');
	if (tokens.size() != 2)
	{
		return;
	}
	config[trim(tokens[0])] = stoi(trim(tokens[1]));
}

// read from configuration file and return the configurations map
map<string, int> get_configurations(string path){
	map<string, int> config;
	string full_path;
	vector<string> result = get_file_paths(path, "config.ini");
	if (result.empty()){
		if ((result = get_file_paths(".", "config.ini")).empty()){
			cout << "ERROR: config.ini does not exist" << endl;
			return config;
		}
		else {
			full_path = result[0];
		}
	}
	else {
		full_path = result[0];
	}
	ifstream fin;
	fin.open(full_path);
	if (!fin.is_open()){
		cout << "ERROR: the configuration file path " << full_path << " is incorrect." << endl;
		return config;
	}

	string line;
	while (getline(fin, line)){
		processLine(line, config);
	}
	fin.close();
	return config;
}

// read from .house files and parse them into a house array;
House** get_houses(char* path) {
	string full_path;
	vector<string> result = get_file_paths(path, "*.house");

	if (result.empty()){
		if ((result = get_file_paths(".", "*.house")).empty()){
			cout << "ERROR: .house files do not exist" << endl << "Terminating the program." << endl;
			return NULL;
		}
	}
	int num_of_houses = result.size();
	House ** house_arr = new House*[num_of_houses]; // deleted in the end of main()
	ifstream fin;
	string name, desc, line;
	int r, c;
	string* matrix;
	pair<int, int> docking;
	string::size_type index;
	int found_docking = 0;
	for (int i = 0; i < num_of_houses; i++)
	{
		fin.open(result[i]);
		getline(fin, name); // getting short name
		getline(fin, desc); // getting long description
		getline(fin,line);
		r = atoi(line.c_str()); // getting r
		getline(fin,line);
		c = atoi(line.c_str()); // getting c
		matrix = new string[r]; // deleted in the destructor of House
		for (int j = 0; j < r && getline(fin, line); j++)
		{
			matrix[j] = line;
			cout << line << "reading" << endl;
			index = line.find("D"); // finding whether the docking station is in this line
			if (index != string::npos){
				docking.first = j;
				docking.second = index;
				found_docking++;
			}
			cout << found_docking << "dokkkkkkkk" << endl;
		}
		fix_house_matrix(matrix, r, c, found_docking);
		if ((found_docking == 0) || (found_docking > 1)){
			if (found_docking == 0){
				cout << "ERROR: couldn't find docking station in house [" << name << "]. Terminating simulation!" << endl;
			}
			else if (found_docking > 1){
				cout << "ERROR: entered " << found_docking << " docking stations, in house [" << name << "], expecting 1. Terminating simulation!" << endl;
			}
			delete[] house_arr;
			delete[] matrix;
			return NULL;
		}
		house_arr[i] = new House(name, desc, r, c, docking, matrix); // deleted in the end of main()
	}
	return house_arr;
}

//function for completing miising cells in house matrix by ' '.
//In addition, surranding the matrix with wall if needed. Also,
// update the 'found_docking' varible in a csee that a docking station was overwriten.
void fix_house_matrix(string *matrix, int rows, int cols, int& found_docking){
	string docking_str("D");
	string wall_str("W");
	string wall_line(cols, 'W');
	string empty_str(cols, ' ');
	//first - check if all rows and columns are complete
	//if not- complete them with ' '
	for (int k = 0; k < rows; k++)
	{
		cout << matrix[k].c_str() << endl;
	}

	for (int i = 0; i < rows; i++)
	{
		if (matrix[i].empty()){
			matrix[i] = empty_str;
		}
		else if (matrix[i].size() < (unsigned)cols){
			matrix[i].append((cols - matrix[i].size()), ' ');
		}
	}
	cout << found_docking << "in my func" << endl;
	//second - check bounderis to be 'W', if 'D' has overwrite, change found_docking to br false
	for (int j = 0; j < rows; j++)
	{
		if (j == 0 || (j == rows - 1)){
			if (strstr(matrix[j].c_str(), docking_str.c_str())){
				found_docking--;
				cout << "here we delete " << j << " index, " << matrix[j].c_str() << endl;
				cout << "this is '0' " << matrix[0].c_str() << endl;
				cout << "this is '1' " << matrix[1].c_str() << endl;


			}
			matrix[j] = wall_line;
		}
		else{
			if ((matrix[j].at(0) == 'D') || (matrix[j].at(cols - 1) == 'D')){
				found_docking--;
				cout << "this is '0' " << matrix[j].at(0) << endl;
				cout << "this is '"<<cols-1<<"' " << matrix[j].at(0) << endl;

				cout << "here we delete2 " << j << " index, " << matrix[j].c_str() << endl;

			}
			if (matrix[j].at(0) != 'W'){
				matrix[j].replace(0, 1, wall_str);
			}
			if (matrix[j].at(cols - 1) != 'W'){
				matrix[j].replace((cols - 1), 1, wall_str);
			}
		}
	}
	cout << found_docking << "in my func out" << endl;

}

// create a house hard coded --- for ex1 only
House* create_house_hard_coded() {
	const int rows = 8;
	const int cols = 10;
	int found_docking = 0;
	string* matrix = new string[rows]; // deleted in the destructor of House
	matrix[0] = "WWWWWWWWWW";
	matrix[1] = "W22  DW59W";
	matrix[2] = "W  W 1119W";
	matrix[3] = "W WWW3 WWW";
	matrix[4] = "W6   3W  W";
	matrix[5] = "W78W  W  W";
	matrix[6] = "W99W  W  D";
	matrix[7] = "WWWWWWWWWW";

	for (int i = 0; i < rows; i++){//count docking stations
		found_docking += count(matrix[i].begin(), matrix[i].end(), 'D');
	}
	fix_house_matrix(matrix, rows, cols, found_docking);//complete missing cells with ' ', and surround the house with 'W'

	if ((found_docking == 0) || (found_docking > 1)){//varify that #'D' == 1
		if (found_docking == 0){
			cout << "ERROR: couldn't find docking station in house [simple1]. Terminating simulation!" << endl;
		}
		else if (found_docking > 1){
			cout << "ERROR: entered " << found_docking << " docking stations, in house [simple1], expecting 1. Terminating simulation!" << endl;
		}
		delete[] matrix;
		return NULL;
	}
	return new House("Simple0", "2 Bedrooms + Kitchen Isle", rows, cols, pair<int, int>(1, 5), matrix); // deleted in the end of main()
}

//main function, initialize config arguments, and given houses descriptions,
//and run the simulator for each house and for each algorithm. Returns 0 on success, 
//1 on failure.
int main(int argc, char* argv[])
{
	map<string, int> config;

	if ((argc > 5) || (argc == 5 && ((strcmp(argv[1], "-config") && strcmp(argv[1], "-house_path")) || (strcmp(argv[3], "-config") && strcmp(argv[3], "-house_path")))) ||
		(argc > 1 && (strcmp(argv[1], "-config") && strcmp(argv[1], "-house_path"))) || (argc > 3 && (strcmp(argv[3], "-config") && strcmp(argv[3], "-house_path") &&
		strcmp(argv[2], "-config") && strcmp(argv[2], "-house_path")))){
		cout << "Usage: [-config [<config_file_location>]] [-house_path <houses_path_location>]." << endl;
		return 1;
	}

	// --- const variables just for ex1 !! ---- 
	const int num_of_houses = 1;
	const int num_of_algorithms = 1;
	// ----------------------------------------
	House** house_arr;

	if ((argc > 2 && !strcmp(argv[1], "-config")) || (argc>2 && !strcmp(argv[2], "-config")) || (argc > 4 && !strcmp(argv[3], "-config")) ||
		(argc == 4 && !strcmp(argv[2], "-config")) || (argc == 4 && !strcmp(argv[3], "-config"))){// if config.ini were given to us in the command line argumentss

		int config_path_cmd_index = 0;
		if (argc > 2 && !strcmp(argv[1], "-config") && strcmp(argv[2], "-house_path")) config_path_cmd_index = 2;
		else if (argc > 4 && !strcmp(argv[3], "-config")) config_path_cmd_index = 4;
		else if (argc == 4 && !strcmp(argv[2], "-config") && strcmp(argv[3], "-house_path")) config_path_cmd_index = 3;
		//else if (argc > 2 && !strcmp(argv[2], "-config")) config_path_cmd_index = 0;

		if (config_path_cmd_index != 0){
			if (argv[config_path_cmd_index][strlen(argv[config_path_cmd_index]) - 1] != '/'){//adding the '/' char, if doesnt exists
				string temp_path(argv[config_path_cmd_index]);
				temp_path += "/";
				config = get_configurations(temp_path); // get configurations from the given directory

			}
			else{
				config = get_configurations(argv[config_path_cmd_index]); // get configurations from the given directory
			}

		}
		else{
			config = get_configurations("."); // get configurations from the working directory
		}
		if (config.empty()){
			return 1;
		}
	}


	if ((argc > 2 && !strcmp(argv[1], "-house_path")) || (argc>2 && !strcmp(argv[2], "-house_path")) || (argc > 4 && !strcmp(argv[3], "-house_path"))
		|| (argc == 4 && !strcmp(argv[2], "-house_path")) || (argc == 4 && !strcmp(argv[3], "-house_path"))) // if houses were given to us in the command line argumentss
	{
		int house_path_cmd_index = 0;
		if (argc > 2 && !strcmp(argv[1], "-house_path") && strcmp(argv[2], "-config")) house_path_cmd_index = 2;
		else if (argc > 4 && !strcmp(argv[3], "-house_path")) house_path_cmd_index = 4;
		else if (argc == 4 && !strcmp(argv[2], "-house_path") && strcmp(argv[3], "-config")) house_path_cmd_index = 3;


		if (house_path_cmd_index != 0){
			if (argv[house_path_cmd_index][strlen(argv[house_path_cmd_index]) - 1] != '/'){//adding the '/' char, if doesnt exists
				string temp_path(argv[house_path_cmd_index]);
				temp_path += "/";
				house_arr = get_houses(const_cast<char *>(temp_path.c_str()));

			}
			else{
				house_arr = get_houses(argv[house_path_cmd_index]);
			}

		}
		else{
			house_arr = get_houses((char *)".");
		}

		if (house_arr == NULL){
			return 1;
		}
	}
	else {
		if ((argc == 2) && !strcmp(argv[1], "-house_path")){//user doesnt entered a path 
			house_arr = get_houses((char *)".");
			if (house_arr == NULL){
				return 1;
			}
		}

		else{// create house hard coded
			house_arr = new House*[num_of_houses]; // deleted in the end of main()
			house_arr[0] = create_house_hard_coded();

			if (house_arr[0] == NULL){//in that case, the house matrix is deleted in 'create_house_hard_coded'
				delete[] house_arr;
				return 1;
			}
		}

	}

	if (argc <= 2){
		config = get_configurations("."); // get configurations from the working directory
		if (config.empty()){
			return 1;
		}
	}
//jfjhfj
//	jgvdskjvds
	Simulator sim(config, house_arr, num_of_houses, num_of_algorithms);
	bool winner = false, about_to_finish = false;
	int num_steps_after_winning = 0;
	int rank_in_competition = 1;

	// the simulation
	for (; sim.get_steps() < config["MaxSteps"] && num_steps_after_winning <= config["MaxStepsAfterWinner"] && sim.get_not_active() < num_of_algorithms * num_of_houses;)
	{
		if (rank_in_competition > 1){ // if there is a winner, start counting
			num_steps_after_winning++;
		}
		if (!winner && rank_in_competition > 1 && sim.get_steps() <= config["MaxSteps"] - config["MaxStepsAfterWinner"]) { // the first robot has finished
			winner = true;
			about_to_finish = true;
		}
		if (rank_in_competition == 1 && sim.get_steps() == config["MaxSteps"] - config["MaxStepsAfterWinner"]){ // if steps == MaxSteps - MaxStepsAfterWinner
			about_to_finish = true;
		}
		rank_in_competition = sim.simulate_step(rank_in_competition, about_to_finish); // do a simulation step
		about_to_finish = false;
	}
	// finish the simulation
	sim.finish_simulation();

	// calculate the score of each robot
	int** score_matrix = new int*[num_of_algorithms]; // deleted in the end of main()
	for (int i = 0; i < num_of_algorithms; i++)
	{
		score_matrix[i] = new int[num_of_houses]; // deleted in the end of main()
	}
	if (sim.get_winner_num_steps() == config["MaxSteps"]){ // if there is no winner, set the winner_num_steps to be simulation_steps
		sim.set_winner_num_steps(sim.get_steps());
	}
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
			auto loc = cur_robot->get_sensor()->get_curr_location();
			score_matrix[i][j] = sim.calculate_score(cur_robot->get_position_in_competition(), sim.get_winner_num_steps(),
				cur_robot->get_num_of_steps(), cur_robot->get_dirt_collected(), house_arr[j]->get_sum_dirt_in_house(),
				cur_robot->get_sensor()->get_matrix()[loc.first][loc.second] == 'D');
		}
	}
	// print the scores
	for (int i = 0; i < num_of_algorithms; i++)
	{
		for (int j = 0; j < num_of_houses; j++)
		{
			cout << "[" << house_arr[j]->get_house_short_name() << "]\t" << score_matrix[i][j] << endl;
		}
	}

	// delete houses array

	for (int i = 0; i < num_of_houses; ++i){
		delete house_arr[i];
	}

	delete[] house_arr;

	// delete score_matrix
	for (int i = 0; i < num_of_algorithms; ++i){
		delete[] score_matrix[i];
	}
	delete[] score_matrix;

	return 0;
}


