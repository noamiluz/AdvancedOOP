/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_B_H 
#define __316602689_B_H 

#include <stdio.h>
#include <stack>
#include <algorithm>

#include "AbstractAlgorithm.h"
#include "GenericAlgorithm.h"

using namespace std;

typedef AbstractAlgorithm *maker_t();

// global factory for making algorithms 
extern map<string, maker_t *, less<string> > factory;


/**
* _316602689_B class. A determenistic algorithm,
* that activate  by this order of prefernce:  West, South,
* North and East(if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_B : public GenericAlgorithm{

public:

	_316602689_B() : GenericAlgorithm() { }

	~_316602689_B() {}

	_316602689_B(const _316602689_B&) = delete;

	_316602689_B& operator=(const _316602689_B&) = delete;

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep);

};

#endif // __316602689_B_H 
