static void print_track_chunks(FILE *out, struct Tracks *tracks, int main,
                               const char *type)
{
    int i, j;
    int64_t pos = 0;
    struct Track *track = tracks->tracks[main];
    int should_print_time_mismatch = 1;
    for (i = 0; i < track->chunks; i++) {
        for (j = main + 1; j < tracks->nb_tracks; j++) {
            if (tracks->tracks[j]->is_audio == track->is_audio) {
                if (track->offsets[i].duration != tracks->tracks[j]->offsets[i].duration) {
                    fprintf(stderr, "Mismatched duration of %s chunk %d in %s (%d) and %s (%d)\n",
                            type, i, track->name, main, tracks->tracks[j]->name, j);
                    should_print_time_mismatch = 1;
                }
                if (track->offsets[i].time != tracks->tracks[j]->offsets[i].time) {
                    if (should_print_time_mismatch)
                        fprintf(stderr, "Mismatched (start) time of %s chunk %d in %s (%d) and %s (%d)\n",
                                type, i, track->name, main, tracks->tracks[j]->name, j);
                    should_print_time_mismatch = 0;
                }
            }
        }
        fprintf(out, "\t\t<c n=\"%d\" d=\"%"PRId64"\" ",
                i, track->offsets[i].duration);
        if (pos != track->offsets[i].time) {
            fprintf(out, "t=\"%"PRId64"\" ", track->offsets[i].time);
            pos = track->offsets[i].time;
        }
        pos += track->offsets[i].duration;
        fprintf(out, "/>\n");
    }
}
