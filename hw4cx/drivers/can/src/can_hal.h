#ifndef __CAN_HAL_H
#define __CAN_HAL_H


enum
{
    CAN_HAL_OK     = +1,
    CAN_HAL_ZERO   =  0,
    CAN_HAL_ERR    = -1,
    CAN_HAL_BUSOFF = -2,
};

enum
{
    CAN_HAL_B125K = 0,
    CAN_HAL_B250K = 1,
    CAN_HAL_B500K = 2,
    CAN_HAL_B1M   = 3,
};


static int  can_hal_open_and_setup_line(int line_n, int speed_n, char **errstr);
static void can_hal_close_line         (int  fd);

static int  can_hal_send_frame         (int  fd,
                                        int  id,   int  dlc,   uint8 *data);
static int  can_hal_recv_frame         (int  fd,
                                        int *id_p, int *dlc_p, uint8 *data);


#endif /* __CAN_HAL_H */
