#ifndef __SENSOR_INFORMATION__H_
#define __SENSOR_INFORMATION__H_

/**
* A struct contains a local view on a specific house matrix
* (i.e if surrounding to the current location there are walls,
* and the amount of dirt in current location).
**/
struct SensorInformation
{
	int dirtLevel;
	bool isWall[4];
};

#endif //__SENSOR_INFORMATION__H_