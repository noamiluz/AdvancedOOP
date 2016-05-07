/* nadavkareen 316602689; noamiluz 201307436 */

#include "MakeUnique.h"
#include "AlgorithmRegistration.h"
#include "Registrar.h"


AlgorithmRegistration::AlgorithmRegistration(function<unique_ptr<AbstractAlgorithm>()> maker){
	Registrar::get_instance().insert_algorithm(maker);
}