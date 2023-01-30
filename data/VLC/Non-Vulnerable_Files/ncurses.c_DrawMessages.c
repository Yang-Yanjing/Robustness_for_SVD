}
static int DrawMessages(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    int l = 0;
    vlc_mutex_lock(&sys->msg_lock);
    int i = sys->i_msgs;
    for(;;) {
        vlc_log_t *msg = sys->msgs[i].item;
        if (msg) {
            if (sys->color)
                color_set(sys->msgs[i].type + C_INFO, NULL);
            MainBoxWrite(sys, l++, "[%s] %s", msg->psz_module, sys->msgs[i].msg);
        }
        if (++i == sizeof sys->msgs / sizeof *sys->msgs)
            i = 0;
        if (i == sys->i_msgs) /* did we loop around the ring buffer ? */
            break;
    }
    vlc_mutex_unlock(&sys->msg_lock);
    if (sys->color)
        color_set(C_DEFAULT, NULL);
    return l;
}
