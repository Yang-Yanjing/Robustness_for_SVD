}
static void VoutDisplayDestroyRender(vout_display_t *vd)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (osys->filters)
        filter_chain_Delete(osys->filters);
}
