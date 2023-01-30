}
static void *Slave(void *handle)
{
    intf_thread_t *intf = handle;
    intf_sys_t *sys = intf->p_sys;
    for (;;) {
        struct pollfd ufd = { .fd = sys->fd, .events = POLLIN, };
        uint64_t data[2];
        mtime_t system = GetPcrSystem(sys->input);
        if (system < 0)
            goto wait;
        /* Send clock request to the master */
        data[0] = hton64(system);
        const mtime_t send_date = mdate();
        if (send(sys->fd, data, sizeof(data[0]), 0) <= 0)
            goto wait;
        /* Don't block */
        if (poll(&ufd, 1, sys->timeout) <= 0)
            continue;
        const mtime_t receive_date = mdate();
        if (recv(sys->fd, data, sizeof(data), 0) <= 0)
            goto wait;
        const mtime_t master_date   = ntoh64(data[0]);
        const mtime_t master_system = ntoh64(data[1]);
        const mtime_t diff_date = receive_date -
                                  ((receive_date - send_date) / 2 + master_date);
        if (master_system > 0) {
            int canc = vlc_savecancel();
            mtime_t client_system;
            if (!input_GetPcrSystem(sys->input, &client_system, NULL)) {
                const mtime_t diff_system = client_system - master_system - diff_date;
                if (diff_system != 0) {
                    input_ModifyPcrSystem(sys->input, true, master_system - diff_date);
#if 0
                    msg_Dbg(intf, "Slave clockref: %"PRId64" -> %"PRId64" -> %"PRId64","
                             " clock diff: %"PRId64", diff: %"PRId64"",
                             system, master_system, client_system,
                             diff_system, diff_date);
#endif
                }
            }
            vlc_restorecancel(canc);
        }
    wait:
        msleep(INTF_IDLE_SLEEP);
    }
    return NULL;
}
