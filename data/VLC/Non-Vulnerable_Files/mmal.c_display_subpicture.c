}
static void display_subpicture(vout_display_t *vd, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    struct dmx_region_t **dmx_region = &sys->dmx_region;
    struct dmx_region_t *unused_dmx_region;
    DISPMANX_UPDATE_HANDLE_T update = 0;
    picture_t *picture;
    video_format_t *fmt;
    struct dmx_region_t *dmx_region_next;
    if(subpicture) {
        subpicture_region_t *region = subpicture->p_region;
        while(region) {
            picture = region->p_picture;
            fmt = &region->fmt;
            if(!*dmx_region) {
                if(!update)
                    update = vc_dispmanx_update_start(10);
                *dmx_region = dmx_region_new(vd, update, region);
            } else if(((*dmx_region)->bmp_rect.width != (int32_t)fmt->i_visible_width) ||
                    ((*dmx_region)->bmp_rect.height != (int32_t)fmt->i_visible_height) ||
                    ((*dmx_region)->pos_x != region->i_x) ||
                    ((*dmx_region)->pos_y != region->i_y) ||
                    ((*dmx_region)->alpha.opacity != (uint32_t)region->i_alpha)) {
                dmx_region_next = (*dmx_region)->next;
                if(!update)
                    update = vc_dispmanx_update_start(10);
                dmx_region_delete(*dmx_region, update);
                *dmx_region = dmx_region_new(vd, update, region);
                (*dmx_region)->next = dmx_region_next;
            } else if((*dmx_region)->picture != picture) {
                if(!update)
                    update = vc_dispmanx_update_start(10);
                dmx_region_update(*dmx_region, update, picture);
            }
            dmx_region = &(*dmx_region)->next;
            region = region->p_next;
        }
    }
    /* Remove remaining regions */
    unused_dmx_region = *dmx_region;
    while(unused_dmx_region) {
        dmx_region_next = unused_dmx_region->next;
        if(!update)
            update = vc_dispmanx_update_start(10);
        dmx_region_delete(unused_dmx_region, update);
        unused_dmx_region = dmx_region_next;
    }
    *dmx_region = NULL;
    if(update)
        vc_dispmanx_update_submit_sync(update);
}
