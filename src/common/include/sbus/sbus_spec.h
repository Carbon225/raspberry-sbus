#ifndef RPISBUS_SBUS_SPEC_H
#define RPISBUS_SBUS_SPEC_H

#define SBUS_BAUD (100000)

#define SBUS_NUM_CHANNELS (16)
#define SBUS_PACKET_SIZE (25)
#define SBUS_HEADER (0x0f)
#define SBUS_END (0x00)

#define SBUS_OPT_C17 (0b0001)
#define SBUS_OPT_C18 (0b0010)
#define SBUS_OPT_FS  (0b1000)
#define SBUS_OPT_FL  (0b0100)

#endif
