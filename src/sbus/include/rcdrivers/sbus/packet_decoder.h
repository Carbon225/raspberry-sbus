#ifndef RCDRIVERS_PACKET_DECODER_H
#define RCDRIVERS_PACKET_DECODER_H

#include "rcdrivers/errors.h"
#include "rcdrivers/sbus/sbus_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

enum rcdrivers_err_t sbus_decode(const uint8_t buf[], struct sbus_packet_t *packet);

enum rcdrivers_err_t sbus_encode(uint8_t buf[], const struct sbus_packet_t *packet);

#ifdef __cplusplus
}
#endif

#endif
