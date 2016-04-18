/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __316602689_A_H 
#define __316602689_A_H 

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
* _316602689_A class. A determenistic algorithm,
* that activate  by this order of prefernce: East, West,
* South and North (if possible). Maintains it's path back
* to docking station, to be used when 'about_to_finish'
* flag turns on.
**/
class _316602689_A : public GenericAlgorithm {

public:

	_316602689_A() : GenericAlgorithm() { }

	~_316602689_A() {}

	_316602689_A(const _316602689_A&) = delete;

	_316602689_A& operator=(const _316602689_A&) = delete;

	// step is called by the simulation for each time unit
	virtual Direction step();
};



#endif // __316602689_A_H 
