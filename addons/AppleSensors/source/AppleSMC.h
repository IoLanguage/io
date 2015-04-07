#include <mach/mach.h>
//#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

#define KERNEL_INDEX_SMC      2
#define SMC_CMD_READ_BYTES    5
#define SMC_CMD_WRITE_BYTES   6
#define SMC_CMD_READ_INDEX    8
#define SMC_CMD_READ_KEYINFO  9
#define SMC_CMD_READ_PLIMIT   11
#define SMC_CMD_READ_VERS     12

#define DATATYPE_FP1F         "fp1f"
#define DATATYPE_FP4C         "fp4c"
#define DATATYPE_FP5B         "fp5b"
#define DATATYPE_FP6A         "fp6a"
#define DATATYPE_FP79         "fp79"
#define DATATYPE_FP88         "fp88"
#define DATATYPE_FPA6         "fpa6"
#define DATATYPE_FPC4         "fpc4"
#define DATATYPE_FPE2         "fpe2"

#define DATATYPE_SP1E         "sp1e"
#define DATATYPE_SP3C         "sp3c"
#define DATATYPE_SP4B         "sp4b"
#define DATATYPE_SP5A         "sp5a"
#define DATATYPE_SP69         "sp69"
#define DATATYPE_SP78         "sp78"
#define DATATYPE_SP87         "sp87"
#define DATATYPE_SP96         "sp96"
#define DATATYPE_SPB4         "spb4"
#define DATATYPE_SPF0         "spf0"

#define DATATYPE_UINT8        "ui8 "
#define DATATYPE_UINT16       "ui16"
#define DATATYPE_UINT32       "ui32"

#define DATATYPE_SI8          "si8 "
#define DATATYPE_SI16         "si16"

#define DATATYPE_PWM          "{pwm"
#define DATATYPE_LSO          "{lso"
#define DATATYPE_ALA          "{ala"

#define DATATYPE_FLAG         "flag"
#define DATATYPE_CHARSTAR     "ch8*"

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
kern_return_t SMCClose();
kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t *val);
SMCVal_t createEmptyValue();
bool representValue(SMCVal_t value, char* str);