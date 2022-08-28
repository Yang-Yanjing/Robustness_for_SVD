static int handle_file(struct Tracks *tracks, const char *file, int split,
                       int ismf, const char *basename,
                       const char* output_prefix)
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
        temp = av_realloc_array(tracks->tracks,
                                tracks->nb_tracks + 1,
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
            if (tracks->audio_track < 0)
                tracks->audio_track = tracks->nb_tracks;
            tracks->nb_audio_tracks++;
            track->channels    = st->codec->channels;
            track->sample_rate = st->codec->sample_rate;
            if (st->codec->codec_id == AV_CODEC_ID_AAC) {
                track->fourcc    = "AACL";
                track->tag       = 255;
                track->blocksize = 4;
            } else if (st->codec->codec_id == AV_CODEC_ID_WMAPRO) {
                track->fourcc    = "WMAP";
                track->tag       = st->codec->codec_tag;
                track->blocksize = st->codec->block_align;
            }
            get_private_data(track, st->codec);
        }
        if (track->is_video) {
            if (tracks->video_track < 0)
                tracks->video_track = tracks->nb_tracks;
            tracks->nb_video_tracks++;
            track->width  = st->codec->width;
            track->height = st->codec->height;
            if (st->codec->codec_id == AV_CODEC_ID_H264)
                track->fourcc = "H264";
            else if (st->codec->codec_id == AV_CODEC_ID_VC1)
                track->fourcc = "WVC1";
            get_video_private_data(track, st->codec);
        }
        tracks->nb_tracks++;
    }
    avformat_close_input(&ctx);
    err = read_mfra(tracks, orig_tracks, file, split, ismf, basename,
                    output_prefix);
fail:
    if (ctx)
        avformat_close_input(&ctx);
    return err;
}
