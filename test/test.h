#ifndef __TEST_H__
#define __TEST_H__

#include "logger.h"
#include <string>

static unsigned int numberOfTests = 0;

class Test
{
protected:
	const char *name;
	void testcase(bool pass, const char *failMessage);
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
