 *****************************************************************************/
static void DateTimeManage( cam_t * p_cam, int i_session_id )
{
    date_time_t *p_date =
        (date_time_t *)p_cam->p_sessions[i_session_id - 1].p_sys;
    if ( p_date->i_interval
          && mdate() > p_date->i_last + (mtime_t)p_date->i_interval * 1000000 )
    {
        DateTimeSend( p_cam, i_session_id );
    }
}
