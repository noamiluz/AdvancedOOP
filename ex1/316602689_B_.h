#ifndef __316602689_B_H 
#define __316602689_B_H 

#include <stdio.h>
#include <stack>

#include "AbstractAlgorithm.h"

using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making shapes 
extern map<string, maker_t *, less<string> > factory;


/**
* _316602689_B class. A determenistic algorithm,
* that activate  by this order of prefernce:  West, South,
* North and East(if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_B : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	pair<int, int> m_relative_docking_location;
	bool m_about_to_finish_flag;


public:

	_316602689_B() : m_about_to_finish_flag(false){
	}

	~_316602689_B() {
	}

	_316602689_B(const _316602689_B&) = delete;

	_316602689_B& operator=(const _316602689_B&) = delete;

	// setSensor is called once(for each house) when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
		m_relative_docking_location = pair<int, int>(0, 0);
		m_path_stack.empty();
	}

	// setConfiguration is called once(for each house) when the Algorithm is initialized - see below 
	virtual void setConfiguration(map<string, int> config){
		m_config = config;
	}

	// step is called by the simulation for each time unit
	virtual Direction step();

	// this method is called by the simulation either when there is a winner or 
	// when steps == MaxSteps - MaxStepsAfterWinner 
	// parameter stepsTillFinishing == MaxStepsAfterWinner 
	virtual void aboutToFinish(int stepsTillFinishing){
		m_about_to_finish_flag = true;
	}

};

#endif // __316602689_B_H 
