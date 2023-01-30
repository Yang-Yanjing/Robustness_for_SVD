 *****************************************************************************/
static int Control( access_t *p_access, int i_query, va_list args )
{
    access_sys_t *p_sys = p_access->p_sys;
    bool   *pb_bool;
    bool    b_bool;
    int64_t      *pi_64;
    int          i_int;
    switch( i_query )
    {
        case ACCESS_CAN_SEEK:
            pb_bool = (bool*)va_arg( args, bool* );
            *pb_bool = !p_sys->b_broadcast;
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
        {
            uint64_t *s = va_arg( args, uint64_t * );
            *s = p_sys->b_broadcast ? 0 : p_sys->asfh.i_file_size;
            return VLC_SUCCESS;
        }
        case ACCESS_GET_PTS_DELAY:
            pi_64 = (int64_t*)va_arg( args, int64_t * );
            *pi_64 = INT64_C(1000)
                   * var_InheritInteger( p_access, "network-caching" );
            break;
        case ACCESS_GET_PRIVATE_ID_STATE:
            i_int = (int)va_arg( args, int );
            pb_bool = (bool *)va_arg( args, bool * );
            if( (i_int < 0) || (i_int > 127) )
                return VLC_EGENERIC;
            *pb_bool =  p_sys->asfh.stream[i_int].i_selected ? true : false;
            break;
        case ACCESS_SET_PRIVATE_ID_STATE:
        {
            i_int = (int)va_arg( args, int );
            b_bool = (bool)va_arg( args, int );
            int i_cat;
            if( i_int > 127 )
                return VLC_EGENERIC;
            else if ( i_int < 0 )
            {
                /* Deselecting all ES in this category */
                assert( !b_bool );
                i_cat = -1 * i_int;
                if ( i_cat > ES_CATEGORY_COUNT )
                    return VLC_EGENERIC;
            }
            else
            {
                /* Chose another ES */
                assert( b_bool );
                i_cat = p_sys->asfh.stream[i_int].i_cat;
            }
            for ( int i=0; i< 128; i++ )
            {
                /* First unselect all streams from the same cat */
                if ( i_cat == p_sys->asfh.stream[i].i_cat )
                    p_sys->asfh.stream[i].i_selected = false;
            }
            if ( i_int > 0 )
                p_sys->asfh.stream[i_int].i_selected = true;
            Stop( p_access );
            Seek( p_access, p_access->info.i_pos );
            return VLC_SUCCESS;
        }
        case ACCESS_SET_PAUSE_STATE:
            b_bool = (bool)va_arg( args, int );
            if( b_bool )
                Stop( p_access );
            else
                Seek( p_access, p_access->info.i_pos );
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
