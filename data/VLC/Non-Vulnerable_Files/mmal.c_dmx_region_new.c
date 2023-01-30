}
static struct dmx_region_t *dmx_region_new(vout_display_t *vd,
                DISPMANX_UPDATE_HANDLE_T update, subpicture_region_t *region)
{
    vout_display_sys_t *sys = vd->sys;
    video_format_t *fmt = &region->fmt;
    struct dmx_region_t *dmx_region = malloc(sizeof(struct dmx_region_t));
    uint32_t image_handle;
    dmx_region->pos_x = region->i_x;
    dmx_region->pos_y = region->i_y;
    vc_dispmanx_rect_set(&dmx_region->bmp_rect, 0, 0, fmt->i_visible_width,
                    fmt->i_visible_height);
    vc_dispmanx_rect_set(&dmx_region->src_rect, 0, 0, fmt->i_visible_width << 16,
                    fmt->i_visible_height << 16);
    vc_dispmanx_rect_set(&dmx_region->dst_rect, region->i_x, region->i_y,
                    fmt->i_visible_width, fmt->i_visible_height);
    dmx_region->resource = vc_dispmanx_resource_create(VC_IMAGE_RGBA32,
                    dmx_region->bmp_rect.width | (region->p_picture->p[0].i_pitch << 16),
                    dmx_region->bmp_rect.height | (dmx_region->bmp_rect.height << 16),
                    &image_handle);
    vc_dispmanx_resource_write_data(dmx_region->resource, VC_IMAGE_RGBA32,
                    region->p_picture->p[0].i_pitch,
                    region->p_picture->p[0].p_pixels, &dmx_region->bmp_rect);
    dmx_region->alpha.flags = DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_MIX;
    dmx_region->alpha.opacity = region->i_alpha;
    dmx_region->alpha.mask = DISPMANX_NO_HANDLE;
    dmx_region->element = vc_dispmanx_element_add(update, sys->dmx_handle,
                    sys->layer + 1, &dmx_region->dst_rect, dmx_region->resource,
                    &dmx_region->src_rect, DISPMANX_PROTECTION_NONE,
                    &dmx_region->alpha, NULL, VC_IMAGE_ROT0);
    dmx_region->next = NULL;
    dmx_region->picture = region->p_picture;
    return dmx_region;
}
