#pragma once
#include <cmath>

double find_True_Solution(const double& X, const double& U_0) {
	return exp(-X / 2)*U_0;
}