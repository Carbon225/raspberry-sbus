#ifndef RCDRIVERS_CRSF_SPEC_H
#define RCDRIVERS_CRSF_SPEC_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

// https://github.com/crsf-wg/crsf/wiki

#define CRSF_BAUD (420000)

// [sync] [len] [type] [payload] [crc8]
#define CRSF_MAX_PACKET_SIZE (64)
#define CRSF_MAX_PAYLOAD_SIZE (60)

#define CRSF_NUM_RC_CHANNELS (16)

// [sync] [len] [type] [[ext dest] [ext src] [payload]] [crc8]
// To support tunneling of packets through the CRSF protocol to remote endpoints,
// there is an extended packet format, which includes an extended dest
// and source as part of the payload (see CRSF Addresses for a list).
// In this case, the LEN field is actually PayloadLength+4,
// as the first two bytes are the extended dest and source.
// Maximum payload length is therefore 58 bytes for extended packets.
// All packet types CRSF_FRAMETYPE 0x28 and higher use Extended Packet Format.
#define CRSF_EXTENDED_PACKET_FORMAT_FIRST_TYPE (0x28)

// All serial CRSF packets begin with the CRSF SYNC byte 0xC8,
// with the exception of EdgeTX's outgoing channel/telemetry packets,
// which begin with 0xEE. Due to this, for compatibility,
// new code should support either for the SYNC byte,
// but all transmitted packets MUST begin with 0xC8.
#define CRSF_SYNC_BYTE (0xC8)
#define CRSF_SYNC_BYTE_EDGETX (0xEE)

#define CRSF_PACKET_LEN_BYTE (1)

// Maximum length of a flight mode string including null (CRSF_FRAMETYPE_FLIGHT_MODE)
#define CRSF_MAX_FLIGHT_MODE_LEN (14)

enum crsf_frametype_t
{
    CRSF_FRAMETYPE_GPS = 0x02,
    CRSF_FRAMETYPE_VARIO = 0x07,
    CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
    CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
    CRSF_FRAMETYPE_HEARTBEAT = 0x0B,
    CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
    CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
    CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED = 0x17,
    CRSF_FRAMETYPE_LINK_RX_ID = 0x1C,
    CRSF_FRAMETYPE_LINK_TX_ID = 0x1D,
    CRSF_FRAMETYPE_ATTITUDE = 0x1E,
    CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,
    CRSF_FRAMETYPE_DEVICE_PING = 0x28,
    CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
    CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
    CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
    CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
    CRSF_FRAMETYPE_ELRS_STATUS = 0x2E,
    CRSF_FRAMETYPE_COMMAND = 0x32,
    CRSF_FRAMETYPE_RADIO_ID = 0x3A,
    CRSF_FRAMETYPE_KISS_REQ = 0x78,
    CRSF_FRAMETYPE_KISS_RESP = 0x79,
    CRSF_FRAMETYPE_MSP_REQ = 0x7A,
    CRSF_FRAMETYPE_MSP_RESP = 0x7B,
    CRSF_FRAMETYPE_MSP_WRITE = 0x7C,
    CRSF_FRAMETYPE_DISPLAYPORT_CMD = 0x7D,
    CRSF_FRAMETYPE_ARDUPILOT_RESP = 0x80,
};

struct crsf_payload_rc_channels_packed_t
{
    uint16_t channels[CRSF_NUM_RC_CHANNELS];
};

struct crsf_payload_battery_sensor_t
{
    int16_t voltage;
    int16_t current;
    int32_t used_capacity;
    int8_t remaining;
};

struct crsf_payload_attitude_t
{
    int16_t pitch;
    int16_t roll;
    int16_t yaw;
};

struct crsf_payload_flight_mode_t
{
    char flight_mode[CRSF_MAX_FLIGHT_MODE_LEN];
};

struct crsf_payload_other_t
{
    uint8_t data[CRSF_MAX_PAYLOAD_SIZE];
    uint8_t len;
};

union crsf_payload_t
{
    struct crsf_payload_rc_channels_packed_t rc_channels_packed;
    struct crsf_payload_battery_sensor_t battery_sensor;
    struct crsf_payload_attitude_t attitude;
    struct crsf_payload_flight_mode_t flight_mode;
    struct crsf_payload_other_t other;
};

struct crsf_packet_t
{
    enum crsf_frametype_t frametype;
    union crsf_payload_t payload;
};

#endif // RCDRIVERS_CRSF_SPEC_H
