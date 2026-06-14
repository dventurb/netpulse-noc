#ifndef MACROS_H 
#define MACROS_H

#include <sodium.h>

#define USERNAME_MAX 30
#define STRING_MAX 100
#define DESCRIPTION_MAX 200
#define COMMAND_MAX 300 

#define ID_MAX 14 // "EQ-xxx" : (10 digits + '\0')
#define CODE_MAX 30 // ex.: TEMP_RACK 
#define NUMBER_MAX 11 // 10 digits + '\0' (always positive int)

#define IP_MAX 16  // "255.255.255.255" + '\0'
#define MAC_MAX 18 // "ff.ff.ff.ff.ff.ff" + '\0'

#define UNIT_MAX 10 // ex.: rpm
#define VALUE_MAX 20 // ex.: 9999 rpm + '\0'

#define PASSWORD_HASH_MAX crypto_pwhash_STRBYTES // crypto_pwhash_STRBYTES

#define KEY_MAX 30  // = USERNAME_MAX 
#define HASHMAP_SIZE 61 // prime number

#define DATE_MAX 11   // "14-06-2026" + '\0'
#define DATETIME_MAX 17 // "16-05-2026 00:25" + '\0'

#define NAME_MIN 2
#define USERNAME_MIN 3
#define PASSWORD_MIN 8

#define CELL_ID_WIDTH 100
#define CELL_NAME_WIDTH 160
#define CELL_TYPE_WIDTH 140
#define CELL_VENDOR_WIDTH 140
#define CELL_MODEL_WIDTH 160
#define CELL_IP_ADDRESS_WIDTH 140
#define CELL_MAC_ADDRESS_WIDTH 170
#define CELL_LOCATION_WIDTH 180
#define CELL_STATUS_WIDTH 160
#define CELL_LAST_CHECK_WIDTH 160

#define CELL_NUMBER_WIDTH 100 
#define CELL_QUEUE_WIDTH 80
#define CELL_SOURCE_WIDTH 160
#define CELL_DESCRIPTION_WIDTH 250
#define CELL_PRIORITY_WIDTH 140 
#define CELL_TECHNICIAN_WIDTH 160 
#define CELL_CREATED_WIDTH 160 
#define CELL_CONCLUDED_WIDTH 160

#define CELL_CODE_WIDTH 100
#define CELL_VALUE_WIDTH 140 
#define CELL_READ_AT_WIDTH 160

#endif
