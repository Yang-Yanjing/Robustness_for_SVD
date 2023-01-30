static int open_output_file(const char *filename,
                            AVCodecContext *input_codec_context,
                            AVFormatContext **output_format_context,
                            AVCodecContext **output_codec_context)
{
    AVIOContext *output_io_context = NULL;
    AVStream *stream               = NULL;
    AVCodec *output_codec          = NULL;
    int error;
    
    if ((error = avio_open(&output_io_context, filename,
                           AVIO_FLAG_WRITE)) < 0) {
        fprintf(stderr, "Could not open output file '%s' (error '%s')\n",
                filename, get_error_text(error));
        return error;
    }
    
    if (!(*output_format_context = avformat_alloc_context())) {
        fprintf(stderr, "Could not allocate output format context\n");
        return AVERROR(ENOMEM);
    }
    
    (*output_format_context)->pb = output_io_context;
    
    if (!((*output_format_context)->oformat = av_guess_format(NULL, filename,
                                                              NULL))) {
        fprintf(stderr, "Could not find output file format\n");
        goto cleanup;
    }
    av_strlcpy((*output_format_context)->filename, filename,
               sizeof((*output_format_context)->filename));
    
    if (!(output_codec = avcodec_find_encoder(AV_CODEC_ID_AAC))) {
        fprintf(stderr, "Could not find an AAC encoder.\n");
        goto cleanup;
    }
    
    if (!(stream = avformat_new_stream(*output_format_context, output_codec))) {
        fprintf(stderr, "Could not create new stream\n");
        error = AVERROR(ENOMEM);
        goto cleanup;
    }
    
    *output_codec_context = stream->codec;
    



    (*output_codec_context)->channels       = OUTPUT_CHANNELS;
    (*output_codec_context)->channel_layout = av_get_default_channel_layout(OUTPUT_CHANNELS);
    (*output_codec_context)->sample_rate    = input_codec_context->sample_rate;
    (*output_codec_context)->sample_fmt     = output_codec->sample_fmts[0];
    (*output_codec_context)->bit_rate       = OUTPUT_BIT_RATE;
    
    (*output_codec_context)->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    
    stream->time_base.den = input_codec_context->sample_rate;
    stream->time_base.num = 1;
    



    if ((*output_format_context)->oformat->flags & AVFMT_GLOBALHEADER)
        (*output_codec_context)->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    
    if ((error = avcodec_open2(*output_codec_context, output_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open output codec (error '%s')\n",
                get_error_text(error));
        goto cleanup;
    }
    return 0;
cleanup:
    avio_closep(&(*output_format_context)->pb);
    avformat_free_context(*output_format_context);
    *output_format_context = NULL;
    return error < 0 ? error : AVERROR_EXIT;
}
