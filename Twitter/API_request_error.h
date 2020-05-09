#pragma once
#include <exception>
class API_request_error : public std::exception
{
public:
	API_request_error(const char* errorCode_);
	virtual const char* what(void) const;
	~API_request_error();
protected:
	const char* errorCode;
};
