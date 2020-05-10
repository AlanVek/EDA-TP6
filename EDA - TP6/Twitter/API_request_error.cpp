#include <iostream>
#include "API_request_error.h"

API_request_error::API_request_error(const char* errorCode_) { errorCode = _strdup(errorCode_); }

const char* API_request_error::what() const { return errorCode; }

API_request_error::~API_request_error() {};