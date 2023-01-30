 */
static int OpenAccess(vlc_object_t *object)
{
    access_t   *access = (access_t *)object;
    imem_sys_t *sys;
    if (OpenCommon(object, &sys, access->psz_location))
        return VLC_EGENERIC;
    if (var_InheritInteger(object, "imem-cat") != 4) {
        CloseCommon(sys);
        return VLC_EGENERIC;
    }
    /* */
    access_InitFields(access);
    access->pf_control = ControlAccess;
    access->pf_read    = NULL;
    access->pf_block   = Block;
    access->pf_seek    = NULL;
    access->p_sys      = (access_sys_t*)sys;
    return VLC_SUCCESS;
}
