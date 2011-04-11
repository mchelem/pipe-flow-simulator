#include "UnitsManager.h"

double UnitsManager::atmToPa(double x){
	// 1 atm = 101 325 pascals
	return x * 101325;
}

double UnitsManager::cPToPas(double x){
	// 1 cP = 0.001 Pa
	return x * 1e-3;
}

double UnitsManager::m3DayToM3Seconds(double x){
	// 1 day = 86400 seconds
	return x / 86400.0;

}