}
int RarParse(stream_t *s, int *count, rar_file_t ***file, bool b_extonly)
{
    *count = 0;
    *file = NULL;
    const rar_pattern_t *pattern = FindVolumePattern(s->psz_path, b_extonly);
    int volume_offset = 0;
    char *volume_mrl;
    if (asprintf(&volume_mrl, "%s://%s",
                 s->psz_access, s->psz_path) < 0)
        return VLC_EGENERIC;
    stream_t *vol = s;
    for (;;) {
        /* Skip marker & archive */
        if (IgnoreBlock(vol, RAR_BLOCK_MARKER) ||
            IgnoreBlock(vol, RAR_BLOCK_ARCHIVE)) {
            if (vol != s)
                stream_Delete(vol);
            free(volume_mrl);
            return VLC_EGENERIC;
        }
        /* */
        int has_next = -1;
        for (;;) {
            rar_block_t bk;
            int ret;
            if (PeekBlock(vol, &bk))
                break;
            switch(bk.type) {
            case RAR_BLOCK_END:
                ret = SkipEnd(vol, &bk);
                has_next = ret && (bk.flags & RAR_BLOCK_END_HAS_NEXT);
                break;
            case RAR_BLOCK_FILE:
                ret = SkipFile(vol, count, file, &bk, volume_mrl);
                break;
            default:
                ret = SkipBlock(vol, &bk);
                break;
            }
            if (ret)
                break;
        }
        if (has_next < 0 && *count > 0 && !(*file)[*count -1]->is_complete)
            has_next = 1;
        if (vol != s)
            stream_Delete(vol);
        if (!has_next || !pattern) {
            free(volume_mrl);
            return VLC_SUCCESS;
        }
        /* Open next volume */
        const int volume_index = pattern->start + volume_offset++;
        if (volume_index > pattern->stop) {
            free(volume_mrl);
            return VLC_SUCCESS;
        }
        char *volume_base;
        if (asprintf(&volume_base, "%s://%.*s",
                     s->psz_access,
                     (int)(strlen(s->psz_path) - strlen(pattern->match)), s->psz_path) < 0) {
            free(volume_mrl);
            return VLC_SUCCESS;
        }
        free(volume_mrl);
        if (pattern->start) {
            if (asprintf(&volume_mrl, pattern->format, volume_base, volume_index) < 0)
                volume_mrl = NULL;
        } else {
            if (asprintf(&volume_mrl, pattern->format, volume_base,
                         'r' + volume_index / 100, volume_index % 100) < 0)
                volume_mrl = NULL;
        }
        free(volume_base);
        if (!volume_mrl)
            return VLC_SUCCESS;
        const int s_flags = s->i_flags;
        s->i_flags |= OBJECT_FLAGS_NOINTERACT;
        vol = stream_UrlNew(s, volume_mrl);
        s->i_flags = s_flags;
        if (!vol) {
            free(volume_mrl);
            return VLC_SUCCESS;
        }
    }
}
