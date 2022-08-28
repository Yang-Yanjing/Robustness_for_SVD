static int get_input(AVFrame *frame, int frame_num)
{
    int err, i, j;
#define FRAME_SIZE 1024
    
    frame->sample_rate    = INPUT_SAMPLERATE;
    frame->format         = INPUT_FORMAT;
    frame->channel_layout = INPUT_CHANNEL_LAYOUT;
    frame->nb_samples     = FRAME_SIZE;
    frame->pts            = frame_num * FRAME_SIZE;
    err = av_frame_get_buffer(frame, 0);
    if (err < 0)
        return err;
    
    for (i = 0; i < 5; i++) {
        float *data = (float*)frame->extended_data[i];
        for (j = 0; j < frame->nb_samples; j++)
            data[j] = sin(2 * M_PI * (frame_num + j) * (i + 1) / FRAME_SIZE);
    }
    return 0;
}
