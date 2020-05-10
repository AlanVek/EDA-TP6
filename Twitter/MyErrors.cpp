#include <iostream>
#include "MyErrors.h"

API_request_error::API_request_error(const char* errorCode_) { errorCode = _strdup(errorCode_); }

const char* API_request_error::what() const { return errorCode; }

API_request_error::~API_request_error() {};

AllegroError::AllegroError(const char* errorStr_, const int& errorCode_) : errorCode(errorCode_)
{
	errorStr = _strdup(errorStr_);
}

const char* AllegroError::what() const { return errorStr; }
int AllegroError::code() const { return errorCode; }

AllegroError::~AllegroError() {};