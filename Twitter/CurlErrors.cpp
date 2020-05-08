#include "CurlErrors.h"
#include <iostream>

CurlErrors::CurlErrors(const char* errorCode_) { errorCode = _strdup(errorCode_); }

const char* CurlErrors::what() { return errorCode; }

CurlErrors::~CurlErrors() {};