}
static void show_background(vout_display_t *vd, bool enable)
{
    vout_display_sys_t *sys = vd->sys;
    uint32_t image_ptr, color = 0xFF000000;
    VC_RECT_T dst_rect, src_rect;
    DISPMANX_UPDATE_HANDLE_T update;
    if (enable && !sys->bkg_element) {
        sys->bkg_resource = vc_dispmanx_resource_create(VC_IMAGE_RGBA32, 1, 1,
                        &image_ptr);
        vc_dispmanx_rect_set(&dst_rect, 0, 0, 1, 1);
        vc_dispmanx_resource_write_data(sys->bkg_resource, VC_IMAGE_RGBA32,
                        sizeof(color), &color, &dst_rect);
        vc_dispmanx_rect_set(&src_rect, 0, 0, 1 << 16, 1 << 16);
        vc_dispmanx_rect_set(&dst_rect, 0, 0, 0, 0);
        update = vc_dispmanx_update_start(0);
        sys->bkg_element = vc_dispmanx_element_add(update, sys->dmx_handle,
                        sys->layer - 1, &dst_rect, sys->bkg_resource, &src_rect,
                        DISPMANX_PROTECTION_NONE, NULL, NULL, VC_IMAGE_ROT0);
        vc_dispmanx_update_submit_sync(update);
    } else if (!enable && sys->bkg_element) {
        update = vc_dispmanx_update_start(0);
        vc_dispmanx_element_remove(update, sys->bkg_element);
        vc_dispmanx_resource_delete(sys->bkg_resource);
        vc_dispmanx_update_submit_sync(update);
        sys->bkg_element = DISPMANX_NO_HANDLE;
        sys->bkg_resource = DISPMANX_NO_HANDLE;
    }
}
