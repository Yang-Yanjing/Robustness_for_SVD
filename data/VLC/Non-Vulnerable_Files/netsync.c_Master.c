}
static void *Master(void *handle)
{
    intf_thread_t *intf = handle;
    intf_sys_t *sys = intf->p_sys;
    for (;;) {
        struct pollfd ufd = { .fd = sys->fd, .events = POLLIN, };
        uint64_t data[2];
        if (poll(&ufd, 1, -1) < 0)
            continue;
        /* We received something */
        struct sockaddr_storage from;
        unsigned struct_size = sizeof(from);
        recvfrom(sys->fd, data, sizeof(data), 0,
                 (struct sockaddr*)&from, &struct_size);
        mtime_t master_system = GetPcrSystem(sys->input);
        if (master_system < 0)
            continue;
        data[0] = hton64(mdate());
        data[1] = hton64(master_system);
        /* Reply to the sender */
        sendto(sys->fd, data, sizeof(data), 0,
               (struct sockaddr *)&from, struct_size);
#if 0
        /* not sure we need the client information to sync,
           since we are the master anyway */
        mtime_t client_system = ntoh64(data[0]);
        msg_Dbg(intf, "Master clockref: %"PRId64" -> %"PRId64", from %s "
                 "(date: %"PRId64")", client_system, master_system,
                 (from.ss_family == AF_INET) ? inet_ntoa(((struct sockaddr_in *)&from)->sin_addr)
                 : "non-IPv4", /*date*/ 0);
#endif
    }
    return NULL;
}
