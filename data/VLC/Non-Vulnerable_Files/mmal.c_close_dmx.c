}
static void close_dmx(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(10);
    struct dmx_region_t *dmx_region = sys->dmx_region;
    struct dmx_region_t *dmx_region_next;
    while(dmx_region) {
        dmx_region_next = dmx_region->next;
        dmx_region_delete(dmx_region, update);
        dmx_region = dmx_region_next;
    }
    vc_dispmanx_update_submit_sync(update);
    sys->dmx_region = NULL;
    show_background(vd, false);
    vc_dispmanx_display_close(sys->dmx_handle);
    sys->dmx_handle = DISPMANX_NO_HANDLE;
}
