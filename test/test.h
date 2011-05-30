#ifndef __TEST_H__
#define __TEST_H__

#include "logger.h"
#include <string>

static unsigned int numberOfTests = 0;

#define TEST_EQUAL_HEADER(_TYPE0_,_TYPE1_) void test_equal(_TYPE0_ correct, _TYPE1_ candidate, const char *failMessage)
#define TEST_NOTEQUAL_HEADER(_TYPE0_,_TYPE1_) void test_not_equal(_TYPE0_ incorrect, _TYPE1_ candidate, const char *failMessage)

class Test
{
protected:
	const char *name;
	void testcase(bool pass, const char *failMessage);
    TEST_EQUAL_HEADER(double,double);
    TEST_EQUAL_HEADER(void *, void *);
    TEST_NOTEQUAL_HEADER(void *, void *);
    bool passed;
    std::string contextLabel;

public:
	virtual bool run() = 0;
    void context(std::string name);
	bool didPass();
	const char *getName();
	Test();
};

#endif
