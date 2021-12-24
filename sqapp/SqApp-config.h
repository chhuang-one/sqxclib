#ifndef SQ_APP_CONFIG_H
#define SQ_APP_CONFIG_H

#ifndef SQ_APP_CONFIG_FILE


// ----------------------------------------------------------------------------
// --- SQL products --- You can enable only one SQL products here

#define DB_SQLITE
// #define DB_MYSQL

// ----------------------------------------------------------------------------
// --- Database Configuration ---

// connection configuration values
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""

// common configuration values
#define DB_DATABASE    "sqapp-example"

// SQLite configuration values
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL


#else   // SQ_APP_CONFIG_FILE has been defined, it will include user's configuration file.
#include SQ_APP_CONFIG_FILE
#endif  // SQ_APP_CONFIG_FILE

#endif  // SQ_APP_CONFIG_H
