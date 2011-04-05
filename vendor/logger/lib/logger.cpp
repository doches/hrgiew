/*
 *  logger.cpp
 *  GLSkel
 *
 *  Created by Trevor Fountain on 24/2/2011.
 *  Copyright 2011 Expat Games. All rights reserved.
 *
 */

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_TEMPLATE_1(_TYPE_) \
void Log::message(const char *system, const char *message, _TYPE_ a0, LogLevel level) {\
char logString[80];\
sprintf(logString,message,a0);\
Log::message(system,logString,level);\
}

#define LOG_TEMPLATE_2(_TYPE0_, _TYPE1_) \
void Log::message(const char *system, const char *message, _TYPE0_ a0, _TYPE1_ a1, LogLevel level) {\
char logString[80];\
sprintf(logString,message,a0,a1);\
Log::message(system,logString,level);\
}

#define LOG_TEMPLATE_3(_TYPE0_, _TYPE1_, _TYPE2_) \
void Log::message(const char *system, const char *message, _TYPE0_ a0, _TYPE1_ a1, _TYPE2_ a2, LogLevel level) {\
char logString[80];\
sprintf(logString,message,a0,a1,a2);\
Log::message(system,logString,level);\
}

void Log::message(const char *system, const char *message, LogLevel level)
{
    fprintf(stdout,"[%s:%d] %s\n",system,level,message);
}

LOG_TEMPLATE_1(const char *)
LOG_TEMPLATE_1(int)
LOG_TEMPLATE_1(double)
LOG_TEMPLATE_1(char)
LOG_TEMPLATE_2(int,int)
LOG_TEMPLATE_2(const char *, const char *)
LOG_TEMPLATE_2(double,double)
LOG_TEMPLATE_3(int,int,int)
LOG_TEMPLATE_3(double,double,double)
LOG_TEMPLATE_3(const char *, const char *, int)

void Log::die(const char *system, const char *message)
{
    Log::message(system,message,DIE);
    exit(1);
}

void Log::error(const char *system, const char *message)
{
    Log::message(system,message,ERROR);
}

void Log::warn(const char *system, const char *message)
{
    Log::message(system,message,WARN);
}

void Log::debug(const char *system, const char *message)
{
    Log::message(system,message,DEBUG);
}

#undef LOG_TEMPLATE_1
#undef LOG_TEMPLATE_2

