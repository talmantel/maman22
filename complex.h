typedef struct{
    double r, i;
} complex;

void readComp(complex *comp, double r, double i);
void printComp(complex *comp);
void addComp(complex *comp1, complex *comp2);
void subComp(complex *comp1, complex *comp2);
void multCompReal(complex *comp, double r);
void multCompImg(complex *comp, double i);
void multCompComp(complex *comp1, complex *comp2);
void absComp(complex *comp);