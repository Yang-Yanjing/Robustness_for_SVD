static int load_encode_and_write(AVAudioFifo *fifo,
                                 AVFormatContext *output_format_context,
                                 AVCodecContext *output_codec_context)
{
    
    AVFrame *output_frame;
    




    const int frame_size = FFMIN(av_audio_fifo_size(fifo),
                                 output_codec_context->frame_size);
    int data_written;
    
    if (init_output_frame(&output_frame, output_codec_context, frame_size))
        return AVERROR_EXIT;
    



    if (av_audio_fifo_read(fifo, (void **)output_frame->data, frame_size) < frame_size) {
        fprintf(stderr, "Could not read data from FIFO\n");
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }
    
    if (encode_audio_frame(output_frame, output_format_context,
                           output_codec_context, &data_written)) {
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }
    av_frame_free(&output_frame);
    return 0;
}
