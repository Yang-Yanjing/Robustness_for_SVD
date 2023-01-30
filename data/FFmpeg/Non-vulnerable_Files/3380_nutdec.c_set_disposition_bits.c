static void set_disposition_bits(AVFormatContext *avf, char *value,
                                 int stream_id)
{
    int flag = 0, i;
    for (i = 0; ff_nut_dispositions[i].flag; ++i)
        if (!strcmp(ff_nut_dispositions[i].str, value))
            flag = ff_nut_dispositions[i].flag;
    if (!flag)
        av_log(avf, AV_LOG_INFO, "unknown disposition type '%s'\n", value);
    for (i = 0; i < avf->nb_streams; ++i)
        if (stream_id == i || stream_id == -1)
            avf->streams[i]->disposition |= flag;
}
