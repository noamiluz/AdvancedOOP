/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __REGISTRAR_H_
#define __REGISTRAR_H_
#include <vector>
#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"

using namespace std;

class Registrar {

	vector<function<unique_ptr<AbstractAlgorithm>()>> _makers; // it will be sorted according to names of algorithms (because they are dlopened in a sorted order)
	static Registrar instance;

public:

	void insert_algorithm(function<unique_ptr<AbstractAlgorithm>()> maker);

	int get_num_of_registered_algorithms();

	vector<AbstractAlgorithm*> get_algorithms();

	void clear_registrar();

	static Registrar& get_instance();

};

#endif