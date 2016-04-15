#include "316602689_A_.h"


/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_A::step() {

	if (m_about_to_finish_flag){
		if (m_path_stack.empty() || (m_relative_docking_location.first == 0 && m_relative_docking_location.second == 0)){// stay if you are in the 'D' station
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
		return d;
	}
	else { // not in about to finish
		SensorInformation s_i = m_sensor->sense();
		if (s_i.dirtLevel > 0){// current position still dirty
			return Direction::Stay;
		}
		else{// current position is clean
			// go to the following direction, in case there isn't a wall there by this order of prefernce: East, West, South, North.
			for (int i = 0; i < 4; i++)
			{
				if (!s_i.isWall[i]){

					// push to stack the return step and update m_relative_docking_location
					switch ((Direction)i)
					{
					case Direction::North:{
						m_path_stack.push(Direction::South);
						m_relative_docking_location.second += 1;
						break;
					}
					case Direction::South:{
						m_path_stack.push(Direction::North);
						m_relative_docking_location.second -= 1;
						break;
					}
					case Direction::East:{
						m_path_stack.push(Direction::West);
						m_relative_docking_location.first += 1;
						break;
					}
					case Direction::West:{
						m_path_stack.push(Direction::East);
						m_relative_docking_location.first -= 1;
						break;
					}
					default:
						m_path_stack.push(Direction::Stay); // should not get here
						break;
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