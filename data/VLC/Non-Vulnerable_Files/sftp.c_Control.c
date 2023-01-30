}
static int Control( access_t* p_access, int i_query, va_list args )
{
    bool*       pb_bool;
    int64_t*    pi_64;
    switch( i_query )
    {
    case ACCESS_CAN_SEEK:
        pb_bool = (bool*)va_arg( args, bool* );
        *pb_bool = true;
        break;
    case ACCESS_CAN_FASTSEEK:
        pb_bool = (bool*)va_arg( args, bool* );
        *pb_bool = false;
        break;
    case ACCESS_CAN_PAUSE:
    case ACCESS_CAN_CONTROL_PACE:
        pb_bool = (bool*)va_arg( args, bool* );
        *pb_bool = true;
        break;
    case ACCESS_GET_SIZE:
        *va_arg( args, uint64_t * ) = p_access->p_sys->filesize;
        break;
    case ACCESS_GET_PTS_DELAY:
        pi_64 = (int64_t*)va_arg( args, int64_t* );
        *pi_64 = INT64_C(1000)
               * var_InheritInteger( p_access, "network-caching" );
        break;
    case ACCESS_SET_PAUSE_STATE:
        break;
    default:
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}