static int init_output_frame(AVFrame **frame,
                             AVCodecContext *output_codec_context,
                             int frame_size)
{
    int error;
    
    if (!(*frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate output frame\n");
        return AVERROR_EXIT;
    }
    






    (*frame)->nb_samples     = frame_size;
    (*frame)->channel_layout = output_codec_context->channel_layout;
    (*frame)->format         = output_codec_context->sample_fmt;
    (*frame)->sample_rate    = output_codec_context->sample_rate;
    



    if ((error = av_frame_get_buffer(*frame, 0)) < 0) {
        fprintf(stderr, "Could allocate output frame samples (error '%s')\n",
                get_error_text(error));
        av_frame_free(frame);
        return error;
    }
    return 0;
}
