} vdp_instance_t;
static VdpStatus vdp_instance_create(const char *name, int num,
                                     vdp_instance_t **pp)
{
    size_t namelen = (name != NULL) ? (strlen(name) + 1) : 0;
    vdp_instance_t *vi = malloc(sizeof (*vi) + namelen);
    if (unlikely(vi == NULL))
        return VDP_STATUS_RESOURCES;
    vi->display = XOpenDisplay(name);
    if (vi->display == NULL)
    {
        free(vi);
        return VDP_STATUS_ERROR;
    }
    vi->next = NULL;
    if (name != NULL)
    {
        vi->name = (void *)(vi + 1);
        memcpy(vi->name, name, namelen);
    }
    else
        vi->name = NULL;
    if (num >= 0)
        vi->num = num;
    else
        vi->num = XDefaultScreen(vi->display);
    vi->refs = 1;
    VdpStatus err = vdp_create_x11(vi->display, vi->num,
                                   &vi->vdp, &vi->device);
    if (err != VDP_STATUS_OK)
    {
        XCloseDisplay(vi->display);
        free(vi);
        return err;
    }
    *pp = vi;
    return VDP_STATUS_OK;
}
