/* nadavkareen 316602689; noamiluz 201307436 */

#include "Registrar.h"

using namespace std;

Registrar Registrar::instance;

void Registrar::insert_algorithm(function<unique_ptr<AbstractAlgorithm>()> maker){
	Registrar::_makers.push_back(maker);
}

int Registrar::get_num_of_registered_algorithms(){
	return (int)Registrar::_makers.size();
}

vector<AbstractAlgorithm*> Registrar::get_algorithms() {
	vector<AbstractAlgorithm*> result;
	for (auto& maker : Registrar::_makers){
		result.push_back(maker().release());
	}
	return result;
}

void Registrar::clear_registrar(){
	_makers.clear();
}

Registrar& Registrar::get_instance(){
	return instance;
}


