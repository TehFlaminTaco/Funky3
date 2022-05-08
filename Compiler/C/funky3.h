#ifndef FUNKY3_H
#define FUNKY3_H

//#define DEBUG

#include "var.h"
#include "gc.h"

#define VAR_NULL 0x00
#define VAR_NUMBER 0x01
#define VAR_STRING 0x02
#define VAR_FUNCTION 0x03
#define VAR_LIST 0x04

#ifdef DEBUG
#define DebugPrint(...) printf(__VA_ARGS__)
#else
#define DebugPrint(...)
#endif

GarbageCollector gc = {NULL};

Var UNDEFINED   = {VAR_NULL, 0, NULL, NULL, 0};
Var NIL         = {VAR_NULL, 1, NULL, NULL, 0};


#endif