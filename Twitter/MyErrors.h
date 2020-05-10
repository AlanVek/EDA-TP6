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