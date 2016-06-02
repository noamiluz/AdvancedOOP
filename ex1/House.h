/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __HOUSE_H 
#define __HOUSE_H 


#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "Montage.h"

using namespace std;

/**
* House class. Represents a given house,
* containng a short name, a valid max steps number
* that a robot can execute on this house.
* In addition, cantains the house repressentaion(by a matrix),
* it's size, it's docking position and the amount of dirt in it.
**/
class House{
	const string m_short_name;
	const int m_max_steps;
	const int m_rows;
	const int m_cols;
	const pair<int, int> m_docking_station;
	string* m_house_matrix;
	const int m_sum_dirt;
	bool m_flag; // true iff the aboutToFinish was called for this house

	// Returns the sum of all the dust in the house. <private method, called once by the constructor>
	int count_dirt() const;

public:

	House(const string& short_name, const int& max_steps, const int& rows, const int& cols,
		const pair<int, int>& docking_station, string* house_matrix) :
		m_short_name(short_name), m_max_steps(max_steps), m_rows(rows), m_cols(cols),
		m_docking_station(docking_station), m_house_matrix(house_matrix), m_sum_dirt(count_dirt()), m_flag(false) {}

	~House(){
		delete[] m_house_matrix;
	}

	House(const House& house);

	House& operator=(const House& house) = delete;

	string* get_house_matrix() const{
		return m_house_matrix;
	}

	const int& get_house_matrix_rows() const{
		return m_rows;
	}

	const int& get_house_matrix_cols() const{
		return m_cols;
	}

	const pair<int, int>& get_house_docking_station() const{
		return m_docking_station;
	}

	const string get_house_short_name() const {
		return m_short_name;
	}

	const int get_max_steps() const{
		return m_max_steps;
	}

	const int get_sum_dirt_in_house() const {
		return m_sum_dirt;
	}

	bool get_flag(){
		return m_flag;
	}

	void set_flag(bool b){
		m_flag = b;
	}

	tuple<string, bool> create_montage(const string& algo_name, int curr_number_of_step, string& house_name, const pair<int,int>& curr_location) const;

	bool create_directory_if_not_exists(const string& dirPath) const;
};

#endif // __HOUSE_H 