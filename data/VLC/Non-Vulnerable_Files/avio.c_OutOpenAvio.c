};
int OutOpenAvio(vlc_object_t *object)
{
    sout_access_out_t *access = (sout_access_out_t*)object;
    config_ChainParse( access, "sout-avio-", ppsz_sout_options, access->p_cfg );
    sout_access_out_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->context = NULL;
    /* */
    vlc_init_avformat(object);
    if (!access->psz_path)
        goto error;
    int ret;
#if LIBAVFORMAT_VERSION_MAJOR < 54
    ret = avio_open(&sys->context, access->psz_path, AVIO_FLAG_WRITE);
#else
    AVIOInterruptCB cb = {
        .callback = UrlInterruptCallback,
        .opaque = access,
    };
    AVDictionary *options = NULL;
    char *psz_opts = var_InheritString(access, "sout-avio-options");
    if (psz_opts && *psz_opts) {
        options = vlc_av_get_options(psz_opts);
        free(psz_opts);
    }
    ret = avio_open2(&sys->context, access->psz_path, AVIO_FLAG_WRITE,
                     &cb, &options);
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(options, "", t, AV_DICT_IGNORE_SUFFIX)))
        msg_Err( access, "unknown option \"%s\"", t->key );
    av_dict_free(&options);
#endif
    if (ret < 0) {
        errno = AVUNERROR(ret);
        msg_Err(access, "Failed to open %s", access->psz_path);
        goto error;
    }
#if LIBAVFORMAT_VERSION_MAJOR < 54
    /* We can accept only one active user at any time */
    if (SetupAvioCb(VLC_OBJECT(access))) {
        msg_Err(access, "Module aready in use");
        goto error;
    }
#endif
    access->pf_write = Write;
    access->pf_control = OutControl;
    access->pf_seek = OutSeek;
    access->p_sys = sys;
    return VLC_SUCCESS;
error:
    free(sys);
    return VLC_EGENERIC;
}
