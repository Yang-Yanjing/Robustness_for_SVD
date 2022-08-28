static int output_mpd(struct Tracks *tracks, const char *filename)
{
    FILE *out;
    int i, j, ret = 0;
    struct Track **adaptation_sets_buf[2] = { NULL };
    struct Track ***adaptation_sets;
    int nb_tracks_buf[2] = { 0 };
    int *nb_tracks;
    int set, nb_sets;
    if (!tracks->multiple_tracks_per_file) {
        adaptation_sets = adaptation_sets_buf;
        nb_tracks = nb_tracks_buf;
        nb_sets = 2;
        for (i = 0; i < 2; i++) {
            adaptation_sets[i] = av_malloc_array(tracks->nb_tracks, sizeof(*adaptation_sets[i]));
            if (!adaptation_sets[i]) {
                ret = AVERROR(ENOMEM);
                goto err;
            }
        }
        for (i = 0; i < tracks->nb_tracks; i++) {
            int set_index = -1;
            if (tracks->tracks[i]->is_video)
                set_index = 0;
            else if (tracks->tracks[i]->is_audio)
                set_index = 1;
            else
                continue;
            adaptation_sets[set_index][nb_tracks[set_index]++] = tracks->tracks[i];
        }
    } else {
        adaptation_sets = &tracks->tracks;
        nb_tracks = &tracks->nb_tracks;
        nb_sets = 1;
    }
    out = fopen(filename, "w");
    if (!out) {
        ret = AVERROR(errno);
        perror(filename);
        return ret;
    }
    fprintf(out, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(out, "<MPD xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                "\txmlns=\"urn:mpeg:dash:schema:mpd:2011\"\n"
                "\txmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
                "\txsi:schemaLocation=\"urn:mpeg:DASH:schema:MPD:2011 http://standards.iso.org/ittf/PubliclyAvailableStandards/MPEG-DASH_schema_files/DASH-MPD.xsd\"\n"
                "\tprofiles=\"urn:mpeg:dash:profile:isoff-on-demand:2011\"\n"
                "\ttype=\"static\"\n");
    fprintf(out, "\tmediaPresentationDuration=\"");
    write_time(out, tracks->duration, 1, AV_ROUND_DOWN);
    fprintf(out, "\"\n");
    fprintf(out, "\tminBufferTime=\"PT5S\">\n");
    fprintf(out, "\t<Period start=\"PT0.0S\">\n");
    for (set = 0; set < nb_sets; set++) {
        if (nb_tracks[set] == 0)
            continue;
        fprintf(out, "\t\t<AdaptationSet segmentAlignment=\"true\">\n");
        if (nb_sets == 1) {
            for (i = 0; i < nb_tracks[set]; i++) {
                struct Track *track = adaptation_sets[set][i];
                if (strcmp(track->name, adaptation_sets[set][0]->name))
                    break;
                fprintf(out, "\t\t\t<ContentComponent id=\"%d\" contentType=\"%s\" />\n", track->track_id, track->is_audio ? "audio" : "video");
            }
        }
        for (i = 0; i < nb_tracks[set]; ) {
            struct Track *first_track = adaptation_sets[set][i];
            int width = 0, height = 0, sample_rate = 0, channels = 0, bitrate = 0;
            fprintf(out, "\t\t\t<Representation id=\"%d\" codecs=\"", i);
            for (j = i; j < nb_tracks[set]; j++) {
                struct Track *track = adaptation_sets[set][j];
                if (strcmp(track->name, first_track->name))
                    break;
                if (track->is_audio) {
                    sample_rate = track->sample_rate;
                    channels = track->channels;
                }
                if (track->is_video) {
                    width = track->width;
                    height = track->height;
                }
                bitrate += track->bitrate;
                if (j > i)
                    fprintf(out, ",");
                fprintf(out, "%s", track->codec_str);
            }
            fprintf(out, "\" mimeType=\"%s/mp4\" bandwidth=\"%d\"",
                    width ? "video" : "audio", bitrate);
            if (width > 0 && height > 0)
                fprintf(out, " width=\"%d\" height=\"%d\"", width, height);
            if (sample_rate > 0)
                fprintf(out, " audioSamplingRate=\"%d\"", sample_rate);
            fprintf(out, ">\n");
            if (channels > 0)
                fprintf(out, "\t\t\t\t<AudioChannelConfiguration schemeIdUri=\"urn:mpeg:dash:23003:3:audio_channel_configuration:2011\" value=\"%d\" />\n", channels);
            fprintf(out, "\t\t\t\t<BaseURL>%s</BaseURL>\n", first_track->name);
            fprintf(out, "\t\t\t\t<SegmentBase indexRange=\"%"PRId64"-%"PRId64"\" />\n", first_track->sidx_start, first_track->sidx_start + first_track->sidx_length - 1);
            fprintf(out, "\t\t\t</Representation>\n");
            i = j;
        }
        fprintf(out, "\t\t</AdaptationSet>\n");
    }
    fprintf(out, "\t</Period>\n");
    fprintf(out, "</MPD>\n");
    fclose(out);
err:
    for (i = 0; i < 2; i++)
        av_free(adaptation_sets_buf[i]);
    return ret;
}
