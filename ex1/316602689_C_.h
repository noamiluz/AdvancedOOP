#ifndef __316602689_C_H 
#define __316602689_C_H 

#include <stdio.h>
#include <stack>
#include "AbstractAlgorithm.h"

using namespace std;

typedef AbstractAlgorithm *maker_t(const AbstractSensor& sensor, map<string, int>& config);

// our global factory for making shapes 
extern map<string, maker_t *, less<string> > factory;

class _316602689_C : public AbstractAlgorithm{
	const AbstractSensor* m_sensor;
	map<string, int> m_config; // configuration properties
	stack<Direction> m_path_stack; // current house path stack
	bool m_about_to_finish_flag;

public:

	_316602689_C(const AbstractSensor& sensor, map<string, int>& config) : m_about_to_finish_flag(false) {
		setSensor(sensor);
		setConfiguration(config);
	}

	~_316602689_C() {
		delete m_sensor;
	}

	_316602689_C(const _316602689_C&) = delete;

	_316602689_C& operator=(const _316602689_C&) = delete;

	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) {
		m_sensor = &sensor;
	}

	// setConfiguration is called once when the Algorithm is initialized - see below 
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

#endif // __316602689_C_H 
