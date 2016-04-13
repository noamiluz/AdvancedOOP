#include "316602689_C_.h"

// step is called by the simulation for each time unit
Direction _316602689_C::step() {
	if (m_about_to_finish_flag){
		if (m_path_stack.empty()){
			return Direction::Stay;
		}
		Direction d = m_path_stack.top();
		m_path_stack.pop();
		return d;
	}
	else { // not in about to finish
		SensorInformation s_i = m_sensor->sense();
		if (s_i.dirtLevel > 0){// current position still dirty
			return Direction::Stay;
		}
		else{// current position is clean
			// go to the following direction, in case there isn't a wall there by this order of prefernce: East, West, South, North
			for (int i = 2; i < 6; i++)
			{
				if (!s_i.isWall[i % 4]){
					// push to stack the return step
					switch ((Direction)i)
					{
					case Direction::North:{
						m_path_stack.push(Direction::South);
						break;
					}
					case Direction::South:{
						m_path_stack.push(Direction::North);
						break;
					}
					case Direction::East:{
						m_path_stack.push(Direction::West);
						break;
					}
					case Direction::West:{
						m_path_stack.push(Direction::East);
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
		return new _316602689_C();
	}

	class proxy {
	public:
		proxy(){
			// register the maker with the factory 
			factory["316602689_C_"] = maker;
		}
	};
	// our one instance of the proxy
	proxy p;
}