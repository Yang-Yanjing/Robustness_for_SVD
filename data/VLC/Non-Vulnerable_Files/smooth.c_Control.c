}
static int Control( stream_t *s, int i_query, va_list args )
{
    stream_sys_t *p_sys = s->p_sys;
    switch( i_query )
    {
        case STREAM_CAN_SEEK:
            *(va_arg( args, bool * )) = true;
            break;
        case STREAM_CAN_FASTSEEK:
        case STREAM_CAN_PAUSE: /* TODO */
        case STREAM_CAN_CONTROL_PACE:
            *(va_arg( args, bool * )) = false;
            break;
        case STREAM_GET_POSITION:
            *(va_arg( args, uint64_t * )) = p_sys->playback.boffset;
            break;
        case STREAM_SET_POSITION:
            {
                uint64_t pos = (uint64_t)va_arg(args, uint64_t);
                int ret = chunk_Seek(s, pos);
                if( ret == VLC_SUCCESS )
                    break;
                else
                    return VLC_EGENERIC;
            }
        case STREAM_GET_SIZE:
            *(va_arg( args, uint64_t * )) = FAKE_STREAM_SIZE;
            break;
        case STREAM_GET_PTS_DELAY:
            *va_arg (args, int64_t *) = INT64_C(1000) *
                var_InheritInteger(s, "network-caching");
             break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
