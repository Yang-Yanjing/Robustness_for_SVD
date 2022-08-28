static int write_fragments(struct Tracks *tracks, int start_index,
                           AVIOContext *in, const char *basename,
                           int split, int ismf, const char* output_prefix)
{
    char dirname[2048], filename[2048], idxname[2048];
    int i, j, ret = 0, fragment_ret;
    FILE* out = NULL;
    if (ismf) {
        snprintf(idxname, sizeof(idxname), "%s%s.ismf", output_prefix, basename);
        out = fopen(idxname, "w");
        if (!out) {
            ret = AVERROR(errno);
            perror(idxname);
            goto fail;
        }
    }
    for (i = start_index; i < tracks->nb_tracks; i++) {
        struct Track *track = tracks->tracks[i];
        const char *type    = track->is_video ? "video" : "audio";
        snprintf(dirname, sizeof(dirname), "%sQualityLevels(%d)", output_prefix, track->bitrate);
        if (split) {
            if (mkdir(dirname, 0777) == -1 && errno != EEXIST) {
                ret = AVERROR(errno);
                perror(dirname);
                goto fail;
            }
        }
        for (j = 0; j < track->chunks; j++) {
            snprintf(filename, sizeof(filename), "%s/Fragments(%s=%"PRId64")",
                     dirname, type, track->offsets[j].time);
            avio_seek(in, track->offsets[j].offset, SEEK_SET);
            if (ismf)
                fprintf(out, "%s %"PRId64, filename, avio_tell(in));
            if (split)
                fragment_ret = write_fragment(filename, in);
            else
                fragment_ret = skip_fragment(in);
            if (ismf)
                fprintf(out, " %"PRId64"\n", avio_tell(in));
            if (fragment_ret != 0) {
                fprintf(stderr, "failed fragment %d in track %d (%s)\n", j,
                        track->track_id, track->name);
                ret = fragment_ret;
            }
        }
    }
fail:
    if (out)
        fclose(out);
    return ret;
}
