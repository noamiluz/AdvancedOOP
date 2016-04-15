/* nadavkareen 316602689; noamiluz 201307436 */

#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

#include <iomanip>
#include <limits.h>

#include <dlfcn.h> 
#include <dirent.h>
#include "simulator.h"

/**
* Function that returns the sensor's information of a
* position in the current house.
* This function assumes that m_matrix[x][y] != 'W'(i.e - current location != 'W')
**/
SensorInformation Sensor::my_sense(const pair<int, int>& position) const {
	SensorInformation result;
	int x = position.first;
	int y = position.second;

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

/**
* Function that calls to my_sense() function, in order
* to get information of a position in the current house.
**/
SensorInformation Sensor::sense() const{
	return my_sense(m_curr_location);
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

/**
* Function returns the amonut of dust currently in the house.
**/
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
		delete m_robot_arr[i];
	}
}

/**
* Function inits the robots array -  for each
* house there is a robots array(each robot for an algorithm).
**/
void Simulator::init_robot_arr(House* house) { 

	for (int i = 0; i < m_num_of_algorithms; i++) {
		m_robot_arr.push_back(new Robot(m_config["BatteryCapacity"], house)); // deleted in Simulator's destructor.
	}
}

/**
* Function that simulate simlation step. Iterate over
* the number of algorithms and activate each robot in the current robots array 
* according to it's corresponding algorithm.
* Returns cuurent rank in competition achieved.
**/
int Simulator::simulate_step(int& rank_in_competition, bool about_to_finish){
	m_steps++; // increment the number of steps
	Robot * cur_robot;
	bool is_someone_finished = false;
	int num_of_winners_this_step = 0;
	for (int i = 0; i < m_num_of_algorithms; i++)
	{
		cur_robot = m_robot_arr[i];
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
				if (m_winner_num_steps == m_max_steps) { // the first robot to finish has to update the winner_num_steps field
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
			continue;

		}

	}

	if (is_someone_finished){ // if there were robots who finished in this step increment the rank
		int ret = rank_in_competition;
		rank_in_competition += num_of_winners_this_step;
		return min(4, ret);
	}
	return rank_in_competition;
}

/**
* Function that updates all the robots that didn't
* finish cleaning the house.
**/
void Simulator::finish_simulation(){
	Robot * cur_robot;
	for (int i = 0; i < m_num_of_algorithms; i++)
	{
		cur_robot = m_robot_arr[i];
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

/**
* Function that updates the files list vector,
* with the full paths of the files.
**/
void FilesLister::refresh() {
	DIR *dir;
	struct dirent *ent;
	this->filesList_.clear();
	if ((dir = opendir(this->basePath_.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			this->filesList_.push_back(concatenateAbsolutePath(this->basePath_, ent->d_name));
		}
		closedir(dir);
	}
	else {
		return;
	}
	sort(this->filesList_.begin(), this->filesList_.end());
}

/**
* Function that saves the files name who
* end with the given suffix 'suffix'.
**/
void FilesListerWithSuffix::filterFiles() {
	vector<string> temp = this->filesList_;
	this->filesList_.clear();
	for (vector<string>::iterator itr = temp.begin(); itr != temp.end(); ++itr){
		if (endsWith(*itr, this->suffix_))
		{
			this->filesList_.push_back(*itr);
		}
	}
}

/**
* Function returns vector of the full paths to all the files in the 
* 'path_of_directory' directory which their names contains the suffix 'suffix'. 
* path_of_directory could be relative or absolute (and with or without '/').
**/
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

/**
* Function reciving  a full path to a file and returns its base name.
**/
string FileParser::get_file_name(const string& full_path){
	return string(find_if(full_path.rbegin(), full_path.rend(),
		[](const char c){return c == '/'; }).base(), full_path.end());
}

/**
* Function that splits a string according to a delimiter. (from recitation)
**/
vector<string> FileParser::split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
* Function that cleans a string from unwanted whitespaces. (from recitation)
**/
string FileParser::trim(string& str) {
	str.erase(0, str.find_first_not_of(' '));// prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);// suffixing spaces
	return str;
}

/**
* Function that given a line read from the configuration file, update the configuration map. (from recitation)
**/
void FileParser::processLine(const string& line, map<string, int> &config)
{
	vector<string> tokens = split(line, '=');
	if (tokens.size() != 2)
	{
		return;
	}
	config[trim(tokens[0])] = stoi(trim(tokens[1]));
}


/**
* Function for printing the collected errors list.
**/
void Main::print_errors(vector<string>& error_list){
	const int length = error_list.size();
	for (int i = 0; i < length; i++)
	{
		cout << error_list[i] << endl;
	}
}

/**
* Function that checks if the content of configuration
* file is valid. If not, print message and return false.
**/
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

/**
* Function that reads from configuration file and returns the configurations map.
**/
map<string, int> Main::get_configurations(const string path){
	FileParser fp;
	ConfigLister cl(path);
	map<string, int> config;
	map<string, int> empty_config;
	string full_path;
	// getting the full path
	vector<string> result = cl.getFilesList();
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
	// 'confing.ini' was oppend successfuly
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

/**
* Function that  reads from '*.house' files and parse them into an house array.
**/
vector<House*> Main::get_houses(string path) {
	FileParser fp;
	HousesLister hl(path);
	vector<House*> house_arr; // deleted in the end of main()
	vector<House*> empty;

	// getting the full path
	vector<string> result = hl.getFilesList();
	if (result.empty()){
		PRINT_USAGE;
		return empty;
	}

	// sort result vector according to file name
	sort(result.begin(), result.end(), [&fp](const string s1, const string s2){ return fp.get_file_name(s1).compare(fp.get_file_name(s2)) < 0; });

	int num_of_houses = result.size();

	string name, line;
	int r, c, max_steps;
	string* matrix;
	pair<int, int> docking;
	string::size_type index;
	for (int i = 0; i < num_of_houses; i++) // iterate on the houses in sorted order
	{
		ifstream fin;
		fin.open(result[i]);
		if (!fin.is_open()){
			error_list.push_back(fp.get_file_name(result[i]) + ": cannot open file");
			continue;
		}
		// '*.house' file was oppend seccessfuly
		getline(fin, name); // getting name
		getline(fin, line); 
		max_steps = atoi(line.c_str()); // getting max_steps
		if (max_steps < 0){
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 2 in house file shall be a positive number, found: " + line);
			continue;
		}
		getline(fin, line);
		r = atoi(line.c_str()); // getting number of rows
		if (r < 0){
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 3 in house file shall be a positive number, found: " + line);
			continue;
		}
		getline(fin, line);
		c = atoi(line.c_str()); // getting number of columns
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
		cout << "All house files in target folder '" << path << "' cannot be opened or are invalid:" << endl;
		print_errors(error_list);
		return empty;
	}

	return house_arr;
}

/**
* Function for completing miising cells in house matrix by ' '.
* In addition, surranding the matrix with wall if needed. Returns how 
* many docking stations in the house, AFTER fixing.
**/
int Main::fix_house_matrix(string *matrix, int rows, int cols){
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
	//second - check bounderis to be 'W', if 'D' has overwrite, change count_docking
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


/**
* Function that loads '*.so' files that represent algorithms,
* creates vector of algorithms (one of each type), and vector 
* of sensors (one for each algorithm).
**/
tuple<vector<AbstractAlgorithm*>, vector<Sensor*>> Main::get_algorithms_and_sensors(string path, map<string, int>& config){
	
	FileParser fp;
	AlgorithmsLister al(path);

	vector<AbstractAlgorithm*> algorithm_arr; // deleted in the end of main()
	vector<AbstractAlgorithm*> algorithm_empty;
	vector<Sensor*> sensor_arr; // deleted in the end of main()
	vector<Sensor*> sensor_empty;

	// getting the full path
	vector<string> result = al.getFilesList();
	if (result.empty()){
		PRINT_USAGE;
		return make_tuple(algorithm_empty, sensor_empty);
	}

	// sort result vector according to file name
	sort(result.begin(), result.end(), [&fp](const string s1, const string s2){ return fp.get_file_name(s1).compare(fp.get_file_name(s2)) < 0; });

	int num_of_algorithms = result.size();

	for (int i = 0; i < num_of_algorithms; i++)
	{
		string tmp(fp.get_file_name(result[i]));
		stringstream ss;
		ss << "./" << tmp;
		string name(ss.str());
		void* dlib = dlopen(name.c_str(), RTLD_NOW);
		if (dlib == NULL){
			error_list.push_back(tmp + ": file cannot be loaded or is not a valid .so");
			continue;
		}
		string algo_name(tmp, 0, tmp.length() - 3);

		auto itr = factory.find(algo_name);
		if (itr == factory.end()){
			error_list.push_back(tmp + ": valid .so but no algorithm was registered after loading it");
			dlclose(dlib);
			continue;
		}
		dl_arr.push_back(dlib);
		Sensor* sensor = new Sensor(); // will be deleted in the end of main
		algorithm_names.push_back(algo_name);
		AbstractAlgorithm* algo = factory[algo_name]();// will be deleted in the end of main
		algo->setSensor(*sensor);
		algo->setConfiguration(config);
		algorithm_arr.push_back(algo); 

		sensor_arr.push_back(sensor);
	}

	if (algorithm_arr.empty()){
		cout << "All algorithm files in target folder '" << path << "' cannot be opened or are invalid:" << endl;
		print_errors(error_list);
		return make_tuple(algorithm_empty, sensor_empty);
	}

	return make_tuple(algorithm_arr, sensor_arr);
}

/**
* Function that parse the command line arguments,
* returns a tuple <config_path, house_path, algorithm_path>.
**/
tuple<string, string, string> Main::command_line_arguments(int argc, char* argv[]){
	
	int config_index = -1, house_index = -1, algorithm_index = -1;
	string config_path, house_path, algorithm_path;
	int i = 1;	// starting from i=1 (excluding the name of the program)
	while (i < argc){
		if (!strcmp(argv[i], "-config") && config_index == -1){ // if at the current there is '-config' and its the first time
			config_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path")){ // make sure that there is a path
				config_path = argv[i + 1];
				i += 2;
			}
			else {
				if (i + 1 >= argc || (i + 1 < argc && (!strcmp(argv[i + 1], "-house_path") || !strcmp(argv[i + 1], "-algorithm_path")))){
					config_path = "./";
					i++;
				}
				else {
					PRINT_USAGE;
					return make_tuple("","","");
				}
			}
			continue;
		}
		if (!strcmp(argv[i], "-house_path") && house_index == -1){ // if at the current there is '-house_path' and its the first time
			house_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path")){ // make sure that there is a path
				house_path = argv[i + 1];
				i += 2;
			}
			else {
				if (i + 1 >= argc || (i + 1 < argc && (!strcmp(argv[i + 1], "-config") || !strcmp(argv[i + 1], "-algorithm_path")))){
					house_path = "./";
					i++;
				}
				else {
					PRINT_USAGE;
					return make_tuple("", "", "");
				}
			}
			continue;
		}
		if (!strcmp(argv[i], "-algorithm_path") && algorithm_index == -1){ // if at the current there is '-algorithm_path' and its the first time
			algorithm_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path")){ // make sure that there is a path
				algorithm_path = argv[i + 1];
				i += 2;
			}
			else {
				if (i + 1 >= argc || (i + 1 < argc && (!strcmp(argv[i + 1], "-house_path") || !strcmp(argv[i + 1], "-config")))){
					algorithm_path = "./";
					i++;
				}
				else {
					PRINT_USAGE;
					return make_tuple("", "", "");
				}
			}
			continue;
		}
		else{
			PRINT_USAGE;
			return make_tuple("", "", "");
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

/**
* Function that simulate the simulator (called once for each house).
**/
void Main::simulate(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	bool winner = false, about_to_finish = false;
	int num_steps_after_winning = 0;
	int rank_in_competition = 1;
	
	// setSenor for all algorithms, in each new simulation (i.e - for each house)
	for (int j = 0; j < num_of_algorithms; j++)
	{
		(sim.get_algorithm_arr())[j]->setSensor(*(sim.get_sensor_arr())[j]);
	}

	// the simulation
	for (; sim.get_steps() < sim.get_max_steps() && num_steps_after_winning <= config["MaxStepsAfterWinner"] && sim.get_not_active() < num_of_algorithms;)
	{
		if (rank_in_competition > 1){ // if there is a winner, start counting
			num_steps_after_winning++;
		}
		if (!winner && rank_in_competition > 1 && sim.get_steps() <= sim.get_max_steps() - config["MaxStepsAfterWinner"]) { // the first robot has finished
			winner = true;
			about_to_finish = true;
		}
		if (rank_in_competition == 1 && sim.get_steps() == sim.get_max_steps() - config["MaxStepsAfterWinner"]){ // if steps == MaxSteps - MaxStepsAfterWinner
			about_to_finish = true;
		}
		rank_in_competition = sim.simulate_step(rank_in_competition, about_to_finish); // do a simulation step
		about_to_finish = false;
	}

	// finish the simulation
	sim.finish_simulation();
}

/**
* Function that calculates the score matrix and prints it.
**/
void Main::score_simulation(vector<Simulator*>& sim_arr, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	// creating score matrix
	int** score_matrix = new int*[num_of_algorithms]; // deleted in the end of main()
	for (int i = 0; i < num_of_algorithms; i++)
	{
		score_matrix[i] = new int[num_of_houses]; // deleted in the end of main()
	}

	for (int i = 0; i < num_of_houses; i++)
	{
		if ((*sim_arr[i]).get_winner_num_steps() == (*sim_arr[i]).get_max_steps()){ // if there is no winner, set the winner_num_steps to be simulation_steps
			(*sim_arr[i]).set_winner_num_steps((*sim_arr[i]).get_steps());
		}
	}
	

	// calculate the score matrix
	Robot * cur_robot;
	for (int i = 0; i < num_of_houses; i++)
	{
		for (int j = 0; j < num_of_algorithms; j++)
		{
			cur_robot = (*sim_arr[i]).get_robot_arr()[j];
			if (!cur_robot->is_valid()){
				score_matrix[j][i] = 0;
				continue;
			}
			auto loc = cur_robot->get_curr_location();
			score_matrix[j][i] = (*sim_arr[i]).calculate_score(cur_robot->get_position_in_competition(), (*sim_arr[i]).get_winner_num_steps(),
				cur_robot->get_num_of_steps(), cur_robot->get_dirt_collected(), cur_robot->get_house()->get_sum_dirt_in_house(),
				cur_robot->get_house()->get_house_matrix()[loc.first][loc.second] == 'D');
		}
	}

	print_score_and_errors(sim_arr, score_matrix);

	// delete score_matrix
	for (int i = 0; i < num_of_algorithms; ++i){
		delete[] score_matrix[i];
	}
	delete[] score_matrix;
}

/**
* Function that trims title in the score matrix to be
* up to 9 chars and aligned to left.
**/
string Main::trim_title(string& title){
	string temp(title, 0, 9);
	return temp + string(10 - temp.length(), ' ');
}

/**
* Function that calculates average score
* (on all houses) of an algorithm with index 'index'.
**/
double Main::calc_avg(int** score_matrix, int index, int num_of_houses){
	int sum = 0;
	for (int i = 0; i < num_of_houses; i++){
		sum += score_matrix[index][i];
	}
	return (double)sum / num_of_houses;
}

/**
* Function that prints the score matrix according to given format,
* prints errors after that, if exist. This function assumes
* that the algorithm_arr, house_arr and robot_matrix are sorted.
**/
void Main::print_score_and_errors(vector<Simulator*>& sim_arr, int** score_matrix){
	
	const int num_of_chars_cols = 14 + 11 * (sim_arr.size() + 1) + 1;
	const int num_of_chars_rows = 2 * ((*sim_arr[0]).get_num_of_algorithms() + 1) + 1;
	const int length = sim_arr.size();
	string dashes_line(num_of_chars_cols, '-');
	for (int i = 0; i < num_of_chars_rows; i++)
	{
		if (i % 2 == 0){
			cout << dashes_line << endl;
			continue;
		}
		if (i == 1){
			stringstream tmp1;
			tmp1 << "|             ";
			for (int j = 0; j < length; j++){
				tmp1 << "|" << trim_title(house_names[j]);
			}
			tmp1 << "|AVG       |";
			cout << tmp1.str() << endl;
			continue;
		}

		int algo_index = ((i - 1) / 2) - 1;
		stringstream tmp;
		tmp << "|";
		string name(algorithm_names[algo_index]);
		tmp << name << string(13 - name.length(), ' ');
		for (int j = 0; j < length; j++)
		{
			string score = to_string(score_matrix[algo_index][j]);
			tmp << "|" << string(10 - score.length(), ' ') << score;
		}
		double d = calc_avg(score_matrix, algo_index, sim_arr.size());
		stringstream stream;
		stream << fixed << setprecision(2) << d;
		string s(stream.str());
		tmp << "|" << string(10 - s.length(), ' ') << s << "|";
		cout << tmp.str() << endl;
	}

	if (!error_list.empty()){ // if there are errors, print them
		cout << endl;
		cout << "Errors:" << endl;
		print_errors(error_list);
	}

}

/**
* Function that frees all the memory left to free in the program.
**/
void Main::deleting_memory(vector<Simulator*> sim_arr, vector<House*>& house_arr, vector<AbstractAlgorithm*>& algorithm_arr, vector<Sensor*>& sensor_arr,
	int num_of_houses, int num_of_algorithms){
	// delete simulator array
	for (int i = 0; i < num_of_houses; ++i){
		delete sim_arr[i];
	}

	// delete houses array
	for (int i = 0; i < num_of_houses; ++i){
		delete house_arr[i];
	}

	// delete algorithms array
	for (int i = 0; i < num_of_algorithms; ++i){
		delete algorithm_arr[i];
	}
	
	// close all dynamic links we opened
	int length = dl_arr.size();
	for (int i = 0; i < length; i++){
		dlclose(dl_arr[i]); 
	}

	// delete sensors array
	for (int i = 0; i < num_of_algorithms; ++i){
		delete sensor_arr[i];
	}
}

/**
* Main function: initialize config arguments, and given houses descriptions,
* and run the simulator for each house and for each algorithm. Returns 0 on success,
* 1 on failure.
**/
int main(int argc, char* argv[])
{
	Main main;
	auto tup = main.command_line_arguments(argc, argv);
	string config_path = get<0>(tup);
	string house_path = get<1>(tup);
	string algorithm_path = get<2>(tup);

	if (!config_path.compare("") && !house_path.compare("") && !algorithm_path.compare("")){ // error
		return 1;
	}
	
	map<string, int> config = main.get_configurations(config_path); // getting configurations map
	if (config.empty()){
		return 1;
	}
	
	vector<House*> house_arr = main.get_houses(house_path); // getting houses arr 
	if (house_arr.empty()){
		return 1;
	}
	
	auto algorithms_and_sensors = main.get_algorithms_and_sensors(algorithm_path, config);
	vector<AbstractAlgorithm*> algorithm_arr = get<0>(algorithms_and_sensors); // getting algorithms arr
	vector<Sensor*> sensor_arr = get<1>(algorithms_and_sensors);
	if (algorithm_arr.empty() || sensor_arr.empty()){
		return 1;
	}
	
	const int num_of_houses = house_arr.size();
	const int num_of_algorithms = algorithm_arr.size();

	// creating vector of simulators, one for each house
	vector<Simulator*> simulator_arr;
	for (int i = 0; i < num_of_houses; i++){
		simulator_arr.push_back(new Simulator(config, algorithm_arr, sensor_arr, house_arr[i]));
	}

	// simulate the simulator for each house
	for (int i = 0; i < num_of_houses; i++){
		main.simulate(*simulator_arr[i], config, num_of_houses, num_of_algorithms);
	}
	
	
	// calculate the score of each robot
	main.score_simulation(simulator_arr, config, num_of_houses, num_of_algorithms);

	// freeing memory
	main.deleting_memory(simulator_arr, house_arr, algorithm_arr, sensor_arr, num_of_houses, num_of_algorithms);
	
	return 0;
}
