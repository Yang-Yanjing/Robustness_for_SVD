static int get_samples_per_packet(MOVTrack *track)
{
    int i, first_duration;

    
    if (!track->audio_vbr)
        return 1;
    
    if (!track->entry)
        return 0;
    first_duration = get_cluster_duration(track, 0);
    for (i = 1; i < track->entry; i++) {
        if (get_cluster_duration(track, i) != first_duration)
            return 0;
    }
    return first_duration;
}
