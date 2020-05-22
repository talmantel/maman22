typedef struct{
    double r, i;
} complex;

void readComp(complex *comp, double r, double i);
void printComp(complex *comp);
complex addComp(complex *comp1, complex *comp2);
complex subComp(complex *comp1, complex *comp2);
complex multCompReal(complex *comp, double r);
complex multCompImg(complex *comp, double i);
complex multCompComp(complex *comp1, complex *comp2);
complex absComp(complex *comp);