static av_cold int read_header(AVFormatContext *ctx)
{
    CDIOContext *s = ctx->priv_data;
    AVStream *st;
    int ret, i;
    char *err = NULL;
    if (!(st = avformat_new_stream(ctx, NULL)))
        return AVERROR(ENOMEM);
    s->drive = cdio_cddap_identify(ctx->filename, CDDA_MESSAGE_LOGIT, &err);
    if (!s->drive) {
        av_log(ctx, AV_LOG_ERROR, "Could not open drive %s.\n", ctx->filename);
        return AVERROR(EINVAL);
    }
    if (err) {
        av_log(ctx, AV_LOG_VERBOSE, "%s\n", err);
        free(err);
    }
    if ((ret = cdio_cddap_open(s->drive)) < 0 || !s->drive->opened) {
        av_log(ctx, AV_LOG_ERROR, "Could not open disk in drive %s.\n", ctx->filename);
        return AVERROR(EINVAL);
    }
    cdio_cddap_verbose_set(s->drive, CDDA_MESSAGE_LOGIT, CDDA_MESSAGE_LOGIT);
    if (s->speed)
        cdio_cddap_speed_set(s->drive, s->speed);
    s->paranoia = cdio_paranoia_init(s->drive);
    if (!s->paranoia) {
        av_log(ctx, AV_LOG_ERROR, "Could not init paranoia.\n");
        return AVERROR(EINVAL);
    }
    cdio_paranoia_modeset(s->paranoia, s->paranoia_mode);
    st->codec->codec_type      = AVMEDIA_TYPE_AUDIO;
    if (s->drive->bigendianp)
        st->codec->codec_id    = AV_CODEC_ID_PCM_S16BE;
    else
        st->codec->codec_id    = AV_CODEC_ID_PCM_S16LE;
    st->codec->sample_rate     = 44100;
    st->codec->channels        = 2;
    if (s->drive->audio_last_sector != CDIO_INVALID_LSN &&
        s->drive->audio_first_sector != CDIO_INVALID_LSN)
        st->duration           = s->drive->audio_last_sector - s->drive->audio_first_sector;
    else if (s->drive->tracks)
        st->duration = s->drive->disc_toc[s->drive->tracks].dwStartSector;
    avpriv_set_pts_info(st, 64, CDIO_CD_FRAMESIZE_RAW, 2*st->codec->channels*st->codec->sample_rate);
    for (i = 0; i < s->drive->tracks; i++) {
        char title[16];
        snprintf(title, sizeof(title), "track %02d", s->drive->disc_toc[i].bTrack);
        avpriv_new_chapter(ctx, i, st->time_base, s->drive->disc_toc[i].dwStartSector,
                       s->drive->disc_toc[i+1].dwStartSector, title);
    }
    s->last_sector = cdio_cddap_disc_lastsector(s->drive);
    return 0;
}
