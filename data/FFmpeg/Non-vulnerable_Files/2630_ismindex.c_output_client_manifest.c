static void output_client_manifest(struct Tracks *tracks, const char *basename,
                                   const char *output_prefix, int split)
{
    char filename[1000];
    FILE *out;
    int i, j;
    if (split)
        snprintf(filename, sizeof(filename), "%sManifest", output_prefix);
    else
        snprintf(filename, sizeof(filename), "%s%s.ismc", output_prefix, basename);
    out = fopen(filename, "w");
    if (!out) {
        perror(filename);
        return;
    }
    fprintf(out, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(out, "<SmoothStreamingMedia MajorVersion=\"2\" MinorVersion=\"0\" "
                 "Duration=\"%"PRId64 "\">\n", tracks->duration * 10);
    if (tracks->video_track >= 0) {
        struct Track *track = tracks->tracks[tracks->video_track];
        struct Track *first_track = track;
        int index = 0;
        fprintf(out,
                "\t<StreamIndex Type=\"video\" QualityLevels=\"%d\" "
                "Chunks=\"%d\" "
                "Url=\"QualityLevels({bitrate})/Fragments(video={start time})\">\n",
                tracks->nb_video_tracks, track->chunks);
        for (i = 0; i < tracks->nb_tracks; i++) {
            track = tracks->tracks[i];
            if (!track->is_video)
                continue;
            fprintf(out,
                    "\t\t<QualityLevel Index=\"%d\" Bitrate=\"%d\" "
                    "FourCC=\"%s\" MaxWidth=\"%d\" MaxHeight=\"%d\" "
                    "CodecPrivateData=\"",
                    index, track->bitrate, track->fourcc, track->width, track->height);
            for (j = 0; j < track->codec_private_size; j++)
                fprintf(out, "%02X", track->codec_private[j]);
            fprintf(out, "\" />\n");
            index++;
            if (track->chunks != first_track->chunks)
                fprintf(stderr, "Mismatched number of video chunks in %s (id: %d, chunks %d) and %s (id: %d, chunks %d)\n",
                        track->name, track->track_id, track->chunks, first_track->name, first_track->track_id, first_track->chunks);
        }
        print_track_chunks(out, tracks, tracks->video_track, "video");
        fprintf(out, "\t</StreamIndex>\n");
    }
    if (tracks->audio_track >= 0) {
        struct Track *track = tracks->tracks[tracks->audio_track];
        struct Track *first_track = track;
        int index = 0;
        fprintf(out,
                "\t<StreamIndex Type=\"audio\" QualityLevels=\"%d\" "
                "Chunks=\"%d\" "
                "Url=\"QualityLevels({bitrate})/Fragments(audio={start time})\">\n",
                tracks->nb_audio_tracks, track->chunks);
        for (i = 0; i < tracks->nb_tracks; i++) {
            track = tracks->tracks[i];
            if (!track->is_audio)
                continue;
            fprintf(out,
                    "\t\t<QualityLevel Index=\"%d\" Bitrate=\"%d\" "
                    "FourCC=\"%s\" SamplingRate=\"%d\" Channels=\"%d\" "
                    "BitsPerSample=\"16\" PacketSize=\"%d\" "
                    "AudioTag=\"%d\" CodecPrivateData=\"",
                    index, track->bitrate, track->fourcc, track->sample_rate,
                    track->channels, track->blocksize, track->tag);
            for (j = 0; j < track->codec_private_size; j++)
                fprintf(out, "%02X", track->codec_private[j]);
            fprintf(out, "\" />\n");
            index++;
            if (track->chunks != first_track->chunks)
                fprintf(stderr, "Mismatched number of audio chunks in %s and %s\n",
                        track->name, first_track->name);
        }
        print_track_chunks(out, tracks, tracks->audio_track, "audio");
        fprintf(out, "\t</StreamIndex>\n");
    }
    fprintf(out, "</SmoothStreamingMedia>\n");
    fclose(out);
}
