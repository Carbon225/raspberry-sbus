#ifndef RPISBUS_SBUS_ERROR_H
#define RPISBUS_SBUS_ERROR_H

enum sbus_err_t
{
    SBUS_OK = 0,
    SBUS_FAIL = -1,
    SBUS_ERR_TCGETS2 = -2,
    SBUS_ERR_TCSETS2 = -3,
    SBUS_ERR_OPEN = -4,
    SBUS_ERR_INVALID_ARG = -5,
    SBUS_ERR_DESYNC = -6,
};

#endif
