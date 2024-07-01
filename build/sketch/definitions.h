#line 1 "C:\\Users\\benja\\Documents\\Arduino\\EspBleTest\\definitions.h"

#define BYTES_TO_INT16(X)       ((int16_t)X[1] << 8) | (X[0])
#define FLOAT_TO_INT_BITS(X)    *(int32_t*)(&X)
#define BUFFER_OFFSET(B, I)     (B + (I * 4))

#define UPDATE_INTERVAL         20
#define DATA_OUT_SIZE           8
#define DATA_IN_SIZE            3
#define TELEMETRY_STATES        2
