}
static int SplitterPictureNew(video_splitter_t *splitter, picture_t *picture[])
{
    vout_display_sys_t *wsys = splitter->p_owner->wrapper->sys;
    for (int i = 0; i < wsys->count; i++) {
        if (vout_IsDisplayFiltered(wsys->display[i])) {
            /* TODO use a pool ? */
            picture[i] = picture_NewFromFormat(&wsys->display[i]->source);
        } else {
            picture_pool_t *pool = vout_display_Pool(wsys->display[i], 1);
            picture[i] = pool ? picture_pool_Get(pool) : NULL;
        }
        if (!picture[i]) {
            for (int j = 0; j < i; j++)
                picture_Release(picture[j]);
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
