static int read_tfra(struct Tracks *tracks, int start_index, AVIOContext *f)
{
    int ret = AVERROR_EOF, track_id;
    int version, fieldlength, i, j;
    int64_t pos   = avio_tell(f);
    uint32_t size = avio_rb32(f);
    struct Track *track = NULL;
    if (avio_rb32(f) != MKBETAG('t', 'f', 'r', 'a'))
        goto fail;
    version = avio_r8(f);
    avio_rb24(f);
    track_id = avio_rb32(f); 
    for (i = start_index; i < tracks->nb_tracks && !track; i++)
        if (tracks->tracks[i]->track_id == track_id)
            track = tracks->tracks[i];
    if (!track) {
        
        ret = 0;
        goto fail;
    }
    fieldlength = avio_rb32(f);
    track->chunks  = avio_rb32(f);
    track->offsets = av_mallocz_array(track->chunks, sizeof(*track->offsets));
    if (!track->offsets) {
        track->chunks = 0;
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    
    
    for (i = 0; i < track->chunks; i++) {
        if (version == 1) {
            track->offsets[i].time   = avio_rb64(f);
            track->offsets[i].offset = avio_rb64(f);
        } else {
            track->offsets[i].time   = avio_rb32(f);
            track->offsets[i].offset = avio_rb32(f);
        }
        for (j = 0; j < ((fieldlength >> 4) & 3) + 1; j++)
            avio_r8(f);
        for (j = 0; j < ((fieldlength >> 2) & 3) + 1; j++)
            avio_r8(f);
        for (j = 0; j < ((fieldlength >> 0) & 3) + 1; j++)
            avio_r8(f);
        if (i > 0)
            track->offsets[i - 1].duration = track->offsets[i].time -
                                             track->offsets[i - 1].time;
    }
    if (track->chunks > 0) {
        track->offsets[track->chunks - 1].duration = track->offsets[0].time +
                                                     track->duration -
                                                     track->offsets[track->chunks - 1].time;
    }
    
    for (i = 0; i < track->chunks; i++) {
        int64_t duration = read_moof_duration(f, track->offsets[i].offset);
        if (duration > 0 && abs(duration - track->offsets[i].duration) > 3) {
            
            
            track->offsets[i].duration = duration;
        }
    }
    if (track->chunks > 0) {
        if (track->offsets[track->chunks - 1].duration <= 0) {
            fprintf(stderr, "Calculated last chunk duration for track %d "
                    "was non-positive (%"PRId64"), probably due to missing "
                    "fragments ", track->track_id,
                    track->offsets[track->chunks - 1].duration);
            if (track->chunks > 1) {
                track->offsets[track->chunks - 1].duration =
                    track->offsets[track->chunks - 2].duration;
            } else {
                track->offsets[track->chunks - 1].duration = 1;
            }
            fprintf(stderr, "corrected to %"PRId64"\n",
                    track->offsets[track->chunks - 1].duration);
            track->duration = track->offsets[track->chunks - 1].time +
                              track->offsets[track->chunks - 1].duration -
                              track->offsets[0].time;
            fprintf(stderr, "Track duration corrected to %"PRId64"\n",
                    track->duration);
        }
    }
    ret = 0;
fail:
    avio_seek(f, pos + size, SEEK_SET);
    return ret;
}
