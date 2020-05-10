#include "AllegroError.h"
#include <iostream>

AllegroError::AllegroError(const char* errorStr_, const int& errorCode_) : errorCode(errorCode_)
{
	errorStr = _strdup(errorStr_);
}

const char* AllegroError::what() const { return errorStr; }
int AllegroError::code() const { return errorCode; }

AllegroError::~AllegroError() {};