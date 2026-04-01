#include <time.h>

#ifndef INFOS_H
#define INFOS_H

// which team user is
typedef enum
{
    RED,
    GREEN,
    BLUE,
    ADMIN,
    MONITORING
} Team;

// user informations
typedef struct 
{
    long    id;
    char    *name;
    char    *email;
    char    *password_hash;
    char    *private_key;
    Team    team;
    time_t  created_at;
} User;

// door that will be opened
typedef struct 
{
    long id;
    char *door_name;
    int  is_open;
} Door;

// failures reasons in enum
typedef enum 
{
    INVALID_CREDENTIAL,
    ACCESS_DENIED,
    EXPIRED_CERTIFICATE,
    UNKNOWN_USER,
    SYSTEM_ERROR
} Failure_reason;

// actions of opening the doors
typedef struct 
{
    long            id;
    time_t          attempted_at;
    int             success;
    Failure_reason  failure_reason;
    char            *certificate;
    long            user_id;
    long            door_id;
} Door_events;

#endif

