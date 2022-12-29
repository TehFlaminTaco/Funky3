#ifndef FUNKY3_H
#define FUNKY3_H

//#define FNK_DEBUG


#include "var.h"
#include "tgc.h"

#define VAR_NULL 0x00
#define VAR_NUMBER 0x01
#define VAR_STRING 0x02
#define VAR_FUNCTION 0x03
#define VAR_LIST 0x04

#ifdef FNK_DEBUG
#define DebugPrint(...) printf(__VA_ARGS__)
#else
#define DebugPrint(...)
#endif

static tgc_t gc = {NULL};

Var UNDEFINED   = {VAR_NULL, 0, NULL};
Var NIL         = {VAR_NULL, 1, NULL};


#endif