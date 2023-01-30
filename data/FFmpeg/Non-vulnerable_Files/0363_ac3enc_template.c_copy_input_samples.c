static void copy_input_samples(AC3EncodeContext *s, SampleType **samples)
{
    int ch;
    
    for (ch = 0; ch < s->channels; ch++) {
        
        memcpy(&s->planar_samples[ch][0], &s->planar_samples[ch][AC3_BLOCK_SIZE * s->num_blocks],
               AC3_BLOCK_SIZE * sizeof(s->planar_samples[0][0]));
        
        memcpy(&s->planar_samples[ch][AC3_BLOCK_SIZE],
               samples[s->channel_map[ch]],
               AC3_BLOCK_SIZE * s->num_blocks * sizeof(s->planar_samples[0][0]));
    }
}
