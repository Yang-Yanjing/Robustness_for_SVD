static int find_sidx(struct Tracks *tracks, int start_index,
                     const char *file)
{
    int err = 0;
    AVIOContext *f = NULL;
    int i;
    if ((err = avio_open2(&f, file, AVIO_FLAG_READ, NULL, NULL)) < 0)
        goto fail;
    while (!f->eof_reached) {
        int64_t pos = avio_tell(f);
        int32_t size, tag;
        size = avio_rb32(f);
        tag  = avio_rb32(f);
        if (size < 8)
            break;
        if (tag == MKBETAG('s', 'i', 'd', 'x')) {
            for (i = start_index; i < tracks->nb_tracks; i++) {
                struct Track *track = tracks->tracks[i];
                if (!track->sidx_start) {
                    track->sidx_start  = pos;
                    track->sidx_length = size;
                } else if (pos == track->sidx_start + track->sidx_length) {
                    track->sidx_length = pos + size - track->sidx_start;
                }
            }
        }
        if (avio_seek(f, pos + size, SEEK_SET) != pos + size)
            break;
    }
fail:
    if (f)
        avio_close(f);
    return err;
}
