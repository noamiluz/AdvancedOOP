/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __MAIN_H_
#define __MAIN_H_

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
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"
#include "Registrar.h"
#include "simulator.h"

#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <functional>

#include <iomanip>
#include <limits.h>

#define PRINT_USAGE cout << "Usage: simulator [­-config <config path>] [-­house_path <house path>] [­-algorithm_path <algorithm path>] [­-score_formula <score.so path>] [­-threads <num threads>]" << endl

typedef int(*score_formula)(const map<string, int>&);


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
#ifdef __gnu_linux__
		return house_names;
#endif
#ifdef WIN32
		vector<string> tmp;
		tmp.push_back("001");
		return tmp;
#endif
	}

	// prints error list
	void print_errors(vector<string>& error_list);

	// checking if the content of configuration file is valid. If not, print message and return false.
	bool check_configurations_validity(const map<string, int>& config, string& temp_missing, string& temp_invalid, int& count_missing, int& count_invalid);

	// read from configuration file and return the configurations map
	map<string, int> get_configurations(const string path);

	// read from .house files and parse them into a house array;
	vector<House*> get_houses(string path);

	//function for completing miising cells in house matrix by ' '.
	//In addition, surranding the matrix with wall if needed. Also,
	//returns how many docking stations in the house, AFTER fixing
	int fix_house_matrix(string *matrix, int rows, int cols);

	// Function that loads '*.so' files that represent algorithms,
	// returns the number of successfully loaded algorithms, otherwise 0.
	int load_algorithms(const string& path);

	// returns a pointer to the score formula function.
	// load the .so file if necessary. otherwise, uses default score formula.
	score_formula get_score_formula(const string& path);

	// parse the command line arguments
	// returns a tuple <config_path, house_path, algorithm_path, score_formula_path, threads number>
	tuple<string, string, string, string, int> command_line_arguments(int argc, char* argv[]);

	// simulate the simulator
	void simulate(Simulator& sim, map<string, int>& config, int num_of_houses, int num_of_algorithms, string& house_name);

	// calculates the score matrix and prints it
	void score_simulation(vector<Simulator*>& sim_arr, score_formula formula, int num_of_houses, int num_of_algorithms);

	// trim title in the score matrix to be up to 9 chars and aligned to left
	string trim_title(string& title);

	// calculate average score (on all houses) of an algorithm with index 'index'
	double calc_avg(int** score_matrix, int index, int num_of_houses);

	// prints the score matrix according to given format.
	// prints errors after that, if exist.
	void print_score_and_errors(vector<Simulator*>& sim_arr, int** score_matrix, vector<pair<string, double>>& algorithm_avg);

	// freeing all the memory left to free in the program
	void deleting_memory(vector<House*>& house_arr, vector<Simulator*>& sim_arr);
};

#endif