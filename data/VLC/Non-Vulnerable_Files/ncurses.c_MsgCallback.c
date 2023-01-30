}
static void MsgCallback(void *data, int type, const vlc_log_t *msg,
                        const char *format, va_list ap)
{
    intf_sys_t *sys = data;
    char *text;
    if (sys->verbosity < 0
     || sys->verbosity < (type - VLC_MSG_ERR)
     || vasprintf(&text, format, ap) == -1)
        return;
    vlc_mutex_lock(&sys->msg_lock);
    sys->msgs[sys->i_msgs].type = type;
    if (sys->msgs[sys->i_msgs].item != NULL)
        msg_Free(sys->msgs[sys->i_msgs].item);
    sys->msgs[sys->i_msgs].item = msg_Copy(msg);
    free(sys->msgs[sys->i_msgs].msg);
    sys->msgs[sys->i_msgs].msg = text;
    if (++sys->i_msgs == (sizeof sys->msgs / sizeof *sys->msgs))
        sys->i_msgs = 0;
    vlc_mutex_unlock(&sys->msg_lock);
}
