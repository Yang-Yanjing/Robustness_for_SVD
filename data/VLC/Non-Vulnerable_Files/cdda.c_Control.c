 *****************************************************************************/
static int Control( access_t *p_access, int i_query, va_list args )
{
    switch( i_query )
    {
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK:
        case ACCESS_CAN_PAUSE:
        case ACCESS_CAN_CONTROL_PACE:
            *va_arg( args, bool* ) = true;
            break;
        case ACCESS_GET_SIZE:
            *va_arg( args, uint64_t * ) = p_access->p_sys->size;
            break;
        case ACCESS_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) =
                INT64_C(1000) * var_InheritInteger( p_access, "disc-caching" );
            break;
        case ACCESS_SET_PAUSE_STATE:
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
