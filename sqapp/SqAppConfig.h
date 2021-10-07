#ifndef SQ_APP_CONFIG_H
#define SQ_APP_CONFIG_H


/* ------ SQL products ------ use one of products */
#define SQLITE
// #define MYSQL

/* ------ Configuration ------ */
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""
#define DB_DATABASE    "sqxc-example"

// SQLite Configuration
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL


#endif  // SQ_APP_CONFIG_H
