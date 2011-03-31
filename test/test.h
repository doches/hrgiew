#ifndef __TEST_H__
#define __TEST_H__

#include "logger.h"

static unsigned int numberOfTests = 0;

class Test
{
protected:
	const char *name;
	void testcase(bool pass, const char *failMessage);
	bool passed;

public:
	virtual bool run() = 0;
	bool didPass();
	const char *getName();
	Test();
};

#endif
