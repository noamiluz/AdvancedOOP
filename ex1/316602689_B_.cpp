/* nadavkareen 316602689; noamiluz 201307436 */
#include "316602689_B_.h"

/**
* Function that returns a seggested step, according to
* a deterministic choise of the algorithm.
* This function is calles by the simulation for
* each time unit.
**/
Direction _316602689_B::step() {

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
			// go to the following direction, in case there isn't a wall there by this order of prefernce: West, South, North, East.
			for (int i = 1; i < 5; i++)
			{
				if (!s_i.isWall[i % 4]){

					// push to stack the return step and update m_relative_docking_location
					switch ((Direction)(i % 4))
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

					return (Direction)(i%4);
				}// end if
			}
			return Direction::Stay;//never should get here
		}
	}

}

extern "C" {
	AbstractAlgorithm *maker(){
		return new _316602689_B();
	}

	class proxy {
	public:
		proxy(){
			// register the maker with the factory 
			factory["316602689_B_"] = maker;
		}
	};
	// our one instance of the proxy
	proxy p;
}