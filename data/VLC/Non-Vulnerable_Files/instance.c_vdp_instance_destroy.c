}
static void vdp_instance_destroy(vdp_instance_t *vi)
{
    vdp_device_destroy(vi->vdp, vi->device);
    vdp_destroy_x11(vi->vdp);
    XCloseDisplay(vi->display);
    free(vi);
}
