}
picture_t *vout_FilterDisplay(vout_display_t *vd, picture_t *picture)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    assert(osys->filters);
    if (filter_chain_GetLength(osys->filters) <= 0) {
        picture_Release(picture);
        return NULL;
    }
    return filter_chain_VideoFilter(osys->filters, picture);
}
