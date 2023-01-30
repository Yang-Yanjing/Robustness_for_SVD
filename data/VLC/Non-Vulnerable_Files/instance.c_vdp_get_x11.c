 **/
VdpStatus vdp_get_x11(const char *display_name, int snum,
                      vdp_t **restrict vdpp, VdpDevice *restrict devicep)
{
    vdp_instance_t *vi, *vi2;
    VdpStatus err = VDP_STATUS_RESOURCES;
    if (display_name == NULL)
    {
        display_name = getenv("DISPLAY");
        if (display_name == NULL)
            return VDP_STATUS_ERROR;
    }
    pthread_mutex_lock(&lock);
    vi = vdp_instance_lookup(display_name, snum);
    pthread_mutex_unlock(&lock);
    if (vi != NULL)
        goto found;
    err = vdp_instance_create(display_name, snum, &vi);
    if (err != VDP_STATUS_OK)
        return err;
    pthread_mutex_lock(&lock);
    vi2 = vdp_instance_lookup(display_name, snum);
    if (unlikely(vi2 != NULL))
    {   /* Another thread created the instance (race condition corner case) */
        pthread_mutex_unlock(&lock);
        vdp_instance_destroy(vi);
        vi = vi2;
    }
    else
    {
        vi->next = list;
        list = vi;
        pthread_mutex_unlock(&lock);
    }
found:
    *vdpp = vi->vdp;
    *devicep = vi->device;
    return VDP_STATUS_OK;
}
