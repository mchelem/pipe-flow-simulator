#include "UnitsManager.h"

class Pipe{
	public:
		// Calculate the pipe's area
		double getArea();

		// x - Pipe length	[m]
		double length;

		// θ	- Pipe angle [deg]
		double angle;

		// D	- Pipe diameter [m]
		double diameter;

		// ε	- Roughness height [m]
		double roughnessHeight;
};