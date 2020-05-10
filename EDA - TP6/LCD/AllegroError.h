#pragma once
#include <exception>

class AllegroError : public std::exception
{
public:
	AllegroError(const char* errorStr_, const int& errorCode_);
	virtual const char* what(void) const;

	virtual int code(void)const;
	~AllegroError();
protected:
	const char* errorStr;
	int errorCode;
};
