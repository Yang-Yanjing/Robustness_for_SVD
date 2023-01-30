 */
static int Control (stream_t *stream, int query, va_list args)
{
    stream_sys_t *p_sys = stream->p_sys;
    switch (query)
    {
        case STREAM_CAN_SEEK:
        case STREAM_CAN_FASTSEEK:
            *(va_arg (args, bool *)) = false;
            break;
        case STREAM_CAN_PAUSE:
             *(va_arg (args, bool *)) = p_sys->can_pause;
            break;
        case STREAM_CAN_CONTROL_PACE:
            *(va_arg (args, bool *)) = p_sys->can_pace;
            break;
        case STREAM_GET_POSITION:
            *(va_arg (args, uint64_t *)) = p_sys->offset;
            break;
        case STREAM_GET_SIZE:
            *(va_arg (args, uint64_t *)) = 0;
            break;
        case STREAM_GET_PTS_DELAY:
            *va_arg (args, int64_t *) = p_sys->pts_delay;
            break;
        case STREAM_SET_PAUSE_STATE:
        {
            bool paused = va_arg (args, unsigned);
            vlc_mutex_lock (&p_sys->lock);
            stream_Control (stream->p_source, STREAM_SET_PAUSE_STATE, paused);
            p_sys->paused = paused;
            vlc_cond_signal (&p_sys->wait);
            vlc_mutex_unlock (&p_sys->lock);
            break;
        }
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
