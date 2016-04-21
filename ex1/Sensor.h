/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __SENSOR_H 
#define __SENSOR_H 

#include <stdio.h>
#include <iostream>

#include "House.h"
#include "AbstractSensor.h"
#include "Direction.h"
#include "SensorInformation.h"

using namespace std;

/**
* Sensor class. Represents a sensor,
* that contains an House instance,
* and the current location of the sensor in
* this house. His main method, sense() returns
* a local view of the house member.
**/
class Sensor : public AbstractSensor {
	House* m_house;
	pair<int, int> m_curr_location;

public:

	Sensor() : m_house(nullptr), m_curr_location(0, 0){
	}

	~Sensor(){}

	// returns the sensor's information of the current location of the robot
	virtual SensorInformation sense() const;

	House* get_house() const{
		return m_house;
	}

	void set_house(House* house){
		m_house = house;
	}

	const pair<int, int>& get_curr_location() const {
		return m_curr_location;
	}

	void set_curr_location(pair<int, int>& location){
		m_curr_location = location;
	}
};


#endif // __SENSOR_H 