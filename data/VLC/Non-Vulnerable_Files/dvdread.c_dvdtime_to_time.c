}
static int64_t dvdtime_to_time( dvd_time_t *dtime, uint8_t still_time )
{
/* Macro to convert Binary Coded Decimal to Decimal */
#define BCD2D(__x__) (((__x__ & 0xf0) >> 4) * 10 + (__x__ & 0x0f))
    double f_fps, f_ms;
    int64_t i_micro_second = 0;
    if (still_time == 0 || still_time == 0xFF)
    {
        i_micro_second += (int64_t)(BCD2D(dtime->hour)) * 60 * 60 * 1000000;
        i_micro_second += (int64_t)(BCD2D(dtime->minute)) * 60 * 1000000;
        i_micro_second += (int64_t)(BCD2D(dtime->second)) * 1000000;
        switch((dtime->frame_u & 0xc0) >> 6)
        {
        case 1:
            f_fps = 25.0;
            break;
        case 3:
            f_fps = 29.97;
            break;
        default:
            f_fps = 2500.0;
            break;
        }
        f_ms = BCD2D(dtime->frame_u&0x3f) * 1000.0 / f_fps;
        i_micro_second += (int64_t)(f_ms * 1000.0);
    }
    else
    {
        i_micro_second = still_time;
        i_micro_second = (int64_t)((double)i_micro_second * 1000000.0);
    }
    return i_micro_second;
}
