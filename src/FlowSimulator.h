#include "Fluid.h"
#include "Pipe.h"
#include <vector>

using namespace std;
	
enum FlowRegime{
	// Reynolds number less than 2300
	LaminarFlow,
	// Reynolds number between 2300 and 4000
	TransitionFlow,
	// Reynolds number greater than 4000
	TurbulentFlow
};

class FlowSimulator{
	private:	
		//Flow rate [m3/s]
		double flowRate;

		//P	- initial fluid pressure [Pa]
		double initialPressure;

		//The fluid circulating in the pipeline
		Fluid fluid;

		//The pipeline
		vector<Pipe> pipes;

		unsigned int getPipeIndex(double depth);
		double getPipelineLength();

		enum FlowRegime getFlowRegime(double reynoldsNumber);	
		double calculateReynoldsNumber(double fluidDensity, double fluidViscosity, double fluidVelocity, double pipeDiameter);
		double calculateFrictionFactor(double roughnessHeight, double diameter, double reynoldsNumber);
		double calculateFrictionFactorTurbulent(double roughnessHeight, double diameter, double reynoldsNumber);

	public:
		void addPipe(Pipe pipe);
		void setFluid(Fluid fluid);
		void setFlowRate(double flowRate);
		void setInitialPressure(double initialPressure);
		void computePressureGradient(double finalDepth, double segmentLength);	
};

	

	

