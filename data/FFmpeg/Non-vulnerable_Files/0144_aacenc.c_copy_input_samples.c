static void copy_input_samples(AACEncContext *s, const AVFrame *frame)
{
    int ch;
    int end = 2048 + (frame ? frame->nb_samples : 0);
    const uint8_t *channel_map = aac_chan_maps[s->channels - 1];
    
    for (ch = 0; ch < s->channels; ch++) {
        
        memcpy(&s->planar_samples[ch][1024], &s->planar_samples[ch][2048], 1024 * sizeof(s->planar_samples[0][0]));
        
        if (frame) {
            memcpy(&s->planar_samples[ch][2048],
                   frame->extended_data[channel_map[ch]],
                   frame->nb_samples * sizeof(s->planar_samples[0][0]));
        }
        memset(&s->planar_samples[ch][end], 0,
               (3072 - end) * sizeof(s->planar_samples[0][0]));
    }
}
