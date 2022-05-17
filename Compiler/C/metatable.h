#ifndef METATABLE_H
#define METATABLE_H

#include "var.h"

Var MetatableBase       = {0x04, 0x00, 0x00};

Var MetatableNull       = {0x04, 0x00, 0x00};
Var MetatableNumber     = {0x04, 0x00, 0x00};
Var MetatableString     = {0x04, 0x00, 0x00};
Var MetatableFunction   = {0x04, 0x00, 0x00};
Var MetatableList       = {0x04, 0x00, 0x00};
Var MetatableWith       = {0x04, 0x00, 0x00};

Var* VarGetMeta(Var* var, char* key);

#endif