#include <stdio.h>

#ifndef DRAW_C
#define DRAW_C

#include "lib.h"

#include "../funky3.h"
#include "../metatable.h"
#include "../hashmap.h"

EM_JS(void, canvasClear, (), {
    Module.ctx.clearRect(0,0,512,512);
});

EM_JS(void, canvasBeginPath, (), {
    Module.ctx.beginPath();
});

EM_JS(void, canvasMoveTo, (double x, double y), {
   Module.ctx.moveTo(x, y);
});

EM_JS(void, canvasLineTo, (double x, double y), {
    Module.ctx.lineTo(x, y);
});

EM_JS(void, canvasStroke, (), {
    Module.ctx.stroke();
});

EM_JS(void, canvasFill, (), {
    Module.ctx.fill();
});

EM_JS(void, canvasStrokeStyle, (char* style), {
    Module.ctx.strokeStyle = UTF8ToString(style);
});

EM_JS(void, canvasFillStyle, (char* style), {
    Module.ctx.fillStyle = UTF8ToString(style);
});

Var* DrawClear(Var* scope, Var* args){
    canvasClear();
}

Var* DrawBeginPath(Var* scope, Var* args){
    canvasBeginPath();
}

Var* DrawMoveTo(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasMoveTo(dx, dy);
}

Var* DrawLineTo(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasLineTo(dx, dy);
}

Var* DrawStroke(Var* scope, Var* args){
    canvasStroke();
};

Var* DrawFill(Var* scope, Var* args){
    canvasFill();
}

Var* DrawStrokeStyle(Var* scope, Var* args){
    Var* style = VarAsString(ArgVarGet(args, 0, "style"));
    if(style -> type != VAR_STRING){
        return &NIL;
    }
    canvasStrokeStyle(style -> value);
}

Var* DrawFillStyle(Var* scope, Var* args){
    Var* style = VarAsString(ArgVarGet(args, 0, "style"));
    if(style -> type != VAR_STRING){
        return &NIL;
    }
    canvasFillStyle(style -> value);
}

Var* drawTable;

EMSCRIPTEN_KEEPALIVE
int DrawHook(){
    Var* frameMethod = VarGet(drawTable, VarNewString("frame"));
    if(ISUNDEFINED(frameMethod))
        return 1;
    Var* args = VarNewList();
    // Later: Delta Time?
    return VarTruthy(VarFunctionCall(frameMethod, args));
}

void PopulateDrawLib(Var* draw){
    drawTable = draw;
    CONSTANT(beginPath, VarNewFunction(DrawBeginPath));
    CONSTANT(clear, VarNewFunction(DrawClear));
    CONSTANT(moveTo, VarNewFunction(DrawMoveTo));
    CONSTANT(lineTo, VarNewFunction(DrawLineTo));
    CONSTANT(stroke, VarNewFunction(DrawStroke));
    CONSTANT(fill, VarNewFunction(DrawFill));
    CONSTANT(strokeStyle, VarNewFunction(DrawStrokeStyle));
    CONSTANT(fillStyle, VarNewFunction(DrawFillStyle));
}

#endif