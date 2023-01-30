 */
static int OpenCommon(vlc_object_t *object, imem_sys_t **sys_ptr, const char *psz_path)
{
    char *tmp;
    /* */
    imem_sys_t *sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    /* Read the user functions */
    tmp = var_InheritString(object, "imem-get");
    if (tmp)
        sys->source.get = (imem_get_t)(intptr_t)strtoll(tmp, NULL, 0);
    free(tmp);
    tmp = var_InheritString(object, "imem-release");
    if (tmp)
        sys->source.release = (imem_release_t)(intptr_t)strtoll(tmp, NULL, 0);
    free(tmp);
    if (!sys->source.get || !sys->source.release) {
        msg_Err(object, "Invalid get/release function pointers");
        free(sys);
        return VLC_EGENERIC;
    }
    tmp = var_InheritString(object, "imem-data");
    if (tmp)
        sys->source.data = (void *)(uintptr_t)strtoull(tmp, NULL, 0);
    free(tmp);
    /* Now we can parse the MRL (get/release must not be parsed to avoid
     * security risks) */
    if (*psz_path)
        ParseMRL(object, psz_path);
    sys->source.cookie = var_InheritString(object, "imem-cookie");
    msg_Dbg(object, "Using get(%p), release(%p), data(%p), cookie(%s)",
            sys->source.get, sys->source.release, sys->source.data,
            sys->source.cookie ? sys->source.cookie : "(null)");
    /* */
    sys->dts       = 0;
    sys->deadline  = VLC_TS_INVALID;
    *sys_ptr = sys;
    return VLC_SUCCESS;
}
