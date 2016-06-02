/* nadavkareen 316602689; noamiluz 201307436 */
#include "House.h"
#include <tuple>


House::House(const House& house) :
m_short_name(house.m_short_name), m_max_steps(house.m_max_steps), m_rows(house.m_rows), m_cols(house.m_cols),
m_docking_station(house.m_docking_station), m_sum_dirt(house.m_sum_dirt){

	m_house_matrix = new string[m_rows];
	for (int i = 0; i < m_rows; i++){
		m_house_matrix[i] = house.m_house_matrix[i];
	}

}

/**
* Function creates a montage to this house, with a given
* name of the algorithm currently cleaning the house.
**/
tuple<string,bool> House::create_montage(const string& algo_name, int curr_number_of_step, string& house_name, const pair<int, int>& curr_location) const{

	vector<string> tiles;
	for (int curr_row = 0; curr_row < m_rows; ++curr_row)
	{
		for (int curr_col = 0; curr_col < m_cols; ++curr_col)
		{
			if (curr_row == curr_location.first && curr_col == curr_location.second){
				tiles.push_back("R");
			}
			else if (m_house_matrix[curr_row][curr_col] == ' ')
				tiles.push_back("0");
			else
				tiles.push_back(string() + m_house_matrix[curr_row][curr_col]);
		}
	}
	string images_dir_path = "images/" + algo_name + "_" +  house_name; // "algo_name" ends with "_"
	bool b1 = create_directory_if_not_exists(images_dir_path);
	if (!b1){
		return make_tuple(images_dir_path, false);
	}
	string counter_str = to_string(curr_number_of_step);
	string composed_image = images_dir_path + "/image" + string(6 - counter_str.length(), '0') + counter_str + ".jpg";
	bool b2 = Montage::compose(tiles, m_cols, m_rows, composed_image);
	return make_tuple("", b2);
}

/**
* Function creats a new directory in the given dirPath,
* if one doesn't exists.
**/
bool House::create_directory_if_not_exists(const string& dir_path) const
{
	string cmd = "mkdir -p " + dir_path;
	int ret = system(cmd.c_str());
	if (ret == -1)
	{
		return false;
	}
	return true;
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