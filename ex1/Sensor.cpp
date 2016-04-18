/* nadavkareen 316602689; noamiluz 201307436 */
#include "Sensor.h"


/**
* Function that returns the sensor's information of a
* position in the current house.
* This function assumes that m_matrix[x][y] != 'W'(i.e - current location != 'W')
**/
SensorInformation Sensor::my_sense(const pair<int, int>& position) const {
	SensorInformation result;
	int x = position.first;
	int y = position.second;

	if (x - 1 >= 0){
		if (this->m_house->get_house_matrix()[x - 1][y] == 'W'){
			result.isWall[(int)Direction::North] = true;
		}
		else {
			result.isWall[(int)Direction::North] = false;
		}
	}
	if (x + 1 < this->m_house->get_house_matrix_rows()){
		if (this->m_house->get_house_matrix()[x + 1][y] == 'W'){
			result.isWall[(int)Direction::South] = true;
		}
		else {
			result.isWall[(int)Direction::South] = false;
		}
	}
	if (y - 1 >= 0){
		if (this->m_house->get_house_matrix()[x][y - 1] == 'W'){
			result.isWall[(int)Direction::West] = true;
		}
		else {
			result.isWall[(int)Direction::West] = false;
		}
	}
	if (y + 1 < this->m_house->get_house_matrix_cols()){
		if (this->m_house->get_house_matrix()[x][y + 1] == 'W'){
			result.isWall[(int)Direction::East] = true;
		}
		else {
			result.isWall[(int)Direction::East] = false;
		}
	}
	result.dirtLevel = this->m_house->get_house_matrix()[x][y] == ' ' || this->m_house->get_house_matrix()[x][y] == 'D' ? 0 : this->m_house->get_house_matrix()[x][y] - '0';
	return result;
}

/**
* Function that calls to my_sense() function, in order
* to get information of a position in the current house.
**/
SensorInformation Sensor::sense() const{
	return my_sense(m_curr_location);
}