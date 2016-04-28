/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_A_H 
#define __316602689_A_H 

#include <stdio.h>
#include <stack>
#include <algorithm>
#include "AbstractAlgorithm.h"
#include "GenericAlgorithm.h"

#define INIT_ROWS_SIZE 10
#define INIT_COLS_SIZE 10

using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making algorithms 
extern map<string, maker_t *, less<string> > factory;

/**
* _316602689_A class. A determenistic algorithm,
* that activate  by this order of prefernce: East, West,
* South and North (if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_A : public GenericAlgorithm {

	string* m_scan_house_matrix;
	pair<int, int> m_scan_matrix_location;
	const int m_rows;
	const int m_cols;

public:

	_316602689_A() : GenericAlgorithm(), m_rows(INIT_ROWS_SIZE), m_cols(INIT_COLS_SIZE), m_scan_matrix_location(5,5) {
		init_matrix(m_scan_house_matrix);
	}

	~_316602689_A() {}

	_316602689_A(const _316602689_A&) = delete;

	_316602689_A& operator=(const _316602689_A&) = delete;

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep);

	void set_battery_level();

	void update_relative_locations_and_stack(Direction prevStep);

	void init_matrix(string* scan_house_matrix);

	void update_scan_house_matrix(SensorInformation info);

	string* get_scan_house_matrix(){
		return m_scan_house_matrix;
	}

};



#endif // __316602689_A_H 
