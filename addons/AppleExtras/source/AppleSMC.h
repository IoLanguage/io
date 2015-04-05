#include <mach/mach.h>
//#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

typedef char		SMCBytes_t[32];
typedef char		UInt32Char_t[5];
typedef char		Flag[1];
typedef UInt		flag;
typedef UInt16		PWMValue;

typedef struct SMCKeyData_vers_t {
	char                  major;
	char                  minor;
	char                  build;
	char                  reserved[1];
	UInt16                release;
} SMCKeyData_vers_t;

typedef struct {
	UInt16                version;
	UInt16                length;
	UInt32                cpuPLimit;
	UInt32                gpuPLimit;
	UInt32                memPLimit;
} SMCKeyData_pLimitData_t;

typedef struct {
	UInt32                dataSize;
	UInt32                dataType;
	char                  dataAttributes;
} SMCKeyData_keyInfo_t;

typedef struct {
	UInt32                  key;
	SMCKeyData_vers_t       vers;
	SMCKeyData_pLimitData_t pLimitData;
	SMCKeyData_keyInfo_t    keyInfo;
	char                    result;
	char                    status;
	char                    data8;
	UInt32                  data32;
	SMCBytes_t              bytes;
} SMCKeyData_t;

typedef struct {
	UInt32Char_t            key;
	UInt32                  dataSize;
	UInt32Char_t            dataType;
	SMCBytes_t              bytes;
} SMCVal_t;

io_connect_t smcGetDataPort(void);