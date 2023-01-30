}
static void dmx_region_delete(struct dmx_region_t *dmx_region,
                DISPMANX_UPDATE_HANDLE_T update)
{
    vc_dispmanx_element_remove(update, dmx_region->element);
    vc_dispmanx_resource_delete(dmx_region->resource);
    free(dmx_region);
}
