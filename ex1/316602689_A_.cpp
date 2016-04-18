/* nadavkareen 316602689; noamiluz 201307436 */
#include "316602689_A_.h"
#include <iostream>

/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_A::step() {
	int size = m_path_stack.size();
	if (m_battery_level == size + 1){
		m_few_battery = true;
		cout << "battery flag up" << endl;
	}

	if (m_battery_level >= m_config["BatteryCapacity"]){
		m_few_battery = false;
		cout << "battery flag down" << endl;
	}
	
	if (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0){
		cout << "in docking!!!!!!" << endl;
	}
	cout << "battery: " << m_battery_level << endl;

	if (m_about_to_finish_flag || m_few_battery){

		if (m_path_stack.empty() || (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0)){// stay if you are in the 'D' station
			if (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0){
				m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
			}
			else{
				m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
			}
			return Direction::Stay;
		}
		Direction d = m_path_stack.top();
		m_path_stack.pop();

		switch (d)// update the m_relative_docking_location
		{
			case Direction::North:{
				m_relative_docking_location.second += 1;
				break;
			}
			case Direction::South:{
				m_relative_docking_location.second -= 1;
				break;
			}
			case Direction::East:{
				m_relative_docking_location.first += 1;
				break;
			}
			case Direction::West:{
				m_relative_docking_location.first -= 1;
				break;
			}
			default:
				break;
		}
		m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
		return d;
	}
	else { // not in about to finish
		bool wasInD = m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0;
		SensorInformation s_i = m_sensor->sense();
		if (s_i.dirtLevel > 0){// current position still dirty
			if (wasInD){
				m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
			}
			else{
				m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
			}
			return Direction::Stay;
		}
		else{// current position is clean
			// go to the following direction, in case there isn't a wall there by this order of prefernce: East, West, South, North.
			for (int i = 2; i < 6; i++)
			{
				if (!s_i.isWall[i % 4]){
					cout << "hi!!!!!!!" << endl;
					// push to stack the return step and update m_relative_docking_location
					switch ((Direction)i)
					{
					case Direction::North:
						m_path_stack.push(Direction::South);
						m_relative_docking_location.second += 1;
						break;
					case Direction::South:
						m_path_stack.push(Direction::North);
						m_relative_docking_location.second -= 1;
						break;				
					case Direction::East:
						m_path_stack.push(Direction::West);
						m_relative_docking_location.first += 1;
						break;				
					case Direction::West:
						m_path_stack.push(Direction::East);
						m_relative_docking_location.first -= 1;
						break;				
					default:
						m_path_stack.push(Direction::Stay); // should not get here
						break;
					}

					if (wasInD){
						m_battery_level = min(m_config["BatteryCapacity"], m_battery_level + m_config["BatteryRechargeRate"]);
					}
					else{
						m_battery_level = max(0, m_battery_level - m_config["BatteryConsumptionRate"]);
					}
					return (Direction)i;
				}// end if
			}
			return Direction::Stay;//never should get here
		}
	}

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