#pragma once
#include <exception>
class CurlErrors : public std::exception
{
public:
	CurlErrors(const char* errorCode_);
	virtual const char* what(void) const;
	~CurlErrors();
protected:
	const char* errorCode;
};
