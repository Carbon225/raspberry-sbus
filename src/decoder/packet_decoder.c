#include "sbus/packet_decoder.h"

enum sbus_err_t sbus_decode(const uint8_t buf[],
                            struct sbus_packet_t *packet)
{
    if (!packet || !buf) {
        return SBUS_ERR_INVALID_ARG;
    }
    if (buf[0] != SBUS_HEADER || buf[24] != SBUS_END) {
        return SBUS_FAIL;
    }

    uint16_t *channels = packet->channels;
    const uint8_t *payload = buf + 1;
    channels[0]  = (uint16_t)((payload[0]    | payload[1] << 8)                          & 0x07FF);
    channels[1]  = (uint16_t)((payload[1] >> 3 | payload[2] << 5)                        & 0x07FF);
    channels[2]  = (uint16_t)((payload[2] >> 6 | payload[3] << 2 | payload[4] << 10)     & 0x07FF);
    channels[3]  = (uint16_t)((payload[4] >> 1 | payload[5] << 7)                        & 0x07FF);
    channels[4]  = (uint16_t)((payload[5] >> 4 | payload[6] << 4)                        & 0x07FF);
    channels[5]  = (uint16_t)((payload[6] >> 7 | payload[7] << 1 | payload[8] << 9)      & 0x07FF);
    channels[6]  = (uint16_t)((payload[8] >> 2 | payload[9] << 6)                        & 0x07FF);
    channels[7]  = (uint16_t)((payload[9] >> 5 | payload[10] << 3)                       & 0x07FF);
    channels[8]  = (uint16_t)((payload[11]   | payload[12] << 8)                         & 0x07FF);
    channels[9]  = (uint16_t)((payload[12] >> 3 | payload[13] << 5)                      & 0x07FF);
    channels[10] = (uint16_t)((payload[13] >> 6 | payload[14] << 2 | payload[15] << 10)  & 0x07FF);
    channels[11] = (uint16_t)((payload[15] >> 1 | payload[16] << 7)                      & 0x07FF);
    channels[12] = (uint16_t)((payload[16] >> 4 | payload[17] << 4)                      & 0x07FF);
    channels[13] = (uint16_t)((payload[17] >> 7 | payload[18] << 1 | payload[19] << 9)   & 0x07FF);
    channels[14] = (uint16_t)((payload[19] >> 2 | payload[20] << 6)                      & 0x07FF);
    channels[15] = (uint16_t)((payload[20] >> 5 | payload[21] << 3)                      & 0x07FF);

    uint8_t opt = buf[23] & 0xf;
    packet->ch17      = opt & SBUS_OPT_C17;
    packet->ch18      = opt & SBUS_OPT_C18;
    packet->failsafe  = opt & SBUS_OPT_FS;
    packet->frameLost = opt & SBUS_OPT_FL;

    return SBUS_OK;
}

enum sbus_err_t sbus_encode(uint8_t buf[],
                            const struct sbus_packet_t *packet)
{
    if (!packet || !buf) {
        return SBUS_ERR_INVALID_ARG;
    }

    // initialize to 0
    for (int i = 0; i < SBUS_PACKET_SIZE; ++i)
        buf[i] = 0;

    const uint16_t *channels = packet->channels;

    buf[0] = SBUS_HEADER;
    buf[24] = SBUS_END;

    buf[1] = channels[0] & 0xff;
    buf[2] = channels[0] >> 8 & 0b111;
    int currentByte = 2;
    int usedBits = 3; // from LSB

    for (int ch = 1; ch < 16; ch++)
    {
        // while channel not fully encoded
        for (int bitsWritten = 0; bitsWritten < 11;)
        {
            // strip written bits, shift over used bits
            buf[currentByte] |= channels[ch] >> bitsWritten << usedBits & 0xff;

            int hadToWrite = 11 - bitsWritten;
            int couldWrite = 8 - usedBits;

            int wrote = couldWrite;
            if (hadToWrite < couldWrite)
            {
                wrote = hadToWrite;
            }
            else
            {
                currentByte++;
            }

            bitsWritten += wrote;
            usedBits += wrote;
            usedBits %= 8;
        }
    }

    buf[23] = 0;

    if (packet->ch17)
        buf[23] |= SBUS_OPT_C17;

    if (packet->ch18)
        buf[23] |= SBUS_OPT_C18;

    if (packet->failsafe)
        buf[23] |= SBUS_OPT_FS;

    if (packet->frameLost)
        buf[23] |= SBUS_OPT_FL;

    return SBUS_OK;
}
