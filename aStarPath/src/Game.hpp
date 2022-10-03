#pragma once
#include "Grid.hpp"

void inputHandle(void);

void updateState(void);

void drawNewState(const float *alpha);

int getRefRate(void);

void getGridStartingConditions(void);

void askFor(const char* message, int* variable, const char* varname);
