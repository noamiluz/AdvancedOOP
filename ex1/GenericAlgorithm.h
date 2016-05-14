/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __GENERIC_ALGORITHM_H 
#define __GENERIC_ALGORITHM_H 

#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stack>
#include "AbstractAlgorithm.h"

class GenericAlgorithm : public AbstractAlgorithm {

protected:
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	pair<int, int> m_relative_docking_location; // the PREV location(relative to docking location)
	bool m_about_to_finish_flag;
	bool m_few_battery;
	int m_battery_level;

public:

	GenericAlgorithm() : m_about_to_finish_flag(false), m_few_battery(false), m_battery_level(m_config["BatteryCapacity"]) {}

	~GenericAlgorithm() {}

	GenericAlgorithm(const GenericAlgorithm&) = delete;

	GenericAlgorithm& operator=(const GenericAlgorithm&) = delete;

	// setSensor is called once(for each house) when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
		m_battery_level = m_config["BatteryCapacity"];
		m_relative_docking_location = pair<int, int>(0, 0);
		stack<Direction> s; // an empty stack
		m_path_stack.swap(s); // clear the m_path_stack
	}

	// setConfiguration is called once(for each house) when the Algorithm is initialized - see below 
	virtual void setConfiguration(map<string, int> config){
		m_config = config;
	}

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep) = 0;

	// this method is called by the simulation either when there is a winner or 
	// when steps == MaxSteps - MaxStepsAfterWinner 
	// parameter stepsTillFinishing == MaxStepsAfterWinner 
	virtual void aboutToFinish(int stepsTillFinishing){
		m_about_to_finish_flag = true;
	}



};


#endif // __GENERIC_ALGORITHM_H 