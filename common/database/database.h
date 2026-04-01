#include <mysql/mysql.h>
#include "../structs/infos.h"
#include "../structs/auxiliar.h"

#ifndef  DATABASE_H
#define DATABASE_H

int create_user(MYSQL *conn, User *user);

int read(MYSQL *conn, const char *query, AnyParam params[], AnyParam results[], int num_param, int num_results);

#endif