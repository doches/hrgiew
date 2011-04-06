#include "test.h"
#include "logger.h"
#include <set>
#include <iostream>

// Include additional test classes here
#include "test_graph.cpp"
#include "test_dendrogram.cpp"

int main(void)
{
	std::set<Test *> tests;
	
	tests.insert( new GraphTest() );
    tests.insert( new DendrogramTest() );
	
	std::set<Test *>::iterator iter;
	int passed = 0;
	for(iter = tests.begin(); iter != tests.end(); iter++)
	{
		std::cout << (*iter)->getName();
		bool result = (*iter)->run();
        std::cout << (result ? " OK" : " FAILED") << std::endl;
		
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
        std::cout << std::endl << "in \""<<contextLabel << "\":\n\t";
		Log::message(this->getName(),failMessage,Log::WARN);
		this->passed = false;
	} else {
        std::cout << ".";
    }
	
	numberOfTests++;
}

void Test::context(std::string name)
{
    contextLabel = name;
}

bool Test::didPass()
{
	return this->passed;
}
