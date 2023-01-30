 *****************************************************************************/
static int Control( access_t *p_access, int i_query, va_list args )
{
    access_sys_t *p_sys = p_access->p_sys;
    bool   *pb_bool;
    int64_t      *pi_64;
    switch( i_query )
    {
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = p_sys->b_seekable;
            break;
        case ACCESS_CAN_PAUSE:
        case ACCESS_CAN_CONTROL_PACE:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = p_sys->b_pace_control;
            break;
        case ACCESS_GET_SIZE:
            *va_arg( args, uint64_t * ) = p_sys->p_file_info->size;
            break;
        case ACCESS_GET_PTS_DELAY:
            pi_64 = (int64_t*)va_arg( args, int64_t * );
            *pi_64 = DEFAULT_PTS_DELAY; /* FIXME */
            break;
        case ACCESS_SET_PAUSE_STATE:
            /* Nothing to do */
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
