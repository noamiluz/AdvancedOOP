/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __ROBOT_H 
#define __ROBOT_H 

#include <stdio.h>
#include <iostream>

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"
#include "House.h"

using namespace std;

/**
* Robot class. Represents a robot,
* that cleans a given house according to a specific algorithm.
* Contains an House instance, it's current battary level and location,
* it's number of steps and position in competition and the
* amount of dirt collected.
**/
class Robot{
	House* m_house;
	int m_curr_battary_level;
	pair<int, int> m_curr_location;
	bool m_is_active; // is the robot currently active in the simulation
	int m_this_num_of_steps; // for the score formula
	int m_dirt_collected;
	int m_position_in_competition; // can be 1,2,3,4 or 10.
	bool m_is_valid; // false iff the robot got stuck in a wall (bad behavior)

public:

	Robot(int init_battary_level, House* house) :
		m_curr_battary_level(init_battary_level), m_curr_location(house->get_house_docking_station()), m_is_active(true),
		m_this_num_of_steps(0), m_dirt_collected(0), m_position_in_competition(0), m_is_valid(true) {
		m_house = new House(*house);
	}

	~Robot(){
		delete m_house;
	}

	Robot(const Robot& robot) = delete;

	Robot& operator=(const Robot& other_robot) = delete;


	House* get_house(){
		return m_house;
	}

	const int& get_curr_battary_level() const{
		return m_curr_battary_level;
	}

	const bool is_active() const {
		return m_is_active;
	}

	void set_curr_battery_level(const int new_level){
		m_curr_battary_level = new_level;
	}


	void set_active(bool active){
		m_is_active = active;
	}

	void increment_num_of_steps(){
		m_this_num_of_steps++;
	}

	void set_num_of_steps(int steps){
		m_this_num_of_steps = steps;
	}

	void increment_dirt_collected(){
		m_dirt_collected++;
	}

	const int get_num_of_steps() const {
		return m_this_num_of_steps;
	}

	const int get_dirt_collected() const {
		return m_dirt_collected;
	}

	const int get_position_in_competition() const {
		return m_position_in_competition;
	}

	const bool is_valid() const{
		return m_is_valid;
	}

	void set_position_in_competition(int position){
		m_position_in_competition = position;
	}

	void set_valid(bool valid){
		m_is_valid = valid;
	}

	pair<int, int>& get_curr_location(){
		return m_curr_location;
	}

	void set_curr_location(pair<int, int>& location){
		m_curr_location = location;
	}
};

#endif // __ROBOT_H 