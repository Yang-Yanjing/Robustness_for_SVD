int main(int argc, char **argv)
{
    AVFormatContext *input_format_context = NULL, *output_format_context = NULL;
    AVCodecContext *input_codec_context = NULL, *output_codec_context = NULL;
    SwrContext *resample_context = NULL;
    AVAudioFifo *fifo = NULL;
    int ret = AVERROR_EXIT;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(1);
    }
    
    av_register_all();
    
    if (open_input_file(argv[1], &input_format_context,
                        &input_codec_context))
        goto cleanup;
    
    if (open_output_file(argv[2], input_codec_context,
                         &output_format_context, &output_codec_context))
        goto cleanup;
    
    if (init_resampler(input_codec_context, output_codec_context,
                       &resample_context))
        goto cleanup;
    
    if (init_fifo(&fifo, output_codec_context))
        goto cleanup;
    
    if (write_output_file_header(output_format_context))
        goto cleanup;
    



    while (1) {
        
        const int output_frame_size = output_codec_context->frame_size;
        int finished                = 0;
        






        while (av_audio_fifo_size(fifo) < output_frame_size) {
            



            if (read_decode_convert_and_store(fifo, input_format_context,
                                              input_codec_context,
                                              output_codec_context,
                                              resample_context, &finished))
                goto cleanup;
            



            if (finished)
                break;
        }
        




        while (av_audio_fifo_size(fifo) >= output_frame_size ||
               (finished && av_audio_fifo_size(fifo) > 0))
            



            if (load_encode_and_write(fifo, output_format_context,
                                      output_codec_context))
                goto cleanup;
        



        if (finished) {
            int data_written;
            
            do {
                if (encode_audio_frame(NULL, output_format_context,
                                       output_codec_context, &data_written))
                    goto cleanup;
            } while (data_written);
            break;
        }
    }
    
    if (write_output_file_trailer(output_format_context))
        goto cleanup;
    ret = 0;
cleanup:
    if (fifo)
        av_audio_fifo_free(fifo);
    swr_free(&resample_context);
    if (output_codec_context)
        avcodec_close(output_codec_context);
    if (output_format_context) {
        avio_closep(&output_format_context->pb);
        avformat_free_context(output_format_context);
    }
    if (input_codec_context)
        avcodec_close(input_codec_context);
    if (input_format_context)
        avformat_close_input(&input_format_context);
    return ret;
}
