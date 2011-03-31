#include "test.h"
#include "logger.h"
#include <set>
#include <iostream>

// Include additional test classes here
#include "testGraph.cpp"

int main(void)
{
	std::set<Test *> tests;
	
	tests.insert( new GraphTest() );
	
	std::set<Test *>::iterator iter;
	int passed = 0;
	for(iter = tests.begin(); iter != tests.end(); iter++)
	{
		bool result = (*iter)->run();
		std::cout << (*iter)->getName() << "..." << (result ? "OK" : "FAILED") << std::endl;
		
		if (!result) {
			passed = 1;
		}
	}
	
	std::cout << std::endl << numberOfTests << " assertion" << (numberOfTests > 1 ? "s" : "") << " in " << tests.size() << " test" << (tests.size() > 1 ? "s" : "") << std::endl;
	
	return passed;
}

const char *Test::getName()
{
	return this->name;
}

Test::Test()
{
	this->name = "Test";
	this->passed = true;
}

void Test::testcase(bool pass, const char *failMessage)
{
	if (!pass) {
		Log::message(this->getName(),failMessage,Log::WARN);
		this->passed = false;
	}
	
	numberOfTests++;
}

bool Test::didPass()
{
	return this->passed;
}
