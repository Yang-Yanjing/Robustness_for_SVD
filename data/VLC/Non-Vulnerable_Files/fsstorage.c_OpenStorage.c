}
static int OpenStorage(vlc_object_t *p_this)
{
    addons_storage_t *p_storage = (addons_storage_t*) p_this;
    p_storage->pf_install = Install;
    p_storage->pf_remove = Remove;
    p_storage->pf_catalog = WriteCatalog;
    return VLC_SUCCESS;
}
