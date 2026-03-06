#ifndef AUXILIAR_H
#define AUXILIAR_H

// possible params for doors and users
typedef enum
{
    TYPE_INT,
    TYPE_LONG,
    TYPE_STRING
} ParamType;

// parameters generalization 
typedef struct 
{
    ParamType       type;
    void            *value;
    unsigned long   length;
} AnyParam;

#endif