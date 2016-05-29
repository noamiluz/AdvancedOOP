/* nadavkareen 316602689; noamiluz 201307436 */

#include "Main.h"

#ifdef __gnu_linux__
#include <dlfcn.h> 
#include <dirent.h>
#endif

#ifdef WIN32
House* getHouseHardCoded(){
	Main main;
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
	matrix[6] = "W99W  W  W";
	matrix[7] = "WWWWWWWWWW";
	/*
	matrix[0] = "WWWWWWWWWW";
	matrix[1] = "W55   W99W";
	matrix[2] = "W9 WD4569W";
	matrix[3] = "WWWWWWWWWW";*/



	found_docking = main.fix_house_matrix(matrix, rows, cols);//complete missing cells with ' ', and surround the house with 'W'

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
	return new House("Simple1", 150, rows, cols, pair<int, int>(1, 5), matrix); // deleted in the end of main()

}

#endif

/**
* Function creates a video for every algorithm & house
**/
void Main::encode_images_into_video() const{
	// iterate over all algorithmes & houses, and encode images into video
	for (string curr_algo_name : algorithm_names) {
			for (string curr_house_name : house_names){
			string images_dir = "images/" + curr_algo_name + curr_house_name; // "curr_algo_name" ends with "_"
			string images_expression = images_dir + "/image%06d.jpg";

			Encoder::encode(images_expression, curr_algo_name + curr_house_name + ".mpg"); // "curr_algo_name" ends with "_"
			// delete the images dir
			string cmd = "rm -rf " + images_dir;
			int ret = system(cmd.c_str());
			if (ret == -1)
			{
				//handle error
			}
		}
	}


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
bool Main::check_configurations_validity(const map<string, int>& config, string& temp_missing, string& temp_invalid, int& count_missing, int& count_invalid){

	if (config.find("MaxStepsAfterWinner") == config.end()){
		count_missing++;
		temp_missing += "MaxStepsAfterWinner, ";
	}
	else {
		if (config.at("MaxStepsAfterWinner") < 0){
			count_invalid++;
			temp_invalid += "MaxStepsAfterWinner, ";
		}
	}
	if (config.find("BatteryCapacity") == config.end()){
		count_missing++;
		temp_missing += "BatteryCapacity, ";
	}
	else {
		if (config.at("BatteryCapacity") < 0){
			count_invalid++;
			temp_invalid += "BatteryCapacity, ";
		}
	}
	if (config.find("BatteryConsumptionRate") == config.end()){
		count_missing++;
		temp_missing += "BatteryConsumptionRate, ";
	}
	else {
		if (config.at("BatteryConsumptionRate") < 0){
			count_invalid++;
			temp_invalid += "BatteryConsumptionRate, ";
		}
	}
	if (config.find("BatteryRechargeRate") == config.end()){
		count_missing++;
		temp_missing += "BatteryRechargeRate, ";
	}
	else {
		if (config.at("BatteryRechargeRate") < 0){
			count_invalid++;
			temp_invalid += "BatteryRechargeRate, ";
		}
	}

	if (count_missing == 0 && count_invalid == 0){
		return true;
	}

	return false;
}

/**
* Function that reads from configuration file and returns the configurations map.
**/
map<string, int> Main::get_configurations(const string path){
	FileParser fp;
	map<string, int> config;
#ifdef __gnu_linux__
	ConfigLister cl(path);
	map<string, int> empty_config;
	string full_path;
	// getting the full path
	vector<string> result = cl.getFilesList();
	if (result.empty()){
		PRINT_USAGE;
		cout << "cannot find config.ini file in '" << path << "'" << endl;
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
	string line, temp_missing, temp_invalid;
	int count_missing = 0, count_invalid = 0;
	while (getline(fin, line)){
		fp.processLine(line, config, temp_invalid, count_invalid);
	}
	fin.close();
	if (!check_configurations_validity(config, temp_missing, temp_invalid, count_missing, count_invalid)){
		if (count_missing > 0){
			cout << "config.ini missing " << count_missing << " parameter(s): " << string(temp_missing, 0, temp_missing.length() - 2) << endl;
		}
		if (count_invalid > 0){
			cout << "config.ini having bad values for " << count_invalid << " parameter(s): " << string(temp_invalid, 0, temp_invalid.length() - 2) << endl;
		}
		return empty_config;
	}
#endif
	return config;
}

/**
* Function that  reads from '*.house' files and parse them into an house array.
**/
vector<House*> Main::get_houses(string path) {
	FileParser fp;
	vector<House*> house_arr; // deleted in the end of main()
	vector<House*> empty;

#ifdef __gnu_linux__
	HousesLister hl(path);

	// getting the full path
	vector<string> result = hl.getFilesList();
	if (result.empty()){
		PRINT_USAGE;
		cout << "cannot find house files in '" << path << "'" << endl;
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
		if (!fp.is_positive_num(line)) {// getting max_steps
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 2 in house file shall be a positive number, found: " + line);
			continue;
		}
		else {
			max_steps = atoi(line.c_str());
		}

		
		getline(fin, line);
		if (!fp.is_positive_num(line)) { // getting max_steps
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 3 in house file shall be a positive number, found: " + line);
			continue;
		}
		else {
			r = atoi(line.c_str());
		}


		getline(fin, line);
		if (!fp.is_positive_num(line)) {// getting max_steps
			error_list.push_back(fp.get_file_name(result[i]) + ": line number 4 in house file shall be a positive number, found: " + line);
			continue;
		}
		else {
			c = atoi(line.c_str());
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
#endif
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
			if ((int)matrix[j].length() > 0 && matrix[j].at(0) != 'W'){
				matrix[j].replace(0, 1, wall_str);
			}
			if ((int)matrix[j].length() > cols -1 && matrix[j].at(cols - 1) != 'W'){
				matrix[j].replace((cols - 1), 1, wall_str);
			}
			count_docking += count(matrix[j].begin(), matrix[j].end(), 'D');
		}
	}

	return count_docking;
}


/**
* Function that loads '*.so' files that represent algorithms,
* returns the number of successfully loaded algorithms, otherwise 0.
**/
int Main::load_algorithms(const string& path){

	vector<string> tmp_error_list;
	FileParser fp;
	int count_registered = Registrar::get_instance().get_num_of_registered_algorithms();

#ifdef __gnu_linux__

	AlgorithmsLister al(path);

	// getting the full path
	vector<string> result = al.getFilesList();
	if (result.empty()){
		PRINT_USAGE;
		cout << "cannot find algorithm files in '" << path << "'" << endl;
		return 0;
	}

	// sort result vector according to file name
	sort(result.begin(), result.end(), [&fp](const string s1, const string s2){ return fp.get_file_name(s1).compare(fp.get_file_name(s2)) < 0; });

	int num_of_algorithms = result.size();

	for (int i = 0; i < num_of_algorithms; i++)
	{
		string tmp(fp.get_file_name(result[i]));
		void* dlib = dlopen(result[i].c_str(), RTLD_NOW);
		if (dlib == NULL){
			tmp_error_list.push_back(tmp + ": file cannot be loaded or is not a valid .so");
			continue;
		}
		string algo_name(tmp, 0, tmp.length() - 3);

		if (Registrar::get_instance().get_num_of_registered_algorithms() != count_registered + 1){
			tmp_error_list.push_back(tmp + ": valid .so but no algorithm was registered after loading it");
			dlclose(dlib);
			continue;
		}

		count_registered = Registrar::get_instance().get_num_of_registered_algorithms();

		dl_arr.push_back(dlib);
		algorithm_names.push_back(algo_name);
	}

	if (dl_arr.empty()){
		cout << "All algorithm files in target folder '" << path << "' cannot be opened or are invalid:" << endl;
		print_errors(tmp_error_list);
		return 0;
	}

	int tmp_length = tmp_error_list.size();
	for (int i = 0; i < tmp_length; i++){
		error_list.push_back(tmp_error_list[i]);
	}

#endif

	return count_registered;
}

/**
* the default score formula
*/
int default_score_formula(const map<string, int>& score_params){
	int position_in_competition = 10;
	if (score_params.at("sum_dirt_in_house") == score_params.at("dirt_collected") && score_params.at("is_back_in_docking")){
		position_in_competition = min(4, score_params.at("actual_position_in_competition"));
	}

	return max(0,
		2000
		- (position_in_competition - 1) * 50
		+ (score_params.at("winner_num_steps") - score_params.at("this_num_steps")) * 10
		- (score_params.at("sum_dirt_in_house") - score_params.at("dirt_collected")) * 3
		+ (score_params.at("is_back_in_docking") ? 50 : -200));

}

// returns a pointer to the score formula function.
// load the .so file if necessary. otherwise, uses default score formula.
score_formula Main::get_score_formula(const string& path){

	if (!path.compare("DEFAULT")) { // the default score formula is returned
		return default_score_formula;
	}

	score_formula formula;
#ifdef __gnu_linux__
	ScoreFormulaLister sfl(path);

	vector<string> result = sfl.getFilesList();

	if (result.empty()){
		PRINT_USAGE;
		cout << "cannot find score_formula.so file in '" << path << "'" << endl;
		return NULL;
	}

	void* dlib = dlopen(result[0].c_str(), RTLD_NOW);
	if (dlib == NULL){
		cout << "score_formula.so exists in '" + path + "' but cannot be opened or is not a valid.so" << endl;
		return NULL;
	}

	// assuming the score formula was written under extern "C"
	formula = reinterpret_cast<score_formula>((long)dlsym(dlib, "calc_score"));
	if (formula == NULL){ // dlsym failed
		cout << "score_formula.so is a valid .so but it does not have a valid score formula" << endl;
		dlclose(dlib);
		return NULL;
	}
	dl_arr.push_back(dlib);

#endif
	return formula;
}






/**
* Function that parse the command line arguments,
* returns a tuple <config_path, house_path, algorithm_path>.
**/
tuple<string, string, string, string, int> Main::command_line_arguments(int argc, char* argv[]){

	int config_index = -1, house_index = -1, algorithm_index = -1, score_formula = -1, threads = -1, video = -1;
	string config_path, house_path, algorithm_path, score_path, threads_num;
	int i = 1;	// starting from i=1 (excluding the name of the program)
	while (i < argc){
		if (!strcmp(argv[i], "-config") && config_index == -1){ // if at the current there is '-config' and its the first time
			config_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path") && strcmp(argv[i + 1], "-score_formula") && strcmp(argv[i + 1], "-threads")){ // make sure that there is a path
				config_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return make_tuple("", "", "", "", 1);
			}
			i += 2;
			continue;
		}
		if (!strcmp(argv[i], "-house_path") && house_index == -1){ // if at the current there is '-house_path' and its the first time
			house_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path") && strcmp(argv[i + 1], "-score_formula") && strcmp(argv[i + 1], "-threads")){ // make sure that there is a path
				house_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return make_tuple("", "", "", "", 1);
			}
			i += 2;
			continue;
		}
		if (!strcmp(argv[i], "-algorithm_path") && algorithm_index == -1){ // if at the current there is '-algorithm_path' and its the first time
			algorithm_index = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path") && strcmp(argv[i + 1], "-score_formula") && strcmp(argv[i + 1], "-threads")){ // make sure that there is a path
				algorithm_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return make_tuple("", "", "", "", 1);
			}
			i += 2;
			continue;
		}
		if (!strcmp(argv[i], "-score_formula") && score_formula == -1){ // if at the current there is '-algorithm_path' and its the first time
			score_formula = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path") && strcmp(argv[i + 1], "-score_formula") && strcmp(argv[i + 1], "-threads")){ // make sure that there is a path
				score_path = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return make_tuple("", "", "", "", 1);
			}
			i += 2;
			continue;
		}
		if (!strcmp(argv[i], "-threads") && threads == -1){ // if at the current there is '-algorithm_path' and its the first time
			threads = i;
			if (i + 1 < argc && strcmp(argv[i + 1], "-config") && strcmp(argv[i + 1], "-house_path") && strcmp(argv[i + 1], "-algorithm_path") && strcmp(argv[i + 1], "-score_formula") && strcmp(argv[i + 1], "-threads")){ // make sure that there is a path
				threads_num = argv[i + 1];
			}
			else {
				PRINT_USAGE;
				return make_tuple("", "", "", "", 1);
			}
			i += 2;
			continue;
		}
		if (!strcmp(argv[i], "-video") && video == -1){ // if at the current there is '-video' and its the first time
			threads = i;
			is_video = true; // there is a need just to set this member to true
			i += 1;
			continue;
		}
		else{
			i++;
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

	// getting score formula
	if (score_formula != -1){
		if (score_path[score_path.length() - 1] != '/'){
			score_path += "/";
		}
	}
	else {
		score_path = "DEFAULT";
	}

	// getting threads number
	int num_threads = 1;
	if (threads != -1){
		if (!threads_num.empty() && find_if(threads_num.begin(),
			threads_num.end(), [](char c) { return !isdigit(c); }) == threads_num.end()){ // value is numeric
			int tmp = stoi(threads_num.c_str());
			if (tmp > 0){ // if positive
				num_threads = tmp;
			}
		}
	}

	return make_tuple(config_path, house_path, algorithm_path, score_path, num_threads);
}


void Main::execute_simulation_multi_threaded(vector<Simulator*>& simulator_arr, map<string, int>& config, int num_of_threads, int num_of_houses, int num_of_algorithms){
	// build thread arr for simulating
	const int size = min(num_of_threads, num_of_houses);
	vector<thread> thread_arr;
	for (int i = 0; i < size; i++){
		thread_arr.push_back(thread(&Main::thread_simulation, this, ref(simulator_arr), ref(config), num_of_houses, num_of_algorithms));
	}
	for (int i = 0; i < size; i++){
		thread_arr[i].join();
	}
}


// the function is given the threads for simulation on the houses.
void Main::thread_simulation(vector<Simulator*>& simulator_arr, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	while (num_of_simulated_houses < num_of_houses){
		mutex_lock.lock();
		num_of_simulated_houses++;
		int tmp = num_of_simulated_houses;
		mutex_lock.unlock();
		simulate(*simulator_arr[tmp - 1], config, num_of_algorithms, house_names[tmp - 1]);
	}
}


/**
* Function that simulate the simulator (called once for each house).
**/
void Main::simulate(Simulator& sim, map<string, int>& config, int num_of_algorithms, string& house_name){
	FileParser fp;
	bool winner = false, about_to_finish = false, called_about_to_finish = false;
	int num_steps_after_winning = 0;
	int rank_in_competition = 1;
	string message("");
	/*
	// setSenor for all algorithms, in each new simulation (i.e - for each house)
	for (int j = 0; j < num_of_algorithms; j++)
	{
		(sim.get_algorithm_arr())[j]->setSensor(*(sim.get_sensor_arr())[j]);
	}*/

	// the simulation
	for (; sim.get_steps() < sim.get_max_steps() && num_steps_after_winning <= config["MaxStepsAfterWinner"] && sim.get_not_active() < num_of_algorithms;)
	{
		if (rank_in_competition > 1){ // if there is a winner, start counting
			num_steps_after_winning++;
		}
		if (!winner && rank_in_competition > 1 && !called_about_to_finish) { // the first robot has finished and abotToFinish was never called
			winner = true;
			about_to_finish = true;
			called_about_to_finish = true;
		}
		if (rank_in_competition == 1 && sim.get_steps() == sim.get_max_steps() - config["MaxStepsAfterWinner"] && !called_about_to_finish){ // if steps == MaxSteps - MaxStepsAfterWinner, and aboutToFinish was never called, call it
			about_to_finish = true;
			called_about_to_finish = true;
		}
		rank_in_competition = sim.simulate_step(rank_in_competition, about_to_finish, message, is_video, algorithm_names, house_name); // do a simulation step

		if (message.compare("")){ // message != ""
			auto splitted_message = fp.split(message, ',');
			error_list.push_back("Algorithm " + algorithm_names[atoi(splitted_message[0].c_str())] +
				" when running on House " + house_name + " went on a wall in step " + splitted_message[1]);
			message = "";
		}
		about_to_finish = false;
	}

	// finish the simulation
	sim.finish_simulation();
}

/**
* Function that calculates the score matrix and prints it.
**/
void Main::score_simulation(vector<Simulator*>& sim_arr, score_formula formula, int num_of_houses, int num_of_algorithms){
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
			map<string, int> score_params;
			score_params["actual_position_in_competition"] = cur_robot->get_position_in_competition();
			score_params["simulation_steps"] = sim_arr[i]->get_steps();
			score_params["winner_num_steps"] = sim_arr[i]->get_winner_num_steps();
			score_params["this_num_steps"] = cur_robot->get_num_of_steps();
			score_params["sum_dirt_in_house"] = cur_robot->get_house()->get_sum_dirt_in_house();
			score_params["dirt_collected"] = cur_robot->get_dirt_collected();
			score_params["is_back_in_docking"] = cur_robot->get_house()->get_house_matrix()[loc.first][loc.second] == 'D' ? 1 : 0;

			score_matrix[j][i] = formula(score_params); // calculate score
			if (score_matrix[j][i] == -1){ // score calculation failed
				error_list.push_back("Score formula could not calculate some scores, see -­1 in the results table");
			}
		}
	}

	vector<pair<pair<int, string>, double>> algorithm_avg;
	for (int i = 0; i < num_of_algorithms; i++)
	{
		algorithm_avg.push_back(pair<pair<int, string>, double>(pair<int, string>(i, algorithm_names[i]), calc_avg(score_matrix, i, num_of_houses)));
	}


	sort(algorithm_avg.begin(), algorithm_avg.end(), [](const pair<pair<int,string>, double>& p1, const pair<pair<int, string>, double>& p2){ if (p1.second == p2.second) return p1.first.second.compare(p2.first.second) < 0; else return p2.second - p1.second < 0; });


	print_score_and_errors(sim_arr, score_matrix, algorithm_avg);

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
void Main::print_score_and_errors(vector<Simulator*>& sim_arr, int** score_matrix, vector<pair<pair<int, string>, double>>& algorithm_avg){

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
		string name(algorithm_avg[algo_index].first.second);
		tmp << name << string(13 - name.length(), ' ');
		for (int j = 0; j < length; j++)
		{
			string score = to_string(score_matrix[algorithm_avg[algo_index].first.first][j]);
			tmp << "|" << string(10 - score.length(), ' ') << score;
		}
		double d = algorithm_avg[algo_index].second;
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
void Main::deleting_memory(vector<House*>& house_arr, vector<Simulator*>& sim_arr){

	// delete simulator array
	int size1 = sim_arr.size();
	for (int i = 0; i < size1; ++i){
		delete sim_arr[i];
	}

	// delete houses array
	int size2 = house_arr.size();
	for (int i = 0; i < size2; ++i){
		delete house_arr[i];
	}

	Registrar::get_instance().clear_registrar();

#ifdef __gnu_linux__
	// close all dynamic links we opened
	int size3 = dl_arr.size();
	for (int i = 0; i < size3; i++){
		dlclose(dl_arr[i]);
	}
#endif
}