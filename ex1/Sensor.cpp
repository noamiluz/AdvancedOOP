/* nadavkareen 316602689; noamiluz 201307436 */
#include "Sensor.h"


/**
* Function that calls to my_sense() function, in order
* to get information of a position in the current house.
*
* Assumption: house_matrix[m_cur_location.first][m_cur_location.second] != 'W'. 
**/
SensorInformation Sensor::sense() const{
	SensorInformation result;
	int x = m_curr_location.first;
	int y = m_curr_location.second;

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