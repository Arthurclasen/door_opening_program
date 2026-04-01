#include "../cJSON/cJSON.h"

#ifndef NETWORK_H
#define NETWORK_H

// routing
// defines a standard func to handlers
typedef void (*route_handler)(int client_fd, cJSON *request_json);

// route struct 
typedef struct {
    char            *method;
    char            *path;
    route_handler   handler;
    int             auth_required;  // 1 - yes / 0 - no
} Route;

// http struct
typedef struct 
{
    char    method[8];
    char    path[256];
    int     content_length;
    cJSON   *json;
} HttpRequest;


// code to create server
int server_create(Route routes[], const char *port);

#endif