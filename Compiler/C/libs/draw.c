#include <stdio.h>
#include <math.h>

#ifndef DRAW_C
#define DRAW_C

#include "lib.h"

#include "../funky3.h"
#include "../metatable.h"
#include "../hashmap.h"

#ifdef EMSCRIPTEN
#define LIBNAME canvas
#include "canvas.c"

Var* DrawClear(Var* scope, Var* args){
    canvasClear();
    return &NIL;
}

Var* DrawPop(Var* scope, Var* args){
    canvasRestore();
    return &NIL;
}

Var* DrawPush(Var* scope, Var* args){
    canvasSave();
    Var* oVar = VarNewList();
    VarRawSet(oVar, VarNewConstString("dispose"), VarNewFunction(DrawPop));
    return oVar;
}


Var* DrawScale(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        y = x;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasScale(dx, dy);
    return &NIL;
}

Var* DrawRotate(Var* scope, Var* args){
    Var* angle = ArgVarGet(args, 0, "angle");
    if(angle->type != VAR_NUMBER){
        return &NIL;
    }
    double dangle;
    memcpy(&dangle, &angle->value, sizeof(double));
    canvasRotate(dangle);
    return &NIL;
}

Var* DrawTranslate(Var* scope, Var* args){
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
    canvasTranslate(dx, dy);
    return &NIL;
}

Var* DrawPushViewport(Var* scope, Var* args){
    double dx;
    double dy;
    double dw;
    double dh;
    Var* x = ArgVarGet(args, 0, "left");
    if(x -> type != VAR_NUMBER) return &NIL;
    Var* y = ArgVarGet(args, 1, "top");
    if(y -> type != VAR_NUMBER) return &NIL;
    Var* w = ArgVarGet(args, 2, "right");
    if(w -> type != VAR_NUMBER) return &NIL;
    Var* h = ArgVarGet(args, 3, "bottom");
    if(h -> type != VAR_NUMBER) return &NIL;
    memcpy(&dx, &x->value, sizeof(double));
    memcpy(&dy, &y->value, sizeof(double));
    memcpy(&dw, &w->value, sizeof(double));
    memcpy(&dh, &h->value, sizeof(double));
    canvasSave();
    canvasScale(512.0/(dw-dx), 512.0/(dh-dy));
    canvasTranslate(-dx, -dy);
    
    Var* oVar = VarNewList();
    VarRawSet(oVar, VarNewConstString("dispose"), VarNewFunction(DrawPop));
    ArgVarSet(oVar, 0, "left", x);
    ArgVarSet(oVar, 1, "top", y);
    ArgVarSet(oVar, 2, "right", w);
    ArgVarSet(oVar, 3, "bottom", h);
    return oVar;
}

Var* DrawViewportToScreen(Var* scope, Var* args){
    double dx;
    double dy;
    double dl;
    double dt;
    double dr;
    double db;
    Var* x = ArgVarGet(args, 0, "x");
    if(x -> type != VAR_NUMBER) return &NIL;
    Var* y = ArgVarGet(args, 1, "y");
    if(y -> type != VAR_NUMBER) return &NIL;
    Var* l = ArgVarGet(args, 2, "left");
    if(l -> type != VAR_NUMBER) return &NIL;
    Var* t = ArgVarGet(args, 3, "top");
    if(t -> type != VAR_NUMBER) return &NIL;
    Var* r = ArgVarGet(args, 4, "right");
    if(r -> type != VAR_NUMBER) return &NIL;
    Var* b = ArgVarGet(args, 5, "bottom");
    if(b -> type != VAR_NUMBER) return &NIL;
    memcpy(&dx, &x->value, sizeof(double));
    memcpy(&dy, &y->value, sizeof(double));
    memcpy(&dl, &l->value, sizeof(double));
    memcpy(&dt, &t->value, sizeof(double));
    memcpy(&dr, &r->value, sizeof(double));
    memcpy(&db, &b->value, sizeof(double));

    double ox = dx - dl;
    double oy = dy - dt;
    ox *= 512.0/(dr-dl);
    oy *= 512.0/(db-dt);
    Var* vox = VarNewNumber(ox);
    Var* voy = VarNewNumber(oy);

    Var* ov = VarNewList();
    ArgVarSet(ov, 0, "x", vox);
    ArgVarSet(ov, 1, "y", voy);
    return ov;
}

Var* DrawScreenToViewport(Var* scope, Var* args){
    double dx;
    double dy;
    double dl;
    double dt;
    double dr;
    double db;
    Var* x = ArgVarGet(args, 0, "x");
    if(x -> type != VAR_NUMBER) return &NIL;
    Var* y = ArgVarGet(args, 1, "y");
    if(y -> type != VAR_NUMBER) return &NIL;
    Var* l = ArgVarGet(args, 2, "left");
    if(l -> type != VAR_NUMBER) return &NIL;
    Var* t = ArgVarGet(args, 3, "top");
    if(t -> type != VAR_NUMBER) return &NIL;
    Var* r = ArgVarGet(args, 4, "right");
    if(r -> type != VAR_NUMBER) return &NIL;
    Var* b = ArgVarGet(args, 5, "bottom");
    if(b -> type != VAR_NUMBER) return &NIL;
    memcpy(&dx, &x->value, sizeof(double));
    memcpy(&dy, &y->value, sizeof(double));
    memcpy(&dl, &l->value, sizeof(double));
    memcpy(&dt, &t->value, sizeof(double));
    memcpy(&dr, &r->value, sizeof(double));
    memcpy(&db, &b->value, sizeof(double));

    Var* vox = VarNewNumber(dx / (512.0/(dr-dl)) + dl);
    Var* voy = VarNewNumber(dy / (512.0/(db-dt)) + dt);

    Var* ov = VarNewList();
    ArgVarSet(ov, 0, "x", vox);
    ArgVarSet(ov, 1, "y", voy);
    return ov;
}

int _ParseCamera(Var* cam, int argOffset, double* dX, double* dY, double* dWidth, double* dHeight, double* dAngle){
    Var* vPosition = ArgVarGet(cam, argOffset, "position");
    if(vPosition -> type != VAR_LIST){
        return 0;
    }
    Var* vX = ArgVarGet(vPosition, 0, "x");
    if(vX -> type != VAR_NUMBER){
        return 0;
    }
    memcpy(dX, &vX->value, sizeof(double));
    Var* vY = ArgVarGet(vPosition, 0, "y");
    if(vY -> type != VAR_NUMBER){
        return 0;
    }
    memcpy(dY, &vY->value, sizeof(double));
    *dWidth = 512;
    *dHeight = 512;
    Var* vSize = ArgVarGet(cam, argOffset+1, "size");
    if(vSize -> type == VAR_NUMBER){
        memcpy(dWidth, &vSize -> value, sizeof(double));
        *dHeight = *dWidth;
    }else if(vSize -> type == VAR_LIST){
        Var* vWidth = ArgVarGet(vSize, 0, "w");
        Var* vHeight = ArgVarGet(vSize, 1, "h");
        Var* vAspectRatio = VarRawGet(vSize, VarNewConstString("aspectRatio"));
        double dAspectRatio = 1.0;
        if(vAspectRatio -> type == VAR_NUMBER){
            memcpy(&dAspectRatio, &vAspectRatio->value, sizeof(double));
        }
        if(vWidth -> type == VAR_NUMBER){
            memcpy(dWidth, &vWidth->value, sizeof(double));
        }
        if(vHeight -> type == VAR_NUMBER){
            memcpy(dHeight, &vHeight->value, sizeof(double));
        }
        if(vWidth -> type != VAR_NUMBER){
            *dWidth = *dHeight * dAspectRatio;
        }
        if(vHeight -> type == VAR_NUMBER){
            *dHeight = *dWidth / dAspectRatio;
        }
    }
    Var* vAngle = ArgVarGet(cam, argOffset+2, "angle");
    if(vAngle -> type == VAR_NUMBER){
        memcpy(dAngle, &vAngle -> value, sizeof(double));
    }
    return 1;
}

Var* DrawPushCamera(Var* scope, Var* args){
    double dX;
    double dY;
    double dWidth;
    double dHeight;
    double dAngle;
    if(!_ParseCamera(args, 0, &dX, &dY, &dWidth, &dHeight, &dAngle)){
        return &NIL;
    }

    canvasSave();
    canvasTranslate(256.0, 256.0);
    canvasRotate(-dAngle);
    canvasScale(512.0/dWidth, 512.0/dHeight);
    canvasTranslate(-dX, -dY);

    Var* oVar = VarNewList();
    VarRawSet(oVar, VarNewConstString("dispose"), VarNewFunction(DrawPop));
    ArgVarSet(oVar, 0, "position", ArgVarGet(args, 0, "position"));
    ArgVarSet(oVar, 1, "size", ArgVarGet(args, 1, "size"));
    ArgVarSet(oVar, 2, "angle", ArgVarGet(args, 2, "angle"));
    return oVar;
}

Var* DrawCameraToScreen(Var* scope, Var* args){
    double dTx;
    double dTy;

    Var* vTx = ArgVarGet(args, 0, "x");
    if(vTx -> type != VAR_NUMBER){
        return &NIL;
    }
    Var* vTy = ArgVarGet(args, 1, "y");
    if(vTy -> type != VAR_NUMBER){
        return &NIL;
    }
    
    double dX;
    double dY;
    double dWidth;
    double dHeight;
    double dAngle;
    if(!_ParseCamera(args, 2, &dX, &dY, &dWidth, &dHeight, &dAngle)){
        return &NIL;
    }

    memcpy(&dTx, &vTx->value, sizeof(double));
    memcpy(&dTy, &vTy->value, sizeof(double));

    dTx -= dX;
    dTy -= dY;
    double dTx2 = dTx*cos(-dAngle) - dTy*sin(-dAngle);
    double dTy2 = dTy*cos(-dAngle) + dTx*sin(-dAngle);
    dTx2 *= 512.0 / dWidth;
    dTy2 *= 512.0 / dHeight;
    dTx2 += 256.0;
    dTy2 += 256.0;
    Var* oVar = VarNewList();
    ArgVarSet(oVar, 0, "x", VarNewNumber(dTx2));
    ArgVarSet(oVar, 1, "y", VarNewNumber(dTy2));
    return oVar;
}
Var* DrawScreenToCamera(Var* scope, Var* args){
    double dTx;
    double dTy;

    Var* vTx = ArgVarGet(args, 0, "x");
    if(vTx -> type != VAR_NUMBER){
        return &NIL;
    }
    Var* vTy = ArgVarGet(args, 1, "y");
    if(vTy -> type != VAR_NUMBER){
        return &NIL;
    }
    
    double dX;
    double dY;
    double dWidth;
    double dHeight;
    double dAngle;
    if(!_ParseCamera(args, 2, &dX, &dY, &dWidth, &dHeight, &dAngle)){
        return &NIL;
    }

    memcpy(&dTx, &vTx->value, sizeof(double));
    memcpy(&dTy, &vTy->value, sizeof(double));

    dTx -= 256.0;
    dTy -= 256.0;
    dTx /= 512.0 / dWidth;
    dTy /= 512.0 / dHeight;
    double dTx2 = dTx*cos(dAngle) - dTy*sin(dAngle);
    double dTy2 = dTy*cos(dAngle) + dTx*sin(dAngle);
    dTx2 += dX;
    dTy2 += dY;
    Var* oVar = VarNewList();
    ArgVarSet(oVar, 0, "x", VarNewNumber(dTx2));
    ArgVarSet(oVar, 1, "y", VarNewNumber(dTy2));
    return oVar;
}

double lastRed = 0;
double lastGreen = 0;
double lastBlue = 0;
double lastAlpha = 0;

Var* _RestoreLastColor(Var* scope, Var* args){
    Var* vRed = VarRawGet(scope, VarNewConstString("r"));
    if(vRed -> type != VAR_NUMBER){
        return &NIL;
    }
    double dr;
    memcpy(&dr, &vRed->value, sizeof(double));
    Var* vGreen = VarRawGet(scope, VarNewConstString("g"));
    if(vGreen -> type != VAR_NUMBER){
        return &NIL;
    }
    double dg;
    memcpy(&dg, &vGreen->value, sizeof(double));
    Var* vBlue = VarRawGet(scope, VarNewConstString("b"));
    if(vBlue -> type != VAR_NUMBER){
        return &NIL;
    }
    double db;
    memcpy(&db, &vBlue->value, sizeof(double));
    Var* vAlpha = VarRawGet(scope, VarNewConstString("a"));
    if(vAlpha -> type != VAR_NUMBER){
        return &NIL;
    }
    double da;
    memcpy(&da, &vAlpha->value, sizeof(double));
    lastRed = dr;
    lastGreen = dg;
    lastBlue = db;
    lastAlpha = da;
    char* styleString;
    if(0 >= asprintf(&styleString, "rgba(%.2f,%.2f,%.2f,%.3f)", dr, dg, db, da/255))
        return &NIL;
    canvasStrokeStyle(styleString);
    canvasFillStyle(styleString);
    free(styleString);
    return &NIL;
}

Var* DrawSetColor(Var* scope, Var* args){
    Var* vRed = ArgVarGet(args, 0, "r");
    if(vRed -> type == VAR_LIST){
        args = vRed;
        vRed = ArgVarGet(args, 0, "r");
    }
    if(vRed -> type != VAR_NUMBER){
        return &NIL;
    }
    double dr;
    memcpy(&dr, &vRed->value, sizeof(double));
    Var* vGreen = ArgVarGet(args, 1, "g");
    if(vGreen -> type != VAR_NUMBER){
        return &NIL;
    }
    double dg;
    memcpy(&dg, &vGreen->value, sizeof(double));
    Var* vBlue = ArgVarGet(args, 2, "b");
    if(vBlue -> type != VAR_NUMBER){
        return &NIL;
    }
    double db;
    memcpy(&db, &vBlue->value, sizeof(double));
    Var* vAlpha = ArgVarGet(args, 3, "a");
    if(vAlpha -> type != VAR_NUMBER){
        vAlpha = VarNewNumber(255.0);
    }
    double da;
    memcpy(&da, &vAlpha->value, sizeof(double));

    double oldRed = lastRed;
    double oldGreen = lastGreen;
    double oldBlue = lastBlue;
    double oldAlpha = lastAlpha;
    
    lastRed = dr;
    lastGreen = dg;
    lastBlue = db;
    lastAlpha = da;
    Var* oldList = VarNewList();
    VarRawSet(oldList, VarNewConstString("r"), VarNewNumber(oldRed));
    VarRawSet(oldList, VarNewConstString("g"), VarNewNumber(oldGreen));
    VarRawSet(oldList, VarNewConstString("b"), VarNewNumber(oldBlue));
    VarRawSet(oldList, VarNewConstString("a"), VarNewNumber(oldAlpha));
    char* styleString;
    if(0 >= asprintf(&styleString, "rgba(%.2f,%.2f,%.2f,%.3f)", dr, dg, db, da/255))
        return &NIL;
    canvasStrokeStyle(styleString);
    canvasFillStyle(styleString);
    free(styleString);
    Var* oVar = VarNewList();
    ArgVarSet(oVar, 0, "r", vRed);
    ArgVarSet(oVar, 1, "g", vGreen);
    ArgVarSet(oVar, 2, "b", vBlue);
    ArgVarSet(oVar, 3, "a", vAlpha);
    Var* dispose = VarNewFunction(_RestoreLastColor);
    ((VarFunction*)(dispose -> value)) -> scope = oldList;
    VarRawSet(oVar, VarNewConstString("dispose"), dispose);
    return oVar;
}

Var* DrawGetColor(Var* scope, Var* args){
    Var* oVar = VarNewList();
    ArgVarSet(oVar, 0, "r", VarNewNumber(lastRed));
    ArgVarSet(oVar, 1, "g", VarNewNumber(lastGreen));
    ArgVarSet(oVar, 2, "b", VarNewNumber(lastBlue));
    ArgVarSet(oVar, 3, "a", VarNewNumber(lastAlpha));
    return oVar;
}

Var* DrawBox(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* w = ArgVarGet(args, 2, "w");
    if(w->type != VAR_NUMBER){
        return &NIL;
    }
    Var* h = ArgVarGet(args, 3, "h");
    if(h->type != VAR_NUMBER){
        h = w;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &w->value, sizeof(double));
    double dh;
    memcpy(&dh, &h->value, sizeof(double));

    canvasFillRect(dx, dy, dw, dh);
    return &NIL;
}

Var* DrawBoxOutline(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* w = ArgVarGet(args, 2, "w");
    if(w->type != VAR_NUMBER){
        return &NIL;
    }
    Var* h = ArgVarGet(args, 3, "h");
    if(h->type != VAR_NUMBER){
        h = w;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &w->value, sizeof(double));
    double dh;
    memcpy(&dh, &h->value, sizeof(double));

    canvasStrokeRect(dx, dy, dw, dh);
    return &NIL;
}

Var* DrawScreenWidth(Var* scope, Var* args){
    return VarNewNumber(512.0);
}
Var* DrawScreenHeight(Var* scope, Var* args){
    return VarNewNumber(512.0);
}
Var* DrawScreenSize(Var* scope, Var* args){
    Var* vec = VarNewList();
    ArgVarSet(vec, 0, "x", VarNewNumber(512.0));
    ArgVarSet(vec, 1, "y", VarNewNumber(512.0));
    return vec;
}

double lastLineWidth = 1.0;
Var* _RestoreLastLineWidth(Var* scope, Var* args){
    Var* w = VarRawGet(scope, VarNewConstString("width"));
    if(w -> type != VAR_NUMBER){
        return &NIL;
    }
    memcpy(&lastLineWidth, &w->value, sizeof(double));
    canvasLineWidth(lastLineWidth);
    return &NIL;
}

Var* DrawSetLineWidth(Var* scope, Var* args){
    Var* w = ArgVarGet(args, 0, "width");
    if(w -> type != VAR_NUMBER){
        return &NIL;
    }
    double oldLineWidth = lastLineWidth;
    Var* oldWidth = VarNewList();
    VarRawSet(oldWidth, VarNewConstString("width"), VarNewNumber(oldLineWidth));
    memcpy(&lastLineWidth, &w->value, sizeof(double));
    canvasLineWidth(lastLineWidth);
    Var* disposable = VarNewList();
    Var* dispose = VarNewFunction(_RestoreLastLineWidth);
    ((VarFunction*)(dispose->value))->scope = oldWidth;
    VarRawSet(disposable, VarNewConstString("dispose"), dispose);
    return disposable;
}

Var* DrawGetLineWidth(Var* scope, Var* args){
    return VarNewNumber(lastLineWidth);
}

Var* DrawCircle(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* r = ArgVarGet(args, 2, "r");
    if(r->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dr;
    memcpy(&dr, &r->value, sizeof(double));

    canvasBeginPath();
    canvasMoveTo(dx, dy);
    canvasArc(dx, dy, dr, 0, 2 * M_PI, 1);
    canvasFill();
    return &NIL;
}

Var* DrawCircleOutline(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* r = ArgVarGet(args, 2, "r");
    if(r->type != VAR_NUMBER){
        return &NIL;
    }

    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dr;
    memcpy(&dr, &r->value, sizeof(double));

    canvasBeginPath();
    canvasMoveTo(dx, dy);
    canvasArc(dx, dy, dr, 0, 2 * M_PI, 1);
    canvasStroke();
    return &NIL;
}

Var* DrawPrint(Var* scope, Var* args){
    Var* text = ArgVarGet(args, 0, "text");
    if(text->type != VAR_STRING){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 1, "x");
    if(x->type != VAR_NUMBER){
        x = VarNewNumber(0.0);
    }
    Var* y = ArgVarGet(args, 2, "y");
    if(y->type != VAR_NUMBER){
        y = VarNewNumber(0.0);
    }
    Var* ox = ArgVarGet(args, 3, "ox");
    if(ox->type != VAR_NUMBER){
        ox = VarNewNumber(0.0);
    }
    Var* oy = ArgVarGet(args, 4, "oy");
    if(oy->type != VAR_NUMBER){
        oy = VarNewNumber(0.0);
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dox;
    memcpy(&dox, &ox->value, sizeof(double));
    double doy;
    memcpy(&doy, &oy->value, sizeof(double));

    canvasTextAlign("left");
    canvasTextBaseline("top");
    if(fabs(dox) > 0.0001){
        dx -= (dox * canvasMeasureTextWidth(text->value));
    }
    if(fabs(doy) > 0.0001){
        dy -= (doy * canvasMeasureTextHeight(text->value));
    }
    
    canvasFillText(text->value, dx, dy);
    return text;
}

Var* DrawPrintOutline(Var* scope, Var* args){
    Var* text = ArgVarGet(args, 0, "text");
    if(text->type != VAR_STRING){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 1, "x");
    if(x->type != VAR_NUMBER){
        x = VarNewNumber(0.0);
    }
    Var* y = ArgVarGet(args, 2, "y");
    if(y->type != VAR_NUMBER){
        y = VarNewNumber(0.0);
    }
    Var* ox = ArgVarGet(args, 3, "ox");
    if(ox->type != VAR_NUMBER){
        ox = VarNewNumber(0.0);
    }
    Var* oy = ArgVarGet(args, 4, "oy");
    if(oy->type != VAR_NUMBER){
        oy = VarNewNumber(0.0);
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dox;
    memcpy(&dox, &ox->value, sizeof(double));
    double doy;
    memcpy(&doy, &oy->value, sizeof(double));

    canvasTextAlign("left");
    canvasTextBaseline("top");
    if(fabs(dox) > 0.0001){
        dx -= (dox * canvasMeasureTextWidth(text->value));
    }
    if(fabs(doy) > 0.0001){
        dy -= (doy * canvasMeasureTextHeight(text->value));
    }
    
    canvasStrokeText(text->value, dx, dy);
    return text;
}

char* lastFont = NULL;
Var* _RestoreLastFont(Var* scope, Var* args){
    Var* font = VarRawGet(scope, VarNewConstString("font"));
    if(font->type != VAR_STRING){
        return &NIL;
    }
    canvasFont(font->value);
    if(lastFont != NULL){
        free(lastFont);
    }
    lastFont = calloc(strlen(font->value), sizeof(char));
    strcpy(lastFont, font->value);
    return &NIL;
}

Var* DrawSetFont(Var* scope, Var* args){
    Var* font = ArgVarGet(args, 0, "font");
    if(font->type != VAR_STRING){
        return &NIL;
    }
    canvasFont(font->value);
    Var* oldFont = VarNewList();
    VarRawSet(oldFont, VarNewConstString("font"), VarNewString(lastFont == NULL ? "10px sans-serif" : lastFont));
    if(lastFont != NULL){
        free(lastFont);
    }
    lastFont = calloc(strlen(font->value), sizeof(char));
    strcpy(lastFont, font->value);
    Var* disposable = VarNewList();
    Var* dispose = VarNewFunction(_RestoreLastFont);
    ((VarFunction*)(dispose->value))->scope = lastFont;
    VarRawSet(disposable, VarNewConstString("dispose"), dispose);
    return disposable;
}

Var *DrawGetFont(Var* scope, Var* args){
    if(lastFont == NULL){
        return VarNewConstString("10px sans-serif");
    }
    return VarNewString(lastFont);
}


#endif

void PopulateDrawLib(Var* draw){
    drawTable = draw;
    #define LIBNAME draw
    CONSTANT(box, VarNewFunction(DrawBox));
    CONSTANT(boxOutline, VarNewFunction(DrawBoxOutline));
    CONSTANT(cameraToScreen, VarNewFunction(DrawCameraToScreen));
    CONSTANT(circle, VarNewFunction(DrawCircle));
    CONSTANT(circleOutline, VarNewFunction(DrawCircleOutline));
    CONSTANT(clear, VarNewFunction(DrawClear));
    CONSTANT(getColor, VarNewFunction(DrawGetColor));
    CONSTANT(getFont, VarNewFunction(DrawGetFont));
    CONSTANT(getLineWidth, VarNewFunction(DrawGetLineWidth));
    CONSTANT(pop, VarNewFunction(DrawPop));
    CONSTANT(print, VarNewFunction(DrawPrint));
    CONSTANT(printOutline, VarNewFunction(DrawPrintOutline));
    CONSTANT(push, VarNewFunction(DrawPush));
    CONSTANT(pushCamera, VarNewFunction(DrawPushCamera));
    CONSTANT(pushViewport, VarNewFunction(DrawPushViewport));
    CONSTANT(rotate, VarNewFunction(DrawRotate));
    CONSTANT(scale, VarNewFunction(DrawScale));
    CONSTANT(screenHeight, VarNewFunction(DrawScreenHeight));
    CONSTANT(screenSize, VarNewFunction(DrawScreenSize));
    CONSTANT(screenToCamera, VarNewFunction(DrawScreenToCamera));
    CONSTANT(screenToViewport, VarNewFunction(DrawScreenToViewport));
    CONSTANT(screenWidth, VarNewFunction(DrawScreenWidth));
    CONSTANT(setColor, VarNewFunction(DrawSetColor));
    CONSTANT(setFont, VarNewFunction(DrawSetFont));
    CONSTANT(setLineWidth, VarNewFunction(DrawSetLineWidth));
    CONSTANT(translate, VarNewFunction(DrawTranslate));
    CONSTANT(viewportToScreen, VarNewFunction(DrawViewportToScreen));

    #ifdef EMSCRIPTEN
    Var* canvas = VarNewList();
    CONSTANT(canvas, canvas);
    PopulateCanvasLib(canvas);
    #endif

}

#endif