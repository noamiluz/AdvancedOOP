#ifndef __ABSTRACT_SENSOR__H_
#define __ABSTRACT_SENSOR__H_

#include "SensorInformation.h"


/**
* AbstractSensor is a pure virtul class. Represents
* a sensor that can sense a house (usually will be represented by a matrix),
* and returns a local view of the current location in the house.
**/
class AbstractSensor
{
public:
	virtual ~AbstractSensor() {}
	// returns the sensor's information of the current location of the robot 
	virtual SensorInformation sense() const = 0;
};

#endif //__ABSTRACT_SENSOR__H_