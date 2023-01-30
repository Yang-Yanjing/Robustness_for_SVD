}
bool vout_IsDisplayFiltered(vout_display_t *vd)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    return osys->filters != NULL;
}
