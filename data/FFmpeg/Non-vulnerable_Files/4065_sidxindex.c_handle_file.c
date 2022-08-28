static int handle_file(struct Tracks *tracks, const char *file)
{
    AVFormatContext *ctx = NULL;
    int err = 0, i, orig_tracks = tracks->nb_tracks;
    char errbuf[50], *ptr;
    struct Track *track;
    err = avformat_open_input(&ctx, file, NULL, NULL);
    if (err < 0) {
        av_strerror(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "Unable to open %s: %s\n", file, errbuf);
        return 1;
    }
    err = avformat_find_stream_info(ctx, NULL);
    if (err < 0) {
        av_strerror(err, errbuf, sizeof(errbuf));
        fprintf(stderr, "Unable to identify %s: %s\n", file, errbuf);
        goto fail;
    }
    if (ctx->nb_streams < 1) {
        fprintf(stderr, "No streams found in %s\n", file);
        goto fail;
    }
    if (ctx->nb_streams > 1)
        tracks->multiple_tracks_per_file = 1;
    for (i = 0; i < ctx->nb_streams; i++) {
        struct Track **temp;
        AVStream *st = ctx->streams[i];
        if (st->codec->bit_rate == 0) {
            fprintf(stderr, "Skipping track %d in %s as it has zero bitrate\n",
                    st->id, file);
            continue;
        }
        track = av_mallocz(sizeof(*track));
        if (!track) {
            err = AVERROR(ENOMEM);
            goto fail;
        }
        temp = av_realloc_array(tracks->tracks, tracks->nb_tracks + 1,
                                sizeof(*tracks->tracks));
        if (!temp) {
            av_free(track);
            err = AVERROR(ENOMEM);
            goto fail;
        }
        tracks->tracks = temp;
        tracks->tracks[tracks->nb_tracks] = track;
        track->name = file;
        if ((ptr = strrchr(file, '/')))
            track->name = ptr + 1;
        track->bitrate   = st->codec->bit_rate;
        track->track_id  = st->id;
        track->timescale = st->time_base.den;
        track->duration  = st->duration;
        track->is_audio  = st->codec->codec_type == AVMEDIA_TYPE_AUDIO;
        track->is_video  = st->codec->codec_type == AVMEDIA_TYPE_VIDEO;
        if (!track->is_audio && !track->is_video) {
            fprintf(stderr,
                    "Track %d in %s is neither video nor audio, skipping\n",
                    track->track_id, file);
            av_freep(&tracks->tracks[tracks->nb_tracks]);
            continue;
        }
        tracks->duration = FFMAX(tracks->duration,
                                 av_rescale_rnd(track->duration, AV_TIME_BASE,
                                                track->timescale, AV_ROUND_UP));
        if (track->is_audio) {
            track->channels    = st->codec->channels;
            track->sample_rate = st->codec->sample_rate;
        }
        if (track->is_video) {
            track->width  = st->codec->width;
            track->height = st->codec->height;
        }
        set_codec_str(st->codec, track->codec_str, sizeof(track->codec_str));
        tracks->nb_tracks++;
    }
    avformat_close_input(&ctx);
    err = find_sidx(tracks, orig_tracks, file);
fail:
    if (ctx)
        avformat_close_input(&ctx);
    return err;
}
