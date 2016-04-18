/* nadavkareen 316602689; noamiluz 201307436 */
#include "House.h"



House::House(const House& house) :
m_short_name(house.m_short_name), m_max_steps(house.m_max_steps), m_rows(house.m_rows), m_cols(house.m_cols),
m_docking_station(house.m_docking_station), m_sum_dirt(house.m_sum_dirt){

	m_house_matrix = new string[m_rows];
	for (int i = 0; i < m_rows; i++){
		m_house_matrix[i] = house.m_house_matrix[i];
	}

}

/**
* Function returns the amonut of dust currently in the house.
**/
int House::count_dirt() const{
	int result = 0;
	for (int i = 0; i < m_rows; i++)
	{
		if (m_house_matrix[i].empty()){
			continue;
		}
		for (int j = 0; j < m_cols; j++)
		{

			if ((unsigned)j < m_house_matrix[i].length() && m_house_matrix[i][j] > 48 && m_house_matrix[i][j] < 58){ // in 1,...,9
				result += (int)(m_house_matrix[i][j] - '0');
			}
		}
	}
	return result;
}