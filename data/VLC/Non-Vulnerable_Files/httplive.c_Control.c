}
static int Control(stream_t *s, int i_query, va_list args)
{
    stream_sys_t *p_sys = s->p_sys;
    switch (i_query)
    {
        case STREAM_CAN_SEEK:
            *(va_arg (args, bool *)) = hls_MaySeek(s);
            break;
        case STREAM_CAN_CONTROL_PACE:
        case STREAM_CAN_PAUSE:
            *(va_arg (args, bool *)) = true;
            break;
        case STREAM_CAN_FASTSEEK:
            *(va_arg (args, bool *)) = false;
            break;
        case STREAM_GET_POSITION:
            *(va_arg (args, uint64_t *)) = p_sys->playback.offset;
            break;
        case STREAM_SET_PAUSE_STATE:
        {
            bool paused = va_arg (args, unsigned);
            vlc_mutex_lock(&p_sys->lock);
            p_sys->paused = paused;
            vlc_cond_signal(&p_sys->wait);
            vlc_mutex_unlock(&p_sys->lock);
            break;
        }
        case STREAM_SET_POSITION:
            if (hls_MaySeek(s))
            {
                uint64_t pos = (uint64_t)va_arg(args, uint64_t);
                if (segment_Seek(s, pos) == VLC_SUCCESS)
                {
                    p_sys->playback.offset = pos;
                    break;
                }
            }
            return VLC_EGENERIC;
        case STREAM_GET_SIZE:
            *(va_arg (args, uint64_t *)) = GetStreamSize(s);
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
