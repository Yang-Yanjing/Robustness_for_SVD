static int read_decode_convert_and_store(AVAudioFifo *fifo,
                                         AVFormatContext *input_format_context,
                                         AVCodecContext *input_codec_context,
                                         AVCodecContext *output_codec_context,
                                         SwrContext *resampler_context,
                                         int *finished)
{
    
    AVFrame *input_frame = NULL;
    
    uint8_t **converted_input_samples = NULL;
    int data_present;
    int ret = AVERROR_EXIT;
    
    if (init_input_frame(&input_frame))
        goto cleanup;
    
    if (decode_audio_frame(input_frame, input_format_context,
                           input_codec_context, &data_present, finished))
        goto cleanup;
    




    if (*finished && !data_present) {
        ret = 0;
        goto cleanup;
    }
    
    if (data_present) {
        
        if (init_converted_samples(&converted_input_samples, output_codec_context,
                                   input_frame->nb_samples))
            goto cleanup;
        



        if (convert_samples((const uint8_t**)input_frame->extended_data, converted_input_samples,
                            input_frame->nb_samples, resampler_context))
            goto cleanup;
        
        if (add_samples_to_fifo(fifo, converted_input_samples,
                                input_frame->nb_samples))
            goto cleanup;
        ret = 0;
    }
    ret = 0;
cleanup:
    if (converted_input_samples) {
        av_freep(&converted_input_samples[0]);
        free(converted_input_samples);
    }
    av_frame_free(&input_frame);
    return ret;
}
