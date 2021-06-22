#include <cstdio>
#include <ctime>
#include "SBUS.h"

// comment to enable/disable printing
//#define LOG_PACKETS
#define LOG_TELEMETRY

SBUS sbus;

void onPacket(sbus_packet_t packet)
{
#ifdef LOG_PACKETS
    static time_t lastPrint = time(nullptr);
    time_t now = time(nullptr);

    // rate limiting
    if (now > lastPrint)
    {
        lastPrint = now;
        printf("ch1: %u\tch2: %u\tch3: %u\tch4: %u\t"
               "ch5: %u\tch6: %u\tch7: %u\tch8: %u\t"
               "ch9: %u\tch10: %u\tch11: %u\tch12: %u\t"
               "ch13: %u\tch14: %u\tch15: %u\tch16: %u\tch17: %u\tch18: %u%s%s\n\r",
               packet.channels[0], packet.channels[1], packet.channels[2], packet.channels[3],
               packet.channels[4], packet.channels[5], packet.channels[6], packet.channels[7],
               packet.channels[8], packet.channels[9], packet.channels[10], packet.channels[11],
               packet.channels[12], packet.channels[13], packet.channels[14], packet.channels[15],
               packet.ch17, packet.ch18,
               packet.frameLost ? "\tFrame lost" : "",
               packet.failsafe ? "\tFailsafe active" : "");
    }
#endif
}

void onTelemetry(sbus_telemetry_t telemetry)
{
#ifdef LOG_TELEMETRY
    printf("Telemetry: type = %d\tdata = %d\n\r", telemetry.type, telemetry.data);
#endif
}

int main()
{
    printf("SBUS2 blocking receiver with telemetry example\n\r");

    sbus.onPacket(onPacket);
    sbus.onTelemetry(onTelemetry);

    sbus_err_t err = sbus.install("/dev/ttyAMA0", true);
    if (err != SBUS_OK)
    {
        fprintf(stderr, "SBUS install error: %d\n\r", err);
        return err;
    }

    // wait for packet
    while ((err = sbus.read()) != SBUS_FAIL)
    {
        if (err == SBUS_ERR_DESYNC)
        {
            fprintf(stderr, "Warning, SBUS desync\n\r");
        }
    }

    fprintf(stderr, "SBUS error: %d\n\r", err);

    return err;
}
