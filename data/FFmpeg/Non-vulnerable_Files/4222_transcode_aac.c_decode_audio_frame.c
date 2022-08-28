static int decode_audio_frame(AVFrame *frame,
                              AVFormatContext *input_format_context,
                              AVCodecContext *input_codec_context,
                              int *data_present, int *finished)
{
    
    AVPacket input_packet;
    int error;
    init_packet(&input_packet);
    
    if ((error = av_read_frame(input_format_context, &input_packet)) < 0) {
        
        if (error == AVERROR_EOF)
            *finished = 1;
        else {
            fprintf(stderr, "Could not read frame (error '%s')\n",
                    get_error_text(error));
            return error;
        }
    }
    





    if ((error = avcodec_decode_audio4(input_codec_context, frame,
                                       data_present, &input_packet)) < 0) {
        fprintf(stderr, "Could not decode frame (error '%s')\n",
                get_error_text(error));
        av_free_packet(&input_packet);
        return error;
    }
    



    if (*finished && *data_present)
        *finished = 0;
    av_free_packet(&input_packet);
    return 0;
}
