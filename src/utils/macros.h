#ifndef MACROS_H 
#define MACROS_H

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

#define SALT_MAX 16
#define HASH_MAX 65 // SHA-256 = 64 + '\0'

#define KEY_MAX 30  // = USERNAME_MAX 
#define HASHMAP_SIZE 61 // prime number

#define DATETIME_MAX 17 // "2026-05-16 00:25" + '\0'

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
