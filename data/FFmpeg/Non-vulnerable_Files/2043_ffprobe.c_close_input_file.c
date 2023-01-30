static void close_input_file(InputFile *ifile)
{
    int i;
    
    for (i = 0; i < ifile->nb_streams; i++)
        if (ifile->streams[i].st->codecpar->codec_id != AV_CODEC_ID_NONE)
            avcodec_free_context(&ifile->streams[i].dec_ctx);
    av_freep(&ifile->streams);
    ifile->nb_streams = 0;
    avformat_close_input(&ifile->fmt_ctx);
}
