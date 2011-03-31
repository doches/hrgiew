/*
 *  logger.h
 *  GLSkel
 *
 *  Created by Trevor Fountain on 24/2/2011.
 *  Copyright 2011 Expat Games. All rights reserved.
 *
 */

#ifndef __GLSKEL_LOGGER_H__
#define __GLSKEL_LOGGER_H__

#include <stdarg.h>
#include <stdio.h>

#define LOG_HTEMPLATE_1(_TYPE0_) static void message(const char *system, const char *message, _TYPE0_ a0, LogLevel level);
#define LOG_HTEMPLATE_2(_TYPE0_,_TYPE1_) static void message(const char *system, const char *message, _TYPE0_ a0, _TYPE1_ a1, LogLevel level);
#define LOG_HTEMPLATE_3(_TYPE0_,_TYPE1_,_TYPE2_) static void message(const char *system, const char *message, _TYPE0_ a0, _TYPE1_ a1, _TYPE2_ a2, LogLevel level);

typedef int LogLevel;

class Log
{
public:
    static void message(const char *system, const char *message, LogLevel level);
    
    static const LogLevel DEBUG=0;
    static const LogLevel WARN=1;
    static const LogLevel ERROR=2;
    static const LogLevel DIE=3;

    LOG_HTEMPLATE_1(const char *)
    LOG_HTEMPLATE_1(int)
    LOG_HTEMPLATE_1(double)
    LOG_HTEMPLATE_1(char)
    LOG_HTEMPLATE_2(int,int)
    LOG_HTEMPLATE_2(const char *, const char *)
    LOG_HTEMPLATE_2(double,double)
    LOG_HTEMPLATE_3(int,int,int)
    LOG_HTEMPLATE_3(double,double,double)

    static void die(const char *system, const char *message);
    static void error(const char *system, const char *message);
    static void warn(const char *system, const char *message);
    static void debug(const char *system, const char *message);
};

#undef LOG_HTEMPLATE_1
#undef LOG_HTEMPLATE_2

#define LOG(_SYSTEM_,_MESSAGE_) Log::message(_SYSTEM_,_MESSAGE_,Log::DEBUG)

#endif

