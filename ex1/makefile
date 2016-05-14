CC = g++
LIBS = 

.cc.o:
	$(CC) -ggdb -c $<

default:
	make all

OBJS = testdcl.o

simulator: simulator.o House.o Sensor.o FilesInfrastructure.o AlgorithmRegistration.o Registrar.o Main.o
	g++ -rdynamic -o $@ $^ -std=c++11 -Wall -O2 -pedantic -lpthread -ldl

simulator.o: simulator.cpp simulator.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h House.h Sensor.h Robot.h Main.h FilesInfrastructure.h
	g++ -c -std=c++11 -Wall -pedantic simulator.cpp

Main.o: Main.cpp Main.h simulator.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h FilesInfrastructure.h House.h Sensor.h Robot.h AlgorithmRegistration.h Registrar.h MakeUnique.h
	g++ -c -std=c++11 -Wall -pedantic -lpthread Main.cpp

House.o: House.cpp House.h 
	g++ -c -std=c++11 -Wall -pedantic House.cpp

Sensor.o: Sensor.cpp Sensor.h House.h AbstractSensor.h Direction.h SensorInformation.h
	g++ -c -std=c++11 -Wall -pedantic Sensor.cpp

FilesInfrastructure.o: FilesInfrastructure.cpp FilesInfrastructure.h 
	g++ -c -std=c++11 -Wall -pedantic FilesInfrastructure.cpp

AlgorithmRegistration.o: AlgorithmRegistration.cpp AlgorithmRegistration.h Registrar.h Main.h
	g++ -c -std=c++11 -Wall -pedantic AlgorithmRegistration.cpp

Registrar.o: Registrar.cpp Registrar.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h
	g++ -c -std=c++11 -Wall -pedantic Registrar.cpp

316602689_B_.o: 316602689_B_.cpp 316602689_B_.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h AlgorithmRegistration.h
	g++ -c -std=c++11 -Wall -pedantic 316602689_B_.cpp -mcmodel=large 

316602689_B_.so: 316602689_B_.o
	g++ -shared -Wl,-soname,316602689_B_.so -o 316602689_B_.so 316602689_B_.o

316602689_A_.o: 316602689_A_.cpp 316602689_A_.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h AlgorithmRegistration.h
	g++ -c -std=c++11 -Wall -pedantic 316602689_A_.cpp -mcmodel=large 

316602689_A_.so: 316602689_A_.o
	g++ -shared -Wl,-soname,316602689_A_.so -o 316602689_A_.so 316602689_A_.o

316602689_C_.o: 316602689_C_.cpp 316602689_C_.h AbstractAlgorithm.h AbstractSensor.h Direction.h SensorInformation.h AlgorithmRegistration.h
	g++ -c -std=c++11 -Wall -pedantic 316602689_C_.cpp -mcmodel=large 

316602689_C_.so: 316602689_C_.o
	g++ -shared -Wl,-soname,316602689_C_.so -o 316602689_C_.so 316602689_C_.o

score_formula.o: score_formula.cpp 
	g++ -c -std=c++11 -Wall -pedantic score_formula.cpp -mcmodel=large 

score_formula.so: score_formula.o
	g++ -shared -Wl,-soname,score_formula.so -o score_formula.so score_formula.o


all: score_formula.so score_formula.o 316602689_A_.so 316602689_A_.o 316602689_B_.so 316602689_B_.o 316602689_C_.so 316602689_C_.o Registrar.o AlgorithmRegistration.o FilesInfrastructure.o Sensor.o House.o Main.o simulator.o simulator

clean:
	rm -f *.so *.o simulator