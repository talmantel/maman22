#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "complex.h"
#include "getch.h"

#define MAX_EXPECTED_PARAMS 3
#define MAX_COMMAND_LENGTH 15
#define MAX_DOUBLE_LENGTH 20
#define MAX_COMPLEX_LENGTH 1


typedef enum{CONTINUE, FINISH} loopState;

typedef enum{FALSE, TRUE} bool;

typedef enum{NONE, DOUBLE, VAR} expectedParamType;

typedef enum{READ_COMP, PRINT_COMP, ADD_COMP, SUB_COMP, MULT_COMP_REAL, MULT_COMP_IMG, MULT_COMP_COMP, ABS_COMP, STOP} commandType;

char *commandNames[] = {
        [READ_COMP] = "read_comp",
        [PRINT_COMP] = "print_comp",
        [ADD_COMP] = "add_comp",
        [SUB_COMP] = "sub_comp",
        [MULT_COMP_REAL] = "mult_comp_real",
        [MULT_COMP_IMG] = "mult_comp_img",
        [MULT_COMP_COMP] = "mult_comp_comp",
        [ABS_COMP] = "abs_comp",
        [STOP] = "stop"
};

expectedParamType expectedParamsTypes[][MAX_EXPECTED_PARAMS] = {
        [READ_COMP] = {VAR, DOUBLE, DOUBLE},
        [PRINT_COMP] = {VAR, NONE, NONE},
        [ADD_COMP] = {VAR, VAR, NONE},
        [SUB_COMP] = {VAR, VAR, NONE},
        [MULT_COMP_REAL] = {VAR, DOUBLE, NONE},
        [MULT_COMP_IMG] =  {VAR, DOUBLE, NONE},
        [MULT_COMP_COMP] =  {VAR, VAR, NONE},
        [ABS_COMP] = {VAR, NONE, NONE},
        [STOP] = {NONE, NONE, NONE}
};

typedef enum{ERR_UNDEFINED_VAR, ERR_UNDEFINED_COMMAND, ERR_PARAM_NOT_NUM,
    ERR_MISSING_PARAM, ERR_EXTRA_TEXT, ERR_CONSECUTIVE_COMMAS,
    ERR_MISSING_COMMA, ERR_ILLEGAL_COMMA, ERR_EOF_BEFORE_STOP, VALID} error;

char *errors[] = {
        [ERR_UNDEFINED_VAR] = "Invalid variable name",
        [ERR_UNDEFINED_COMMAND] = "Undefined command name",
        [ERR_PARAM_NOT_NUM] = "Invalid parameter - not a number",
        [ERR_MISSING_PARAM] = "Missing Parameter",
        [ERR_EXTRA_TEXT] = "Extraneous text after command",
        [ERR_CONSECUTIVE_COMMAS] = "Multiple consecutive commas",
        [ERR_MISSING_COMMA] = "Missing comma",
        [ERR_ILLEGAL_COMMA] = "Illegal comma",
        [ERR_EOF_BEFORE_STOP] = "Reached end of file before 'stop' command"
};


typedef union  {
    complex *var;
    double d;
} expectedParam;

loopState parseAndRunCommand();
error finishReadingLine(bool returnErrorOnNotEmpty);
error getCommandType(commandType *command);
error getNextParamString(char *param, bool needsComma, int maxLength);
error getExpectedParams(commandType type, expectedParam *params);
error getComplexParam(expectedParam *param, bool needsComma);
error getDoubleParam(expectedParam *param, bool needsComma);
loopState callCommandWithParams(commandType command, expectedParam *params);
void skipWhiteSpaces();
void printErrorAndFinishReadingLine(error err);
complex *getVarByChar(char c);

complex A = {0.0, 0.0};
complex B = {0.0, 0.0};
complex C = {0.0, 0.0};
complex D = {0.0, 0.0};
complex E = {0.0, 0.0};
complex F = {0.0, 0.0};

int main() {
    while(1){
        printf("Please enter a command\n");
        if(parseAndRunCommand() == FINISH)
            break;
    }
}


loopState parseAndRunCommand(){
    expectedParam params[MAX_EXPECTED_PARAMS];
    commandType command;
    error result;
    int nextChar;

    skipWhiteSpaces();

    nextChar = getch();
    if(nextChar == EOF){
        printErrorAndFinishReadingLine(ERR_EOF_BEFORE_STOP);
        return FINISH;
    }
    else if(nextChar == '\n')
        return CONTINUE;

    ungetch(nextChar);

    result = getCommandType(&command);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    result = getExpectedParams(command, params);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    result = finishReadingLine(TRUE);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    return callCommandWithParams(command, params);
}

error getCommandType(commandType *command){

    char c[MAX_COMMAND_LENGTH];
    int ch;
    int i = -1;

    skipWhiteSpaces();

    do {
        i++;
        ch = getch();
        c[i] = (char)ch;
    }while (i < MAX_COMMAND_LENGTH && ch != ' ' && ch != '\t'  && ch != ',' && ch != '\n' && ch != EOF);

    if(ch == ',')
        return ERR_ILLEGAL_COMMA;

    if(ch == '\n' || ch == EOF)
        ungetch(c[i]);

    c[i] = '\0';

    for(i = 0; i < sizeof(commandNames)/sizeof(commandNames[0]); i++){
        if(strcmp(c, commandNames[i]) == 0) {
            *command = i;
            return VALID;
        }
    }

    return ERR_UNDEFINED_COMMAND;
}

error getExpectedParams(commandType type, expectedParam *params){
    int i;
    error result;
    bool needsComma = FALSE;

    for(i = 0; i < MAX_EXPECTED_PARAMS; i++){
        switch (expectedParamsTypes[type][i]) {
            case DOUBLE:
                result = getDoubleParam(params + i, needsComma);
                if(result != VALID)
                    return result;
                break;
            case VAR:
                result = getComplexParam(params + i, needsComma);
                if(result != VALID)
                    return result;
                break;
        }

        if(needsComma == FALSE)
            needsComma = TRUE;
    }

    return VALID;
}

error getComplexParam(expectedParam *param, bool needsComma){
    int maxInputLength = MAX_COMPLEX_LENGTH + 1;
    char paramString[maxInputLength];
    error result = getNextParamString(paramString, needsComma, maxInputLength);

    if(result != VALID)
        return result;
    else if(strlen(paramString) > MAX_COMPLEX_LENGTH)
        return ERR_UNDEFINED_VAR;

    param -> var = getVarByChar((char)paramString[0]);
    if(param -> var == NULL)
        return ERR_UNDEFINED_VAR;

    return VALID;
}

error getDoubleParam(expectedParam *param, bool needsComma){
    int i = 0;
    bool hasPeriod = FALSE;
    int maxInputLength = MAX_DOUBLE_LENGTH + 1;
    char paramString[maxInputLength];

    error result = getNextParamString(paramString, needsComma, maxInputLength);

    if(result != VALID)
        return result;
    else if(strlen(paramString) > MAX_DOUBLE_LENGTH)
        return ERR_PARAM_NOT_NUM;

    while(paramString[i] != '\0'){
        if(paramString[i] == '-'){
            if(i != 0)
                return ERR_PARAM_NOT_NUM;
        }
        else if(paramString[i] == '.'){
            if(hasPeriod == TRUE)
                return ERR_PARAM_NOT_NUM;
            hasPeriod = TRUE;
        }
        else if(isdigit(paramString[i]) == 0)
            return ERR_PARAM_NOT_NUM;
        i++;
    }

    sscanf((char *)paramString, "%lf", &param->d);

    return VALID;
}

error getNextParamString(char *param, bool needsComma, int maxLength){
    int i = -1;
    int nextChar;

    skipWhiteSpaces();

    if(needsComma == TRUE){
        nextChar = getch();
        if(nextChar != ',') {
            ungetch(nextChar);
            if(nextChar != '\n' && nextChar != EOF)
                return ERR_MISSING_COMMA;
            else
                return ERR_MISSING_PARAM;
        }
        skipWhiteSpaces();
    }
    else if(needsComma == FALSE)
    {
        nextChar = getch();
        if(nextChar == ',')
            return ERR_ILLEGAL_COMMA;
        else
            ungetch(nextChar);
    }

    do {
        i++;
        nextChar = getch();
        param[i] = (char)nextChar;
    }while (i < maxLength && nextChar != ' ' && nextChar != '\t' && nextChar != '\n' && nextChar != EOF && nextChar != ',');

    if(i == 0 && nextChar == ',')
        return ERR_CONSECUTIVE_COMMAS;
    else if(nextChar == '\n' || nextChar == EOF || nextChar == ',')
        ungetch(nextChar);

    param[i] = '\0';

    if(i == 0)
        return ERR_MISSING_PARAM;

    return VALID;
}




complex *getVarByChar(char c){
    switch (c) {
        case 'A':
            return &A;
        case 'B':
            return &B;
        case 'C':
            return &C;
        case 'D':
            return &D;
        case 'E':
            return &E;
        case 'F':
            return &F;
        default:
            return NULL;
    }
}

void skipWhiteSpaces(){
    int c;
    do{
        c = getch();
    }while (c == ' ' || c == '\t');
    ungetch(c);
}


void printErrorAndFinishReadingLine(error err){
    if(err != ERR_EOF_BEFORE_STOP)
        finishReadingLine(FALSE);

    printf("%s\n", errors[err]);
}

error finishReadingLine(bool returnErrorOnNotEmpty){
    int c;
    do{
        c = getch();
        if(returnErrorOnNotEmpty == TRUE && c != ' ' && c != '\t' && c != '\n' && c != EOF)
            return ERR_EXTRA_TEXT;
    }while (c != '\n' && c != EOF);

    if(c == EOF)
        ungetch(c);

    return VALID;
}


loopState callCommandWithParams(commandType command, expectedParam *params) {
    switch (command) {
        case READ_COMP:
            readComp(params[0].var, params[1].d, params[2].d);
            break;
        case PRINT_COMP:
            printComp(params[0].var);
            break;
        case ADD_COMP:
            addComp(params[0].var, params[1].var);
            break;
        case SUB_COMP:
            subComp(params[0].var, params[1].var);
            break;
        case MULT_COMP_REAL:
            multCompReal(params[0].var, params[1].d);
            break;
        case MULT_COMP_IMG:
            multCompImg(params[0].var, params[1].d);
            break;
        case MULT_COMP_COMP:
            multCompComp(params[0].var, params[1].var);
            break;
        case ABS_COMP:
            absComp(params[0].var);
            break;
        case STOP:
            return FINISH;
    }
    return CONTINUE;
}