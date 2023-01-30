}
static void dmx_region_update(struct dmx_region_t *dmx_region,
                DISPMANX_UPDATE_HANDLE_T update, picture_t *picture)
{
    vc_dispmanx_resource_write_data(dmx_region->resource, VC_IMAGE_RGBA32,
                    picture->p[0].i_pitch, picture->p[0].p_pixels, &dmx_region->bmp_rect);
    vc_dispmanx_element_change_source(update, dmx_region->element, dmx_region->resource);
    dmx_region->picture = picture;
}
