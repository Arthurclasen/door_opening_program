#include "../common/network/network.h"
#include <stdio.h>
#include "../common/cJSON/cJSON.h"
#include <mysql/mysql.h>

void extract_data(cJSON *json, char *user, char *pass)
{
    if (!json)
    {
        printf("NULL JSON\n");
        return; 
    }

    cJSON *user_item = cJSON_GetObjectItemCaseSensitive(json, "user");
    cJSON *pass_item = cJSON_GetObjectItemCaseSensitive(json, "pass");

    if (!cJSON_IsString(user_item) || !cJSON_IsString(pass_item))
    {
        printf("Invalid field\n");
        return;
    }

    user = user_item->valuestring;
    pass = pass_item->valuestring;

    printf("User: %s\n", user);
    printf("Pass: %s\n", pass);
}

void create_account(int client_fd, cJSON *json)
{
    printf("CREATE\n");
    char *user, *pass;
    extract_data(json, user, pass);
}

void login(int client_fd, cJSON *json)
{
    printf("LOGIN\n");
    char *user, *pass;
    extract_data(json, user, pass);
    
}

void verify_code(int client_fd, cJSON *json)
{
    printf("VERIFY\n");
}

int main()
{
    Route routes[] = 
    {
    {"POST", "/appdoor/login", login, 0},
    {"POST", "/appdoor/create",  create_account,  0}, 
    {"POST", "/appdoor/verify-code", verify_code, 0},
    {NULL, NULL, NULL, 0}
    };

    // get environment variables
    char *db_username = getenv("DATABASE_USERNAME_DOOR");
    char *db_password = getenv("DATABASE_PASSWORD_DOOR");
    char *db_databasename = getenv("DATABASE_NAME_DOOR");

    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "mysql-db", db_username, db_password, db_databasename, 3306, NULL, 0))
    {
        printf("Error: %s\n", mysql_error(conn));
        return 1;
    }
    
    printf("Connected to MySQL!\n");

    mysql_query(conn, "SELECT VERSION()");
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    printf("Versão MySQL: %s\n", row[0]);

    if (server_create(routes, "3490") == 1)
    {
        printf("error");
        mysql_free_result(res);
        mysql_close(conn);
        return 1;
    }
    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}