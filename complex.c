#include <stdio.h>
#include <math.h>
#include "complex.h"


void readComp(complex *comp, double r, double i){
    comp->r = r;
    comp->i = i;
}

void printComp(complex *comp){
    if(comp->i >= 0)
        printf("%.2f + (%.2f)i\n", comp->r, comp->i);
    else
        printf("%.2f - (%.2f)i\n", comp->r, -comp->i);
}

complex addComp(complex *comp1, complex *comp2){
    complex result;
    result.r = comp1->r + comp2->r;
    result.i = comp1->i + comp2->i;
    return result;
}

complex subComp(complex *comp1, complex *comp2){
    complex result;
    result.r = comp1->r - comp2->r;
    result.i = comp1->i - comp2->i;
    return result;
}

complex multCompReal(complex *comp, double r){
    complex result;
    result.r = r * comp->r;
    result.i = r * comp->i;
    return result;
}

complex multCompImg(complex *comp, double i){
    complex result;
    result.r = -i * comp->i;
    result.i = i * comp->r;
    return result;
}

complex multCompComp(complex *comp1, complex *comp2){
    complex result;
    result.r = ((comp1->r) * (comp2->r)) - ((comp1->i) * (comp2->i));
    result.i = ((comp1->r) * (comp2->i)) + ((comp1->i) * (comp2->r));
    return result;
}

complex absComp(complex *comp){
    complex result;
    result.r = sqrt(pow(comp->r, 2) + pow(comp->i, 2));
    result.i = 0;
    return result;
}