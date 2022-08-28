static void clean_tracks(struct Tracks *tracks)
{
    int i;
    for (i = 0; i < tracks->nb_tracks; i++) {
        av_freep(&tracks->tracks[i]);
    }
    av_freep(&tracks->tracks);
    tracks->nb_tracks = 0;
}
