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

void addComp(complex *comp1, complex *comp2){
    complex result;
    result.r = comp1->r + comp2->r;
    result.i = comp1->i + comp2->i;
    printComp(&result);
}

void subComp(complex *comp1, complex *comp2){
    complex result;
    result.r = comp1->r - comp2->r;
    result.i = comp1->i - comp2->i;
    printComp(&result);
}

void multCompReal(complex *comp, double r){
    complex result;
    result.r = r * comp->r;
    result.i = r * comp->i;
    printComp(&result);
}

void multCompImg(complex *comp, double i){
    complex result;
    result.r = -i * comp->i;
    result.i = i * comp->r;
    printComp(&result);
}

void multCompComp(complex *comp1, complex *comp2){
    complex result;
    result.r = ((comp1->r) * (comp2->r)) - ((comp1->i) * (comp2->i));
    result.i = ((comp1->r) * (comp2->i)) + ((comp1->i) * (comp2->r));
    printComp(&result);
}

void absComp(complex *comp){
    complex result;
    result.r = sqrt(pow(comp->r, 2) + pow(comp->i, 2));
    result.i = 0;
    printComp(&result);
}