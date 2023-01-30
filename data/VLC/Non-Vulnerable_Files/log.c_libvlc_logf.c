}
static void libvlc_logf (void *data, int level, const vlc_log_t *item,
                         const char *fmt, va_list ap)
{
    libvlc_instance_t *inst = data;
    switch (level)
    {
        case VLC_MSG_INFO: level = LIBVLC_NOTICE;  break;
        case VLC_MSG_ERR:  level = LIBVLC_ERROR;   break;
        case VLC_MSG_WARN: level = LIBVLC_WARNING; break;
        case VLC_MSG_DBG:  level = LIBVLC_DEBUG;   break;
    }
    inst->log.cb (inst->log.data, level, item, fmt, ap);
}
