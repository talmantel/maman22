#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "complex.h"
#include "getch.h"

#define MAX_EXPECTED_PARAMS 3
#define MAX_COMMAND_NAME_LENGTH 15
#define MAX_DOUBLE_PARAM_LENGTH 20
#define MAX_COMPLEX_PARAM_LENGTH 1


typedef enum{CONTINUE, FINISH} loopState;

typedef enum{FALSE, TRUE} bool;

typedef enum{NONE, DOUBLE, VAR} expectedParamType;

typedef enum{READ_COMP, PRINT_COMP, ADD_COMP, SUB_COMP, MULT_COMP_REAL, MULT_COMP_IMG, MULT_COMP_COMP, ABS_COMP, STOP} commandType;

/* Array defining the name for each command */
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

/* Array defining the expected parameter types for each command type */
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
    ERR_MISSING_COMMA, ERR_ILLEGAL_COMMA, ERR_EOF_BEFORE_STOP, VALID} parseResult;

/* Array defining a human readable error for each parse error enum */
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

/* combined type for complex and double*/
typedef union  {
    complex *var;
    double d;
} expectedParam;

loopState parseAndRunCommand();
parseResult finishReadingLine(bool shouldBeEmpty);
parseResult getCommandType(commandType *command);
parseResult getNextParamString(char *param, bool needsComma, int maxLength);
parseResult getExpectedParams(commandType command, expectedParam *params);
parseResult getComplexParam(expectedParam *param, bool needsComma);
parseResult getDoubleParam(expectedParam *param, bool needsComma);
void callCommandWithParams(commandType command, expectedParam *params);
void skipWhiteSpaces();
void printErrorAndFinishReadingLine(parseResult err);
complex *getVarByChar(char c);

complex A = {0.0, 0.0};
complex B = {0.0, 0.0};
complex C = {0.0, 0.0};
complex D = {0.0, 0.0};
complex E = {0.0, 0.0};
complex F = {0.0, 0.0};

/* Parses and executes commands from input, until reaches EOF or stop command */
int main() {
    loopState state;
    do{
        printf("Please enter a command\n");
        state = parseAndRunCommand();
    }while(state == CONTINUE);
}

/*
    Parses and executes a single command from the standard input
    Returns a loopState - FINISH if EOF reached or 'stop' command was executed, CONTINUE otherwise
 */
loopState parseAndRunCommand(){
    expectedParam params[MAX_EXPECTED_PARAMS];
    commandType command;
    parseResult result;
    int nextChar;

    skipWhiteSpaces();

    /* Check if there is any non-whitespace input to parse, and whether end of file was reached*/
    nextChar = getch();
    if(nextChar == EOF){
        printErrorAndFinishReadingLine(ERR_EOF_BEFORE_STOP);
        return FINISH;
    }
    else if(nextChar == '\n')
        return CONTINUE;

    ungetch(nextChar);

    /* Parse the command name */
    result = getCommandType(&command);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    /* parse parameters from input, depending on the command */
    result = getExpectedParams(command, params);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    /* read until end of line, to make sure there isn't any extra text after the command and the expected paramenters */
    result = finishReadingLine(TRUE);
    if(result != VALID) {
        printErrorAndFinishReadingLine(result);
        return CONTINUE;
    }

    if(command == STOP)
        return FINISH;
    else {
        /* execute the command with the parsed parameters */
        callCommandWithParams(command, params);
        return CONTINUE;
    }
}

/*
    Parse a command name from standard input
    Receives a pointer, stores the parsed command type (enum) in it
    Returns a parseResult - a parse error or VALID if no errors were found
 */
parseResult getCommandType(commandType *command){

    char c[MAX_COMMAND_NAME_LENGTH];
    int ch;
    int i = -1;

    skipWhiteSpaces();

    /*  copy the command name from standard input into a char array*/
    do {
        i++;
        ch = getch();
        c[i] = (char)ch;
    }while (i < MAX_COMMAND_NAME_LENGTH && ch != ' ' && ch != '\t' && ch != ',' && ch != '\n' && ch != EOF);
    c[i] = '\0';

    if(ch == '\n' || ch == EOF)
        ungetch(ch);

    /* return an error if there is a comma in the command name */
    if(ch == ',')
        return ERR_ILLEGAL_COMMA;

    /*
       Compare the received command name with the defined command names in 'commandNames'
       Save the command's enum in the command pointer location received as an argument
    */
    for(i = 0; i < sizeof(commandNames)/sizeof(commandNames[0]); i++){
        if(strcmp(c, commandNames[i]) == 0) {
            *command = i;
            return VALID;
        }
    }

    return ERR_UNDEFINED_COMMAND;
}

/*
    Parse the expected parameters for a command type (defined in expectedParamsTypes) from the standard input
    Receives a the commandType, and a params array to store the received parameters in
    Returns a parseResult - a parse error or VALID if no errors were found
 */
parseResult getExpectedParams(commandType command, expectedParam params[]){
    int i;
    parseResult result;
    bool needsComma = FALSE;

    /*
       Loop through the expected parameter types for the command type and try to parse a parameter of that type from input.
       Return a parse error if any occurred
    */
    for(i = 0; i < MAX_EXPECTED_PARAMS; i++){
        switch (expectedParamsTypes[command][i]) {
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

/*
    Parse a single 'Complex' parameter from standard input
    Receives a pointer in which to store the received parameter, and a boolean indicating whether there should be a comma before the parameter
    Returns a parseResult - a parse error or VALID if no errors were found
 */
parseResult getComplexParam(expectedParam *param, bool needsComma){
    int maxInputLength = MAX_COMPLEX_PARAM_LENGTH + 1;
    char paramString[maxInputLength];

    parseResult result = getNextParamString(paramString, needsComma, maxInputLength);
    if(result != VALID)
        return result;

    /* Return a parse error if param is longer than the max length for this parameter type */
    if(strlen(paramString) > MAX_COMPLEX_PARAM_LENGTH)
        return ERR_UNDEFINED_VAR;

    /* Convert the char received from input into the matching complex variable */
    param -> var = getVarByChar((char)paramString[0]);

    if(param -> var == NULL)
        return ERR_UNDEFINED_VAR;

    return VALID;
}

/*
    Parse a single 'double' parameter from standard input
    Receives a pointer in which to store the received parameter, and a boolean indicating whether there should be a comma before the parameter
    Returns a parseResult - a parse error or VALID if no errors were found
 */
parseResult getDoubleParam(expectedParam *param, bool needsComma){
    int i = 0;
    bool hasPeriod = FALSE;
    int maxInputLength = MAX_DOUBLE_PARAM_LENGTH + 1;
    char paramString[maxInputLength];

    parseResult result = getNextParamString(paramString, needsComma, maxInputLength);

    if(result != VALID)
        return result;

    /* Return a parse error if param is longer than the max length for this parameter type */
    if(strlen(paramString) > MAX_DOUBLE_PARAM_LENGTH)
        return ERR_PARAM_NOT_NUM;


    /* Make sure the input string represents a double */
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

    /* Convert the received string into a double and store it in param */
    sscanf((char *)paramString, "%lf", &param->d);

    return VALID;
}


/*
    Copy the next parameter from standard input into the char array 'param' received as an argument
    Receives an in which to store the received parameter string, a boolean indicating whether there should be a comma before the parameter,
    and a max length for the parameter.
    Returns a parseResult - a parse error or VALID if no errors were found
 */
parseResult getNextParamString(char param[], bool needsComma, int maxLength){
    int i = -1;
    int nextChar;

    skipWhiteSpaces();

    /* Make sure there is a comma if its expected, or there isn't one if its not expected */
    if(needsComma == TRUE){
        nextChar = getch();
        if(nextChar != ',') {
            ungetch(nextChar);
            if(nextChar != '\n' && nextChar != EOF)
                return ERR_MISSING_COMMA;
            else
                return ERR_MISSING_PARAM;
        }
        /* skip whitespaces again after a comma */
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


    /*  copy the next parameter from standard input into a char array */
    do {
        i++;
        nextChar = getch();
        param[i] = (char)nextChar;
    }while (i < maxLength && nextChar != ' ' && nextChar != '\t' && nextChar != '\n' && nextChar != EOF && nextChar != ',');
    param[i] = '\0';

    if(nextChar == '\n' || nextChar == EOF || nextChar == ',')
        ungetch(nextChar);

    /* Return parse error if the parameter is empty */
    if(i == 0) {
        if(nextChar == ',')
            return ERR_CONSECUTIVE_COMMAS;
        else
            return ERR_MISSING_PARAM;
    }

    return VALID;
}



/*
    Convert a char into the matching complex variable pointer
    Receives a character
    Returns a pointer to a complex variable or NULL if there isnt one matching the character
 */
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

/*
 Read from standard input until a non-whitespace character is reached
 */
void skipWhiteSpaces(){
    int c;
    do{
        c = getch();
    }while (c == ' ' || c == '\t');
    ungetch(c);
}


/*
 Receives a parseResult (anything but VALID)
 Prints a human readable error and discards the rest of the command (line from standard input)
 */
void printErrorAndFinishReadingLine(parseResult err){
    if(err != ERR_EOF_BEFORE_STOP)
        finishReadingLine(FALSE);

    printf("%s\n", errors[err]);
}

/*
    Reads until the end of the line.
    Receives a boolean indicating whether non-whitespace characters are allowed
    Returns a parseResult - a parse error if shouldBeEmpty is TRUE and a non-whitespace character was found, VALID otherwise
 */
parseResult finishReadingLine(bool shouldBeEmpty){
    int c;
    do{
        c = getch();
        if(shouldBeEmpty == TRUE && c != ' ' && c != '\t' && c != '\n' && c != EOF)
            return ERR_EXTRA_TEXT;
    }while (c != '\n' && c != EOF);

    if(c == EOF)
        ungetch(c);

    return VALID;
}

/*
    Executes a command with parameters
    Receives a command to execute and an array of parameters to execute the command with
 */
void callCommandWithParams(commandType command, expectedParam *params) {
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
    }
}