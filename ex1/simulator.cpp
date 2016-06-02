/* nadavkareen 316602689; noamiluz 201307436 */

#ifdef WIN32
#include "316602689_B_.h"
#endif

#include <unistd.h>
#include "simulator.h"
#include "Main.h"

Simulator::Simulator(map<string, int>& config, vector<AbstractAlgorithm*>& algorithm_arr, House* house) :
m_config(config), m_steps(0), m_algorithm_arr(algorithm_arr),
m_num_of_algorithms(algorithm_arr.size()), m_max_steps(house->get_max_steps()), m_winner_num_steps(house->get_max_steps()), m_not_active(0) {
	init_robot_arr(house);
	// init m_prev_steps with Direction::Stay and create sensors
	for (int i = 0; i < m_num_of_algorithms; i++)
	{
		Sensor *tmp = new Sensor();
		m_algorithm_arr[i]->setConfiguration(m_config);
		m_algorithm_arr[i]->setSensor(*tmp);
		m_sensor_arr.push_back(tmp);
		m_prev_steps.push_back(Direction::Stay);
	}
}

Simulator::~Simulator(){
	// delete robots array
	for (int i = 0; i < m_num_of_algorithms; i++){
		delete m_robot_arr[i];
		delete m_algorithm_arr[i];
		delete m_sensor_arr[i];
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
int Simulator::simulate_step(int rank_in_competition, bool about_to_finish, string& message, bool is_video_on,
	const vector<string>& algorithm_names, string& house_name, vector<int>& failed_images, vector<string>& failed_directory){
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

		Direction d = m_algorithm_arr[i]->step(m_prev_steps[i]); // ask the algorithm what direction to go
		House* cur_house = cur_robot->get_house(); // get the house of the current robot
		const pair<int, int>& cur_loc = cur_robot->get_curr_location(); // get the current location of the robot
		pair<int, int> next_loc;
		
	
		///////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
		/*
		if (i == 1){
			cout << "simulator battery level: " << cur_robot->get_curr_battary_level() << endl;
			cout << "step number in the simulator " << m_steps << endl;
			for (int r = 0; r < cur_house->get_house_matrix_rows(); r++)
			{
				for (int c = 0; c < cur_house->get_house_matrix_cols(); c++)
				{
					if (r == cur_loc.first && c == cur_loc.second){
						cout << 'R';
						continue;
					}
					cout << cur_house->get_house_matrix()[r][c];
				}
				cout << endl;

			}
			cout << endl;
			//usleep(500000);

		}
		*/
		////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		
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
		// update m_prev_steps vector to contain the last preformed step
		m_prev_steps[i] = d;

		char ch = cur_house->get_house_matrix()[next_loc.first][next_loc.second]; // get the matrix item in the next location
		cur_robot->increment_num_of_steps(); // increment the number of steps the robot has done

		if (ch == 'W'){ // if there is a wall in the next location
			cur_robot->set_curr_location(next_loc); // set the next location as the current one - MAYBE to delete

			cur_robot->set_active(false);
			m_not_active++;
			cur_robot->set_valid(false);
			message = i + "," + m_steps;
			// in case that the argument -video is enterd by the user - create an image
			// of the current step of the current algorithem & house.
			if (is_video_on){
				auto tup = cur_robot->get_house()->create_montage(algorithm_names[i], m_steps, house_name, cur_loc);
				string dir = get<0>(tup);
				bool b_img = get<1>(tup);
				if (!b_img) {
					failed_images[i]++;
				}
				failed_directory[i] = dir;
			}
			continue;

		}
		// if the current location is a docking station, increment the curr_battery_level by RechargeRate.
		// if the battery is full do not increment.

		if (cur_house->get_house_matrix()[next_loc.first][next_loc.second] == 'D'){
			
			if (d == Direction::Stay){ // if the current move was 'stay in docking station' do not decrement battery
				cur_robot->set_curr_battery_level(min(m_config["BatteryCapacity"], cur_robot->get_curr_battary_level() + m_config["BatteryRechargeRate"]));
			}

			else { // if the current move was to docking station decrement battery.
				cur_robot->set_curr_battery_level(max(0, cur_robot->get_curr_battary_level() - m_config["BatteryConsumptionRate"]));
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

			cur_robot->set_curr_location(next_loc); // set the next location as the current one
			// in case that the argument -video is enterd by the user - create an image
			// of the current step of the current algorithem & house.
			if (is_video_on){
				auto tup = cur_robot->get_house()->create_montage(algorithm_names[i], m_steps, house_name, cur_loc);
				string dir = get<0>(tup);
				bool b_img = get<1>(tup);
				if (!b_img) {
					failed_images[i]++;
				}
				failed_directory[i] = dir;
			}
			continue;

		}
		// if the current move was docking station, and the next one isn't docking station, charge the battary and do not decrement.
	
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
			cur_robot->set_curr_location(next_loc); // set the next location as the current one -  MAYBE TO DELETE
			cur_robot->set_active(false);
			m_not_active++;
			cur_robot->set_position_in_competition(10);
			// in case that the argument -video is enterd by the user - create an image
			// of the current step of the current algorithem & house.
			if (is_video_on){
				auto tup = cur_robot->get_house()->create_montage(algorithm_names[i], m_steps, house_name, cur_loc);
				string dir = get<0>(tup);
				bool b_img = get<1>(tup);
				if (!b_img) {
					failed_images[i]++;
				}
				failed_directory[i] = dir;
			}
			continue;

		}
		cur_robot->set_curr_location(next_loc); // set the next location as the current one
		// in case that the argument -video is enterd by the user - create an image
		// of the current step of the current algorithem & house.
		if (is_video_on){
			auto tup = cur_robot->get_house()->create_montage(algorithm_names[i], m_steps, house_name, cur_loc);
			string dir = get<0>(tup);
			bool b_img = get<1>(tup);
			if (!b_img) {
				failed_images[i]++;
			}
			failed_directory[i] = dir;
		}
	}

	if (is_someone_finished){ // if there were robots who finished in this step increment the rank
		rank_in_competition += num_of_winners_this_step;
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
		// if the robot finished cleaning, or invalid, we continue
		if (!cur_robot->is_valid() || cur_robot->get_dirt_collected() == cur_robot->get_house()->get_sum_dirt_in_house()){
			continue;
		}
		// disactive the robot and set its position to be 10
		cur_robot->set_active(false);
		cur_robot->set_position_in_competition(10);
		cur_robot->set_num_of_steps(m_steps); // set this_num_steps to be simulation_stpes
	}
	
}

/*
void thread_simulation1(Main &main, vector<Simulator*>& simulator_arr, map<string, int>& config, int num_of_houses, int num_of_algorithms){
	while (main.num_of_simulated_houses < num_of_houses){
		main.num_of_simulated_houses++;
		main.simulate(*simulator_arr[main.num_of_simulated_houses], config, num_of_algorithms, main.get_house_names()[main.num_of_simulated_houses]);
	}
}*/


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
	string formula_path = get<3>(tup);
	const int num_of_threads = get<4>(tup);

	if (!config_path.compare("") && !house_path.compare("") && !algorithm_path.compare("") && !formula_path.compare("") && num_of_threads == 1){ // error
		return 1;
	}
	
	map<string, int> config = main.get_configurations(config_path); // getting configurations map
	if (config.empty()){
		return 1;
	}

	score_formula formula = main.get_score_formula(formula_path);
	if (formula == NULL){
		return 1;
	}
	
	vector<House*> house_arr = main.get_houses(house_path); // getting houses arr 
	if (house_arr.empty()){
		return 1;
	}
	
	const int num_of_algorithms = main.load_algorithms(algorithm_path);
	if (!num_of_algorithms){
		vector<Simulator*> empty;
		main.deleting_memory(house_arr, empty);
		return 1;
	}

	const int num_of_houses = house_arr.size();

	// creating vector of simulators, one for each house
	vector<Simulator*> simulator_arr;
	for (int i = 0; i < num_of_houses; i++){
		vector<AbstractAlgorithm*> tmp = Registrar::get_instance().get_algorithms();
		simulator_arr.push_back(new Simulator(config, tmp, house_arr[i]));
	}
	
	main.execute_simulation_multi_threaded(simulator_arr, config, num_of_threads, num_of_houses, num_of_algorithms);

	// if -video was entered - create the videos
	if (main.get_is_video()){
		main.encode_images_into_video();
	}

	// calculate the score of each robot
	main.score_simulation(simulator_arr, formula, num_of_houses, num_of_algorithms);

	// freeing memory
	main.deleting_memory(house_arr, simulator_arr);
	
	return 0;
}


