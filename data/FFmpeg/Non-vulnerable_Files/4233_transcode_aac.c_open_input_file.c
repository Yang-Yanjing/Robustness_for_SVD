static int open_input_file(const char *filename,
                           AVFormatContext **input_format_context,
                           AVCodecContext **input_codec_context)
{
    AVCodec *input_codec;
    int error;
    
    if ((error = avformat_open_input(input_format_context, filename, NULL,
                                     NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
                filename, get_error_text(error));
        *input_format_context = NULL;
        return error;
    }
    
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not open find stream info (error '%s')\n",
                get_error_text(error));
        avformat_close_input(input_format_context);
        return error;
    }
    
    if ((*input_format_context)->nb_streams != 1) {
        fprintf(stderr, "Expected one audio input stream, but found %d\n",
                (*input_format_context)->nb_streams);
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }
    
    if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[0]->codec->codec_id))) {
        fprintf(stderr, "Could not find input codec\n");
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }
    
    if ((error = avcodec_open2((*input_format_context)->streams[0]->codec,
                               input_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open input codec (error '%s')\n",
                get_error_text(error));
        avformat_close_input(input_format_context);
        return error;
    }
    
    *input_codec_context = (*input_format_context)->streams[0]->codec;
    return 0;
}
