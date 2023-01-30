}
static sap_address_t *AddressCreate (vlc_object_t *obj, const char *group)
{
    int fd = net_ConnectUDP (obj, group, IPPORT_SAP, 255);
    if (fd == -1)
        return NULL;
    sap_address_t *addr = malloc (sizeof (*addr));
    if (addr == NULL)
    {
        net_Close (fd);
        return NULL;
    }
    strlcpy (addr->group, group, sizeof (addr->group));
    addr->fd = fd;
    addr->origlen = sizeof (addr->orig);
    getsockname (fd, (struct sockaddr *)&addr->orig, &addr->origlen);
    addr->interval = var_CreateGetInteger (obj, "sap-interval");
    vlc_mutex_init (&addr->lock);
    vlc_cond_init (&addr->wait);
    addr->session_count = 0;
    addr->first = NULL;
    if (vlc_clone (&addr->thread, RunThread, addr, VLC_THREAD_PRIORITY_LOW))
    {
        msg_Err (obj, "unable to spawn SAP announce thread");
        net_Close (fd);
        free (addr);
        return NULL;
    }
    return addr;
}
