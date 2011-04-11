#include <cmath>
#include "Pipe.h"

/********************************************//**	
* Calculate the pipe's section area (πr^2)

* @return The pipe's surface area [m2]
***********************************************/
double Pipe::getArea(){
	return PI * pow(diameter / 2.0, 2.0);
}