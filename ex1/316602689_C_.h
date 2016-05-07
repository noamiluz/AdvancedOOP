/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_C_H 
#define __316602689_C_H 

#include <stdio.h>
#include <stack>
#include <algorithm>
#include "AbstractAlgorithm.h"
#include "GenericAlgorithm.h"
#include "AlgorithmRegistration.h"

using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making algorithms 
extern map<string, maker_t *, less<string> > factory;


/**
* _316602689_C class. A determenistic algorithm,
* that activate  by this order of prefernce:  South, North, 
* East and West,(if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_C : public GenericAlgorithm{

public:

	_316602689_C() : GenericAlgorithm() { }

	~_316602689_C() {}

	_316602689_C(const _316602689_C&) = delete;

	_316602689_C& operator=(const _316602689_C&) = delete;

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep);

};

#endif // __316602689_C_H 
