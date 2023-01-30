}
static int OpenLister(vlc_object_t *p_this)
{
    addons_finder_t *p_finder = (addons_finder_t*) p_this;
    p_finder->pf_find = List;
    p_finder->pf_retrieve = NULL;
    return VLC_SUCCESS;
}
