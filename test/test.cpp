#include "test.h"
#include "logger.h"
#include <set>
#include <iostream>

// Include additional test classes here
#include "test_graph.cpp"
#include "test_dendrogram.cpp"
#include "test_consensus.cpp"

#define TEST_EQUAL_METHOD(_TYPE0_,_TYPE1_) void Test::test_equal(_TYPE0_ correct, _TYPE1_ candidate, const char *failMessage)\
{\
    bool result = correct == candidate;\
    \
    testcase(result, failMessage);\
    \
    if (!result) {\
        std::cout << "\t"<<correct<<" expected, got "<<candidate<< std::endl;\
    }\
}

#define TEST_NOTEQUAL_METHOD(_TYPE0_,_TYPE1_) void Test::test_not_equal(_TYPE0_ incorrect, _TYPE1_ candidate, const char *failMessage)\
{\
bool result = incorrect != candidate;\
\
testcase(result, failMessage);\
\
if (!result) {\
std::cout << "\t"<<candidate<<" should not be "<<incorrect<< std::endl;\
}\
}

int main(void)
{
	std::set<Test *> tests;
	
	tests.insert( new GraphTest() );
    tests.insert( new DendrogramTest() );
    tests.insert( new ConsensusTest() );
	
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

TEST_EQUAL_METHOD(double,double);
TEST_EQUAL_METHOD(void *,void *);

TEST_NOTEQUAL_METHOD(void *, void *);

void Test::context(std::string name)
{
    contextLabel = name;
}

bool Test::didPass()
{
	return this->passed;
}
