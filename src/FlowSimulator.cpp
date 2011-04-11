#include "FlowSimulator.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

//--- Simulator setup methods 
void FlowSimulator::addPipe(Pipe pipe){
	pipes.push_back(pipe);
}

void FlowSimulator::setFluid(Fluid newFluid){
	fluid = newFluid;
}

void FlowSimulator::setFlowRate(double newflowRate){
	flowRate = newflowRate;
}		

void FlowSimulator::setInitialPressure(double newInitialPressure){
	initialPressure = newInitialPressure;
}
//---

/********************************************//**
* Get the pipe index in the pipeline corresponding to a depth
* 
* @param[in] depth The depth in the pipeline [m]
*
* @return The index indentifying the pipe
***********************************************/
unsigned int FlowSimulator::getPipeIndex(double depth){
	unsigned int i = 0;
	double length = 0.0;
	for (i = 0; i < pipes.size(); i++){
		length += pipes[i].length;
		if (length > depth){
			return i;
		}
	}
	return pipes.size() - 1;
}

/********************************************//**
* Get the pipeline length
* 
*
* @return The pipeline length [m]
***********************************************/
double FlowSimulator::getPipelineLength(){
	double length = 0.0;
	for (unsigned int i = 0; i < pipes.size(); i++){
		length += pipes[i].length;
	}
	return length;
}

/********************************************//**
* Get the flow regime corresponding to a given reynolds number
* 
* @param[in] reynoldsNumber Reynolds number [dimensionless]
* 
* @return The flow regime (laminar, transition or turbulent)
***********************************************/
enum FlowRegime FlowSimulator::getFlowRegime(double reynoldsNumber){
	// The flow is laminar when the Reynolds number is < 2300. 
	// If it is between 2300 and 4000, it is a transition flow, 
	// otherwise it is turbulent.
	FlowRegime flowRegime = TurbulentFlow;
	if (reynoldsNumber < 2300){
		flowRegime = LaminarFlow;
	} else if (reynoldsNumber < 4000){
		flowRegime = TransitionFlow;
	} else {
		flowRegime = TurbulentFlow;
	}
	return flowRegime;
}

/********************************************//**	
* Calculate the Reynolds number
* 
* @param[in] fluidDensity Fluid density [kg/m3]
* @param[in] fluidViscosity Fluid dynamic viscosity	[Pa.s]
* @param[in] fluidVelocity Fluid velocity [m/s]
* @param[in] pipeDiameter Pipe diameter [m]
* 
* @return Reynolds number [dimensionless]
***********************************************/
double FlowSimulator::calculateReynoldsNumber(double fluidDensity, double fluidViscosity, double fluidVelocity, double pipeDiameter){
	// Re = (ρvD)/μ
	double reynoldsNumber = (fluidDensity * fluidVelocity * pipeDiameter) / fluidViscosity;
	return reynoldsNumber;
}

/********************************************//**	
* Calculate the friction factor for transition and
* turbulent flow regimen
* 
* @param[in] roughnessHeight Roughness height [m]
* @param[in] diameter Pipe diameter [m]
* @param[in] reynoldsNumber Reynolds number [dimensionless]
* 
* @return Friction factor [dimensionless]
***********************************************/
double FlowSimulator::calculateFrictionFactorTurbulent(double roughnessHeight, double diameter, double reynoldsNumber){
	int maxIterations = 1000;
	double tolerance = 1e-8;

	// friction factor initial guess
	double ff = reynoldsNumber / 64.0;
	double f = 0.0;	
	double error = tolerance + 1.0;

	double e = roughnessHeight;
	double D = diameter;
	double Re = reynoldsNumber;

	for (int iteration = 0; iteration < maxIterations && error > tolerance; iteration++){
		f = 1.0 / pow(-2 * log10((e / (3.7 * D)) + (2.51 / (Re * sqrt(ff)))), 2);
		error = fabs((f - ff) / ff);
		ff = f;
	}
	return ff;
}

/********************************************//**	
* Calculate the friction factor
* 
* @param[in] roughnessHeight Roughness height [m]
* @param[in] diameter Pipe diameter [m]
* @param[in] reynoldsNumber Reynolds number [dimensionless]
* 
* @return Friction factor [dimensionless]
***********************************************/
double FlowSimulator::calculateFrictionFactor(double roughnessHeight, double diameter, double reynoldsNumber){
	double frictionFactor = 0.0;
	FlowRegime flowRegime = getFlowRegime(reynoldsNumber);	

	// When the flow is laminar, you can use the following equation: f = 64 / Re (Darcy–Weisbach friction factor)
	if (flowRegime == LaminarFlow){
		frictionFactor = 64 / reynoldsNumber;
	} else {
		frictionFactor = calculateFrictionFactorTurbulent(roughnessHeight, diameter, reynoldsNumber);
	}

	//cout << "reynolds number: " << fixed << setprecision(8) << reynoldsNumber << endl;
	//cout << "friction factor: " << fixed << setprecision(8) << frictionFactor << endl;
	return frictionFactor;
}
		
/********************************************//**	
* Compute the pressure gradient
* 
* @param[in] finalDepth The final depth [m]
* @param[in] segmentLength The segment lenght [m]
* 
***********************************************/
void FlowSimulator::computePressureGradient(double finalDepth, double segmentLength){
	double dP_dx = 0;
	double pressure = initialPressure;
	if (finalDepth > getPipelineLength())
		finalDepth = getPipelineLength();

	cout << "Depth\t" << "Pressure\t" << endl;

	for (double depth = 0; depth <= finalDepth; depth+=segmentLength){
		unsigned int pipeIndex = getPipeIndex(depth);
		unsigned int pipeIndex1 = getPipeIndex(depth+segmentLength);

		Pipe pipe = pipes[pipeIndex];	
		fluid.velocity = flowRate / pipe.getArea();
		double reynoldsNumber = calculateReynoldsNumber(fluid.density, fluid.dinamicViscosity, fluid.velocity, pipe.diameter);
		double frictionFactor = calculateFrictionFactor(pipe.roughnessHeight, pipe.diameter, reynoldsNumber);

		double dv_dx = 0;
		if (depth > 0)
			dv_dx = (flowRate / pipes[pipeIndex1].getArea() - flowRate / pipes[pipeIndex].getArea()) / segmentLength;

		// -ρv(dv/dx)
		double advectiveTerm = -fluid.density * fluid.velocity * dv_dx;
		// -ρg(sinθ)
		double gravitationalTerm = -fluid.density * GRAVITY_ACCELERATION * sin(pipe.angle);
		// -fρv^2/2D
		double frictionTerm = (-frictionFactor * fluid.density * pow(fluid.velocity, 2)) / (2 * pipe.diameter);	

		pressure = pressure + dP_dx * segmentLength;
		//cout << "depth: " << depth << " adv: " << advectiveTerm << " grav: " << gravitationalTerm << " fric: " << frictionTerm << " press: " << fixed << setprecision(4) << pressure << endl;
		cout << setprecision(2) << depth << "\t" << fixed << setprecision(4) << pressure << endl;
		dP_dx = advectiveTerm + gravitationalTerm + frictionTerm;		
		
	}
}
