static int encode_audio_frame(AVFrame *frame,
                              AVFormatContext *output_format_context,
                              AVCodecContext *output_codec_context,
                              int *data_present)
{
    
    AVPacket output_packet;
    int error;
    init_packet(&output_packet);
    
    if (frame) {
        frame->pts = pts;
        pts += frame->nb_samples;
    }
    



    if ((error = avcodec_encode_audio2(output_codec_context, &output_packet,
                                       frame, data_present)) < 0) {
        fprintf(stderr, "Could not encode frame (error '%s')\n",
                get_error_text(error));
        av_free_packet(&output_packet);
        return error;
    }
    
    if (*data_present) {
        if ((error = av_write_frame(output_format_context, &output_packet)) < 0) {
            fprintf(stderr, "Could not write frame (error '%s')\n",
                    get_error_text(error));
            av_free_packet(&output_packet);
            return error;
        }
        av_free_packet(&output_packet);
    }
    return 0;
}
