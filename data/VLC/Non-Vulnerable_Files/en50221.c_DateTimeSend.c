 *****************************************************************************/
static void DateTimeSend( cam_t * p_cam, int i_session_id )
{
    date_time_t *p_date =
        (date_time_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    time_t t = time(NULL);
    struct tm tm_gmt;
    struct tm tm_loc;
    if ( gmtime_r(&t, &tm_gmt) && localtime_r(&t, &tm_loc) )
    {
        int Y = tm_gmt.tm_year;
        int M = tm_gmt.tm_mon + 1;
        int D = tm_gmt.tm_mday;
        int L = (M == 1 || M == 2) ? 1 : 0;
        int MJD = 14956 + D + (int)((Y - L) * 365.25)
                    + (int)((M + 1 + L * 12) * 30.6001);
        uint8_t p_response[7];
#define DEC2BCD(d) (((d / 10) << 4) + (d % 10))
        SetWBE( &p_response[0], MJD );
        p_response[2] = DEC2BCD(tm_gmt.tm_hour);
        p_response[3] = DEC2BCD(tm_gmt.tm_min);
        p_response[4] = DEC2BCD(tm_gmt.tm_sec);
        SetWBE( &p_response[5], tm_loc.tm_gmtoff / 60 );
        APDUSend( p_cam, i_session_id, AOT_DATE_TIME, p_response, 7 );
        p_date->i_last = mdate();
    }
}
