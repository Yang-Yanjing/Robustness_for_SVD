/* */
int OpenAvio(vlc_object_t *object)
{
    access_t *access = (access_t*)object;
    access_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->context = NULL;
    /* We accept:
     * - avio://full_url
     * - url (only a subset of available protocols).
     */
    char *url;
    if (!strcmp(access->psz_access, "avio"))
        url = strdup(access->psz_location);
    else if (asprintf(&url, "%s://%s", access->psz_access,
                      access->psz_location) < 0)
        url = NULL;
    if (!url) {
        free(sys);
        return VLC_ENOMEM;
    }
    /* */
    vlc_init_avformat(object);
    int ret;
#if LIBAVFORMAT_VERSION_MAJOR < 54
    ret = avio_open(&sys->context, url, AVIO_FLAG_READ);
#else
    AVIOInterruptCB cb = {
        .callback = UrlInterruptCallback,
        .opaque = access,
    };
    AVDictionary *options = NULL;
    char *psz_opts = var_InheritString(access, "avio-options");
    if (psz_opts && *psz_opts) {
        options = vlc_av_get_options(psz_opts);
        free(psz_opts);
    }
    ret = avio_open2(&sys->context, url, AVIO_FLAG_READ, &cb, &options);
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(options, "", t, AV_DICT_IGNORE_SUFFIX)))
        msg_Err( access, "unknown option \"%s\"", t->key );
    av_dict_free(&options);
#endif
    if (ret < 0) {
        msg_Err(access, "Failed to open %s: %s", url,
                vlc_strerror_c(AVUNERROR(ret)));
        free(url);
        goto error;
    }
    free(url);
#if LIBAVFORMAT_VERSION_MAJOR < 54
    /* We can accept only one active user at any time */
    if (SetupAvioCb(VLC_OBJECT(access))) {
        msg_Err(access, "Module aready in use");
        avio_close(sys->context);
        goto error;
    }
#endif
    int64_t size = avio_size(sys->context);
    bool seekable;
#if LIBAVFORMAT_VERSION_MAJOR < 54
    seekable = !sys->context->is_streamed;
#else
    seekable = sys->context->seekable;
#endif
    msg_Dbg(access, "%sseekable, size=%"PRIi64, seekable ? "" : "not ", size);
    sys->size = size > 0 ? size : 0;
    /* */
    access_InitFields(access);
    access->pf_read = Read;
    access->pf_block = NULL;
    access->pf_control = Control;
    access->pf_seek = Seek;
    access->p_sys = sys;
    return VLC_SUCCESS;
error:
    free(sys);
    return VLC_EGENERIC;
}
