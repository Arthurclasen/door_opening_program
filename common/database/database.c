#include <stdio.h>
#include <mysql/mysql.h>
#include <../structs/infos.h>
#include <../structs/auxiliar.h>

int create_user(MYSQL *conn, User *user)
{
    MYSQL_STMT *stmt;   // prepared statement
    MYSQL_BIND bind[5]; // user elements that will be added

    // query
    const char *query = 
        "INSERT INTO users (name, email, password_hash, private_key, team) VALUES (?, ?, ?, ?, ?)";

    stmt = mysql_stmt_init(conn);

    if (!stmt)
    {
        printf("mysql_stmt_init() failed\n");
        return 1;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    memset(bind, 0, sizeof(bind));

    char buff_name[5] = {user->name, user->email, user->password_hash, user->private_key, user->team};
    for (int i = 0; i < 5; i++)
    {
        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = buff_name[i];
        bind[i].buffer_length = strlen(buff_name[i]);
    }
    bind[4].buffer_type = MYSQL_TYPE_ENUM;
    free(buff_name);

    if (mysql_stmt_bind_param(stmt, bind))
    {
        printf("Bind failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    if (mysql_stmt_execute(stmt))
    {
        printf("Execute failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    printf("User inserted!\n");

    mysql_stmt_close(stmt);
    return 0;
}

MYSQL_BIND* binding(int num, AnyParam data[], MYSQL_STMT *stmt, int queryXresponse)
{
    // transforming the params/results into something that can be binded with the stmt (query)
    MYSQL_BIND *bind = calloc(num, sizeof(MYSQL_BIND));
    memset(bind, 0, sizeof(MYSQL_BIND) * num);
    for (int i = 0; i < num; i++)
    {
        switch (data[i].type)
        {
        case TYPE_INT:
            bind[i].buffer_type  = MYSQL_TYPE_LONG;
            bind[i].buffer       = data[i].value;
            break;
        case TYPE_LONG:
            bind[i].buffer_type  = MYSQL_TYPE_LONGLONG;
            bind[i].buffer       = data[i].value;
            break;
        case TYPE_STRING:
            bind[i].buffer_type  = MYSQL_TYPE_STRING;
            bind[i].buffer       = data[i].value;
            bind[i].buffer_length= data[i].length;
            break;
        }
    }

    if (queryXresponse == 0) // binding the params to the stmt
    {
        if (mysql_stmt_bind_param(stmt, bind))
        {
            printf("Bind failed: %s\n", mysql_stmt_error(stmt));
            return NULL;
        }
    } 
    else // binding the results to the stmt
    {
       if (mysql_stmt_bind_result(stmt, bind))
        {
            return NULL;
        } 
    }
    return bind;
}

int read(MYSQL *conn, const char *query, AnyParam params[], AnyParam results[], int num_param, int num_results)
{
    MYSQL_STMT *stmt;   // prepared statement for searching user

    // creating stmt (prepared statement) relating to conn
    stmt = mysql_stmt_init(conn);          
    if (mysql_stmt_prepare(stmt, query, strlen(query))) // preparing the stmt query
    {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    // binding the params[] into the stmt query
    MYSQL_BIND *bind_params;   // variable for binding params 
    if ((bind_params = binding(num_param, params, stmt, 0)) == NULL)
    {
        printf("Bind failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }
    
    if (mysql_stmt_execute(stmt))   // executing the search itself
    {
        printf("Execute failed: %s\n", mysql_stmt_error(stmt));
        free(bind_params);
        return 1;
    }
    free(bind_params); // will not use this variable anymore

    // binding the results into results[]
    MYSQL_BIND *bind_results;   // variable for binding results 
    if ((bind_results = binding(num_results, results, stmt, 1)) == NULL)
    {
        printf("Bind failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    // fetch into results[]
    int status = mysql_stmt_fetch(stmt);
    
    free(bind_results); // cleaning memory
    mysql_stmt_close(stmt); // close stmt

    return (status == 0) ? 0 : 1;
}