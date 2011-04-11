#include <iostream>
#include "FlowSimulator.h"

using namespace std;

int main(){

	Pipe pipe1;
	pipe1.angle = -8.0 * PI / 180.0;
	pipe1.diameter = 0.2;
	pipe1.roughnessHeight = 0.2e-3;
	pipe1.length = 1000;
	 
	Pipe pipe2;
	pipe2.angle = -8.0 * PI / 180.0; 
	pipe2.diameter = 0.1;
	pipe2.roughnessHeight = 0.2e-3;
	pipe2.length = 1000;

	Fluid fluid;
	fluid.density = 800;
	fluid.dinamicViscosity = UnitsManager::cPToPas(100);

	FlowSimulator flowSimulator;
	flowSimulator.setFluid(fluid);
	flowSimulator.addPipe(pipe1);
	flowSimulator.addPipe(pipe2);
	flowSimulator.setFlowRate(UnitsManager::m3DayToM3Seconds(8000));
	
	flowSimulator.setInitialPressure(UnitsManager::atmToPa(1));
    flowSimulator.computePressureGradient(2000, 100);
	cout << "---" << endl;

	flowSimulator.setInitialPressure(17633884.035);
	flowSimulator.computePressureGradient(2000, 100);

	getchar();
	return 0;
}