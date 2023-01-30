 *****************************************************************************/
int DirControl( access_t *p_access, int i_query, va_list args )
{
    switch( i_query )
    {
        case ACCESS_CAN_SEEK:
        case ACCESS_CAN_FASTSEEK:
            *va_arg( args, bool* ) = false;
            break;
        case ACCESS_CAN_PAUSE:
        case ACCESS_CAN_CONTROL_PACE:
            *va_arg( args, bool* ) = true;
            break;
        case ACCESS_GET_PTS_DELAY:
            *va_arg( args, int64_t * ) = DEFAULT_PTS_DELAY * 1000;
            break;
        case ACCESS_GET_CONTENT_TYPE:
            *va_arg( args, char** ) = strdup("application/xspf+xml");
            return VLC_SUCCESS;
        default:
            return VLC_EGENERIC;
    }
    (void) p_access;
    return VLC_SUCCESS;
}
