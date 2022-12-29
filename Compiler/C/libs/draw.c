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

EM_JS(void, canvasStrokeText, (char* text, double x, double y), {
    Module.ctx.strokeText(UTF8ToString(text), x, y);
});

EM_JS(void, canvasFillText, (char* text, double x, double y), {
    Module.ctx.fillText(UTF8ToString(text), x, y);
});

EM_JS(void, canvasFont, (char* font), {
    Module.ctx.font = UTF8ToString(font);
});

EM_JS(void, canvasTextAlign, (char* align), {
    Module.ctx.textAlign = UTF8ToString(align);
});

EM_JS(void, canvasTextBaseline, (char* baseline), {
    Module.ctx.textBaseline = UTF8ToString(baseline);
});

EM_JS(void, canvasFillRect, (double x, double y, double w, double h), {
    Module.ctx.fillRect(x, y, w, h);
});

EM_JS(void, canvasStrokeRect, (double x, double y, double w, double h), {
    Module.ctx.strokeRect(x, y, w, h);
});

EM_JS(void, canvasArc, (double x, double y, double r, double s, double e, int cw), {
    Module.ctx.arc(x, y, r, s, e, cw);
});

EM_JS(void, canvasArcTo, (double x1, double y1, double x2, double y2, double r), {
    Module.ctx.arcTo(x1, y1, x2, y2, r);
});

EM_JS(void, canvasBezierCurveTo, (double cp1x, double cp1y, double cp2x, double cp2y, double x, double y), {
    Module.ctx.bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
});

EM_JS(void, canvasQuadraticCurveTo, (double cpx, double cpy, double x, double y), {
    Module.ctx.quadraticCurveTo(cpx, cpy, x, y);
});

EM_JS(void, canvasRect, (double x, double y, double w, double h), {
    Module.ctx.rect(x, y, w, h);
});

EM_JS(void, canvasClosePath, (), {
    Module.ctx.closePath();
});

EM_JS(void, canvasClip, (), {
    Module.ctx.clip();
});

EM_JS(int, canvasIsPointInPath, (double x, double y), {
    return Module.ctx.isPointInPath(x, y) ? 1 : 0;
});

EM_JS(int, canvasIsPointInStroke, (double x, double y), {
    return Module.ctx.isPointInStroke(x, y) ? 1 : 0;
});

EM_JS(void, canvasScale, (double x, double y), {
    Module.ctx.scale(x, y);
});

EM_JS(void, canvasRotate, (double angle), {
    Module.ctx.rotate(angle);
});

EM_JS(void, canvasTranslate, (double x, double y), {
    Module.ctx.translate(x, y);
});

EM_JS(void, canvasTransform, (double a, double b, double c, double d, double e, double f), {
    Module.ctx.transform(a, b, c, d, e, f);
});

EM_JS(void, canvasSetTransform, (double a, double b, double c, double d, double e, double f), {
    Module.ctx.setTransform(a, b, c, d, e, f);
});

EM_JS(void, canvasResetTransform, (), {
    Module.ctx.resetTransform();
});

EM_JS(void, canvasDrawImage, (char* img, double x, double y), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.drawImage(image, x, y);
});

EM_JS(void, canvasDrawImage2, (char* img, double x, double y, double w, double h), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.drawImage(image, x, y, w, h);
});

EM_JS(void, canvasDrawImage3, (char* img, double sx, double sy, double sw, double sh, double dx, double dy, double dw, double dh), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh);
});

EM_JS(void, canvasCreateImageData, (double w, double h), {
    Module.ctx.createImageData(w, h);
});

EM_JS(void, canvasCreateImageData2, (char* img), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.createImageData(image);
});

EM_JS(void, canvasPutImageData, (char* img, double x, double y), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.putImageData(image, x, y);
});

EM_JS(void, canvasPutImageData2, (char* img, double x, double y, double dx, double dy, double dw, double dh), {
    var image = new Image();
    image.src = UTF8ToString(img);
    Module.ctx.putImageData(image, x, y, dx, dy, dw, dh);
});

EM_JS(void, canvasGetImageData, (double x, double y, double w, double h), {
    Module.ctx.getImageData(x, y, w, h);
});

EM_JS(void, canvasSave, (), {
    Module.ctx.save();
});

EM_JS(void, canvasRestore, (), {
    Module.ctx.restore();
});

EM_JS(void, canvasReset, (), {
    Module.ctx.reset();
});

EM_JS(void, canvasRoundRect, (double x, double y, double w, double h, double r), {
    Module.ctx.roundRect(x, y, w, h, r);
});

EM_JS(void, canvasSetLineDash, (char* dash), {
    Module.ctx.setLineDash(dash);
});

EM_JS(void, canvasGetLineDash, (), {
    Module.ctx.getLineDash();
});

EM_JS(double, canvasMeasureTextWidth, (char* text), {
    return Module.ctx.measureText(UTF8ToString(text)).width;
});

EM_JS(double, canvasMeasureTextHeight, (char* text), {
    return Module.ctx.measureText(UTF8ToString(text)).height;
});

EM_JS(void, canvasLineWidth, (double width), {
    Module.ctx.lineWidth = width;
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
    canvasStrokeStyle((char*) style -> value);
}

Var* DrawFillStyle(Var* scope, Var* args){
    Var* style = VarAsString(ArgVarGet(args, 0, "style"));
    if(style -> type != VAR_STRING){
        return &NIL;
    }
    canvasFillStyle((char*) style -> value);
}

Var* DrawStrokeText(Var* scope, Var* args){
    Var* text = VarAsString(ArgVarGet(args, 0, "text"));
    if(text -> type != VAR_STRING){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 1, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 2, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasStrokeText((char*) text -> value, dx, dy);
}

Var* DrawFillText(Var* scope, Var* args){
    Var* text = VarAsString(ArgVarGet(args, 0, "text"));
    if(text -> type != VAR_STRING){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 1, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 2, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasFillText((char*) text -> value, dx, dy);
}

Var* DrawFont(Var* scope, Var* args){
    Var* font = VarAsString(ArgVarGet(args, 0, "font"));
    if(font -> type != VAR_STRING){
        return &NIL;
    }
    canvasFont((char*) font -> value);
}

Var* DrawTextAlign(Var* scope, Var* args){
    Var* align = VarAsString(ArgVarGet(args, 0, "align"));
    if(align -> type != VAR_STRING){
        return &NIL;
    }
    canvasTextAlign((char*) align -> value);
}

Var* DrawTextBaseline(Var* scope, Var* args){
    Var* baseline = VarAsString(ArgVarGet(args, 0, "baseline"));
    if(baseline -> type != VAR_STRING){
        return &NIL;
    }
    canvasTextBaseline((char*) baseline -> value);
}

Var* DrawFillRect(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* width = ArgVarGet(args, 2, "width");
    if(width->type != VAR_NUMBER){
        return &NIL;
    }
    Var* height = ArgVarGet(args, 3, "height");
    if(height->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &width->value, sizeof(double));
    double dh;
    memcpy(&dh, &height->value, sizeof(double));
    canvasFillRect(dx, dy, dw, dh);
}

Var* DrawStrokeRect(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* width = ArgVarGet(args, 2, "width");
    if(width->type != VAR_NUMBER){
        return &NIL;
    }
    Var* height = ArgVarGet(args, 3, "height");
    if(height->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &width->value, sizeof(double));
    double dh;
    memcpy(&dh, &height->value, sizeof(double));
    canvasStrokeRect(dx, dy, dw, dh);
}

Var* DrawArc(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* radius = ArgVarGet(args, 2, "radius");
    if(radius->type != VAR_NUMBER){
        return &NIL;
    }
    Var* startAngle = ArgVarGet(args, 3, "startAngle");
    if(startAngle->type != VAR_NUMBER){
        startAngle = VarNewNumber(0);
    }
    Var* endAngle = ArgVarGet(args, 4, "endAngle");
    if(endAngle->type != VAR_NUMBER){
        endAngle = VarNewNumber(2 * M_PI);
    }
    Var* anticlockwise = ArgVarGet(args, 5, "anticlockwise");
    if(anticlockwise->type != VAR_NUMBER){
        anticlockwise = VarNewNumber(0);
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dr;
    memcpy(&dr, &radius->value, sizeof(double));
    double ds;
    memcpy(&ds, &startAngle->value, sizeof(double));
    double de;
    memcpy(&de, &endAngle->value, sizeof(double));
    canvasArc(dx, dy, dr, ds, de, VarTruthy(anticlockwise));
}

Var* DrawArcTo(Var* scope, Var* args){
    Var* x1 = ArgVarGet(args, 0, "x1");
    if(x1->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y1 = ArgVarGet(args, 1, "y1");
    if(y1->type != VAR_NUMBER){
        return &NIL;
    }
    Var* x2 = ArgVarGet(args, 2, "x2");
    if(x2->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y2 = ArgVarGet(args, 3, "y2");
    if(y2->type != VAR_NUMBER){
        return &NIL;
    }
    Var* radius = ArgVarGet(args, 4, "radius");
    if(radius->type != VAR_NUMBER){
        return &NIL;
    }
    double dx1;
    memcpy(&dx1, &x1->value, sizeof(double));
    double dy1;
    memcpy(&dy1, &y1->value, sizeof(double));
    double dx2;
    memcpy(&dx2, &x2->value, sizeof(double));
    double dy2;
    memcpy(&dy2, &y2->value, sizeof(double));
    double dr;
    memcpy(&dr, &radius->value, sizeof(double));
    canvasArcTo(dx1, dy1, dx2, dy2, dr);
}

Var* DrawBezierCurveTo(Var* scope, Var* args){
    Var* cp1x = ArgVarGet(args, 0, "cp1x");
    if(cp1x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* cp1y = ArgVarGet(args, 1, "cp1y");
    if(cp1y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* cp2x = ArgVarGet(args, 2, "cp2x");
    if(cp2x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* cp2y = ArgVarGet(args, 3, "cp2y");
    if(cp2y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 4, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 5, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dcp1x;
    memcpy(&dcp1x, &cp1x->value, sizeof(double));
    double dcp1y;
    memcpy(&dcp1y, &cp1y->value, sizeof(double));
    double dcp2x;
    memcpy(&dcp2x, &cp2x->value, sizeof(double));
    double dcp2y;
    memcpy(&dcp2y, &cp2y->value, sizeof(double));
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasBezierCurveTo(dcp1x, dcp1y, dcp2x, dcp2y, dx, dy);
}

Var* DrawQuadraticCurveTo(Var* scope, Var* args){
    Var* cpx = ArgVarGet(args, 0, "cpx");
    if(cpx->type != VAR_NUMBER){
        return &NIL;
    }
    Var* cpy = ArgVarGet(args, 1, "cpy");
    if(cpy->type != VAR_NUMBER){
        return &NIL;
    }
    Var* x = ArgVarGet(args, 2, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 3, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    double dcpx;
    memcpy(&dcpx, &cpx->value, sizeof(double));
    double dcpy;
    memcpy(&dcpy, &cpy->value, sizeof(double));
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    canvasQuadraticCurveTo(dcpx, dcpy, dx, dy);
}

Var* DrawRect(Var* scope, Var* args){
    Var* x = ArgVarGet(args, 0, "x");
    if(x->type != VAR_NUMBER){
        return &NIL;
    }
    Var* y = ArgVarGet(args, 1, "y");
    if(y->type != VAR_NUMBER){
        return &NIL;
    }
    Var* width = ArgVarGet(args, 2, "width");
    if(width->type != VAR_NUMBER){
        return &NIL;
    }
    Var* height = ArgVarGet(args, 3, "height");
    if(height->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &width->value, sizeof(double));
    double dh;
    memcpy(&dh, &height->value, sizeof(double));
    canvasRect(dx, dy, dw, dh);
}

Var* DrawClosePath(Var* scope, Var* args){
    canvasClosePath();
}

Var* DrawClip(Var* scope, Var* args){
    canvasClip();
}

Var* DrawIsPointInPath(Var* scope, Var* args){
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
    return VarNewNumber(canvasIsPointInPath(dx, dy));
}

Var* DrawIsPointInStroke(Var* scope, Var* args){
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
    return VarNewNumber(canvasIsPointInStroke(dx, dy));
}

Var* DrawScale(Var* scope, Var* args){
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
    canvasScale(dx, dy);
}

Var* DrawRotate(Var* scope, Var* args){
    Var* angle = ArgVarGet(args, 0, "angle");
    if(angle->type != VAR_NUMBER){
        return &NIL;
    }
    double dangle;
    memcpy(&dangle, &angle->value, sizeof(double));
    canvasRotate(dangle);
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
}

Var* DrawTransform(Var* scope, Var* args){
    Var* a = ArgVarGet(args, 0, "a");
    if(a->type != VAR_NUMBER){
        return &NIL;
    }
    Var* b = ArgVarGet(args, 1, "b");
    if(b->type != VAR_NUMBER){
        return &NIL;
    }
    Var* c = ArgVarGet(args, 2, "c");
    if(c->type != VAR_NUMBER){
        return &NIL;
    }
    Var* d = ArgVarGet(args, 3, "d");
    if(d->type != VAR_NUMBER){
        return &NIL;
    }
    Var* e = ArgVarGet(args, 4, "e");
    if(e->type != VAR_NUMBER){
        return &NIL;
    }
    Var* f = ArgVarGet(args, 5, "f");
    if(f->type != VAR_NUMBER){
        return &NIL;
    }
    double da;
    memcpy(&da, &a->value, sizeof(double));
    double db;
    memcpy(&db, &b->value, sizeof(double));
    double dc;
    memcpy(&dc, &c->value, sizeof(double));
    double dd;
    memcpy(&dd, &d->value, sizeof(double));
    double de;
    memcpy(&de, &e->value, sizeof(double));
    double df;
    memcpy(&df, &f->value, sizeof(double));
    canvasTransform(da, db, dc, dd, de, df);
}

Var* DrawSetTransform(Var* scope, Var* args){
    Var* a = ArgVarGet(args, 0, "a");
    if(a->type != VAR_NUMBER){
        return &NIL;
    }
    Var* b = ArgVarGet(args, 1, "b");
    if(b->type != VAR_NUMBER){
        return &NIL;
    }
    Var* c = ArgVarGet(args, 2, "c");
    if(c->type != VAR_NUMBER){
        return &NIL;
    }
    Var* d = ArgVarGet(args, 3, "d");
    if(d->type != VAR_NUMBER){
        return &NIL;
    }
    Var* e = ArgVarGet(args, 4, "e");
    if(e->type != VAR_NUMBER){
        return &NIL;
    }
    Var* f = ArgVarGet(args, 5, "f");
    if(f->type != VAR_NUMBER){
        return &NIL;
    }
    double da;
    memcpy(&da, &a->value, sizeof(double));
    double db;
    memcpy(&db, &b->value, sizeof(double));
    double dc;
    memcpy(&dc, &c->value, sizeof(double));
    double dd;
    memcpy(&dd, &d->value, sizeof(double));
    double de;
    memcpy(&de, &e->value, sizeof(double));
    double df;
    memcpy(&df, &f->value, sizeof(double));
    canvasSetTransform(da, db, dc, dd, de, df);
}

Var* DrawResetTransform(Var* scope, Var* args){
    canvasResetTransform();
}

Var* DrawDrawImage(Var* scope, Var* args){
    Var* image = ArgVarGet(args, 0, "image");
    if(image->type != VAR_STRING){
        return &NIL;
    }
    Var* dx = ArgVarGet(args, 1, "dx");
    if(dx->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dy = ArgVarGet(args, 2, "dy");
    if(dy->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dw = ArgVarGet(args, 3, "dw");
    Var* dh = ArgVarGet(args, 4, "dh");

    if(dw -> type != VAR_NUMBER || dh -> type != VAR_NUMBER){
        canvasDrawImage(image->value, dx->value, dy->value);
        return &NIL;
    }
    double ddx;
    memcpy(&ddx, &dx->value, sizeof(double));
    double ddy;
    memcpy(&ddy, &dy->value, sizeof(double));
    double ddw;
    memcpy(&ddw, &dw->value, sizeof(double));
    double ddh;
    memcpy(&ddh, &dh->value, sizeof(double));
    canvasDrawImage2(image->value, ddx, ddy, ddw, ddh);
}

Var* DrawDrawImage2(Var* scope, Var* args){
    Var* image = ArgVarGet(args, 0, "image");
    if(image->type != VAR_STRING){
        return &NIL;
    }
    Var* sx = ArgVarGet(args, 1, "sx");
    if(sx->type != VAR_NUMBER){
        return &NIL;
    }
    Var* sy = ArgVarGet(args, 2, "sy");
    if(sy->type != VAR_NUMBER){
        return &NIL;
    }
    Var* sw = ArgVarGet(args, 3, "sw");
    if(sw->type != VAR_NUMBER){
        return &NIL;
    }
    Var* sh = ArgVarGet(args, 4, "sh");
    if(sh->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dx = ArgVarGet(args, 5, "dx");
    if(dx->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dy = ArgVarGet(args, 6, "dy");
    if(dy->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dw = ArgVarGet(args, 7, "dw");
    if(dw->type != VAR_NUMBER){
        return &NIL;
    }
    Var* dh = ArgVarGet(args, 8, "dh");
    if(dh->type != VAR_NUMBER){
        return &NIL;
    }
    double dsx;
    memcpy(&dsx, &sx->value, sizeof(double));
    double dsy;
    memcpy(&dsy, &sy->value, sizeof(double));
    double dsw;
    memcpy(&dsw, &sw->value, sizeof(double));
    double dsh;
    memcpy(&dsh, &sh->value, sizeof(double));
    double ddx;
    memcpy(&ddx, &dx->value, sizeof(double));
    double ddy;
    memcpy(&ddy, &dy->value, sizeof(double));
    double ddw;
    memcpy(&ddw, &dw->value, sizeof(double));
    double ddh;
    memcpy(&ddh, &dh->value, sizeof(double));
    canvasDrawImage3(image->value, dsx, dsy, dsw, dsh, ddx, ddy, ddw, ddh);
}

Var* DrawSave(Var* scope, Var* args){
    canvasSave();
}

Var* DrawRestore(Var* scope, Var* args){
    canvasRestore();
}

Var* DrawReset(Var* scope, Var* args){
    canvasReset();
}

Var* DrawRoundRect(Var* scope, Var* args){
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
        return &NIL;
    }
    Var* r = ArgVarGet(args, 4, "r");
    if(r->type != VAR_NUMBER){
        return &NIL;
    }
    double dx;
    memcpy(&dx, &x->value, sizeof(double));
    double dy;
    memcpy(&dy, &y->value, sizeof(double));
    double dw;
    memcpy(&dw, &w->value, sizeof(double));
    double dh;
    memcpy(&dh, &h->value, sizeof(double));
    double dr;
    memcpy(&dr, &r->value, sizeof(double));
    canvasRoundRect(dx, dy, dw, dh, dr);
}

Var* DrawMeasureText(Var* scope, Var* args){
    Var* text = ArgVarGet(args, 0, "text");
    if(text->type != VAR_STRING){
        return &NIL;
    }
    double width = canvasMeasureTextWidth(text->value);
    Var* w = VarNewNumber(width);
    double height = canvasMeasureTextHeight(text->value);
    Var* h = VarNewNumber(height);
    Var* ret = VarNewList();
    ArgVarSet(ret, 0, "w", w);
    ArgVarSet(ret, 1, "h", h);
    return ret;
}

Var* DrawLineWidth(Var* scope, Var* args){
    Var* width = ArgVarGet(args, 0, "width");
    if(width->type != VAR_NUMBER){
        return &NIL;
    }
    double dwidth;
    memcpy(&dwidth, &width->value, sizeof(double));
    canvasLineWidth(dwidth);
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
    CONSTANT(strokeText, VarNewFunction(DrawStrokeText));
    CONSTANT(fillText, VarNewFunction(DrawFillText));
    CONSTANT(font, VarNewFunction(DrawFont));
    CONSTANT(textAlign, VarNewFunction(DrawTextAlign));
    CONSTANT(textBaseline, VarNewFunction(DrawTextBaseline));
    CONSTANT(fillRect, VarNewFunction(DrawFillRect));
    CONSTANT(strokeRect, VarNewFunction(DrawStrokeRect));
    CONSTANT(arc, VarNewFunction(DrawArc));
    CONSTANT(arcTo, VarNewFunction(DrawArcTo));
    CONSTANT(bezierCurveTo, VarNewFunction(DrawBezierCurveTo));
    CONSTANT(quadraticCurveTo, VarNewFunction(DrawQuadraticCurveTo));
    CONSTANT(rect, VarNewFunction(DrawRect));
    CONSTANT(closePath, VarNewFunction(DrawClosePath));
    CONSTANT(clip, VarNewFunction(DrawClip));
    CONSTANT(isPointInPath, VarNewFunction(DrawIsPointInPath));
    CONSTANT(isPointInStroke, VarNewFunction(DrawIsPointInStroke));
    CONSTANT(scale, VarNewFunction(DrawScale));
    CONSTANT(rotate, VarNewFunction(DrawRotate));
    CONSTANT(translate, VarNewFunction(DrawTranslate));
    CONSTANT(transform, VarNewFunction(DrawTransform));
    CONSTANT(setTransform, VarNewFunction(DrawSetTransform));
    CONSTANT(resetTransform, VarNewFunction(DrawResetTransform));
    CONSTANT(drawImage, VarNewFunction(DrawDrawImage));
    CONSTANT(drawImage2, VarNewFunction(DrawDrawImage2));
    CONSTANT(save, VarNewFunction(DrawSave));
    CONSTANT(restore, VarNewFunction(DrawRestore));
    CONSTANT(reset, VarNewFunction(DrawReset));
    CONSTANT(roundRect, VarNewFunction(DrawRoundRect));
    CONSTANT(measureText, VarNewFunction(DrawMeasureText));
    CONSTANT(lineWidth, VarNewFunction(DrawLineWidth));

}

#endif