}
static void FilterFL64( audio_volume_t *p_volume, block_t *p_buffer,
                        float f_multiplier )
{
    double *p = (double *)p_buffer->p_buffer;
    double mult = f_multiplier;
    if( mult == 1. )
        return; /* nothing to do */
    for( size_t i = p_buffer->i_buffer / sizeof(*p); i > 0; i-- )
        *(p++) *= mult;
    (void) p_volume;
}
