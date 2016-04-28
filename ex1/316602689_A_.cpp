/* nadavkareen 316602689; noamiluz 201307436 */
#include "316602689_A_.h"
#include <iostream>
/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_A::step(Direction prevStep) {

	// update the amount of dirt in the last location
	char ch = get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second];
	get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second] = ch < 49 || ch > 57 ? ' ' : (char)(ch - 1);

	set_battery_level();
	update_relative_locations_and_stack(prevStep);

	int size = m_path_stack.size();
	if (m_battery_level == (size + 2) * m_config["BatteryConsumptionRate"]){
		m_few_battery = true;
	}

	if (m_battery_level >= m_config["BatteryCapacity"]){
		m_few_battery = false;
	}
	
	if (m_about_to_finish_flag || m_few_battery){
		if (m_path_stack.empty() || (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0)){// stay if you are in the 'D' station
			return Direction::Stay;
		}
		Direction d = m_path_stack.top();
		m_path_stack.pop();

		return d;
	}
	else { // not in about to finish
		SensorInformation s_i = m_sensor->sense();
		update_scan_house_matrix(s_i);
		if (s_i.dirtLevel > 0){ // current position still dirty
			return Direction::Stay;
		}
		else{ // current position is clean
			// go to the following direction, in case there isn't a wall there by this order of prefernce: East, West, South, North.
			for (int i = 0; i < 4; i++)
			{
				if (!s_i.isWall[i]){
					return (Direction)i;
				}// end if
			}
			return Direction::Stay;//never should get here
		}
	}

}

void _316602689_A::set_battery_level(){// TO DO -to comment

	// case before two steps the robot was at 'D' (attention - m_relative_docking_location isn't update to the last step taken yet)
	bool wasInD = m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0;
	if (wasInD){ // for any LAST step that was taken, there is only a charging of battery
		m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
	}
	else{ // for any LAST step that was taken, there is only a consumption of battery
		m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
	}

}

void _316602689_A::update_relative_locations_and_stack(Direction prevStep){// TO DO - to comment
	
	switch (prevStep)
	{
	case Direction::North:
		m_path_stack.push(Direction::South);
		m_relative_docking_location.first += 1;
		m_scan_matrix_location.first += 1;
		break;
	case Direction::South:
		m_path_stack.push(Direction::North);
		m_relative_docking_location.first -= 1;
		m_scan_matrix_location.first -= 1;
		break;
	case Direction::East:
		m_path_stack.push(Direction::West);
		m_relative_docking_location.second -= 1;
		m_scan_matrix_location.second -= 1;
		break;
	case Direction::West:
		m_path_stack.push(Direction::East);
		m_relative_docking_location.second += 1;
		m_scan_matrix_location.second += 1;
		break;
	default: // there is no need to update m_relative_docking_location
		m_path_stack.push(Direction::Stay); // should not get here
		break;
	}


}


void _316602689_A::init_matrix(string* scan_house_matrix){
	scan_house_matrix = new string[m_rows];
	for (int i = 0; i < m_rows; i++){
		scan_house_matrix[i] = string(m_cols, 'X');
	}

	// setting 'D' in location [5,5] in the matrix 
	scan_house_matrix[5][5] = 'D';
}


// 'X' for loc not scan yet, 'D' for docking, '*' for not a wall but no more info, ' ' for no dirt, '1'...'9'-for dirt
void _316602689_A::update_scan_house_matrix(SensorInformation info){ //  TO DO- to comment

	// easy implementation first (no matrix extenrion)
	// update current location
	if (info.dirtLevel == 0 && get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second] != 'D'){
		get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second] = ' ';
	}
	else if (info.dirtLevel > 0){
		get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second] = '0' + info.dirtLevel;
	}

	//update current location surrounding
	if (m_scan_matrix_location.second + 1 >= m_cols){// check matrix bounderies - TO DO
		cout << "it's just for now... the matrix is to small" << endl;
	}
	else{
		char temp_ch = get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second + 1];
		get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second + 1] = info.isWall[0] ? 'W' :
			(temp_ch == 'X' ? '*' : temp_ch); // i.e - if we didnf scan this loc ('X') change it to '*', otherwise, we allready have info on this loc.
	}
	 
	//// TO CONTINUE FROM HERE


	char ch = get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second];
	get_scan_house_matrix()[m_scan_matrix_location.first][m_scan_matrix_location.second] = ch < 49 || ch > 57 ? ' ' : (char)(ch - 1);




}


extern "C" {
	AbstractAlgorithm *maker(){
		return new _316602689_A();
	}

	class proxy {
	public:
		proxy(){
			// register the maker with the factory 
			factory["316602689_A_"] = maker;
		}
	};
	// our one instance of the proxy
	proxy p;
}