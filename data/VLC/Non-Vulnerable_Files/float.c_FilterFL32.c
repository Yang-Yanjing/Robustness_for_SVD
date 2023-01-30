 */
static void FilterFL32( audio_volume_t *p_volume, block_t *p_buffer,
                        float f_multiplier )
{
    if( f_multiplier == 1.f )
        return; /* nothing to do */
    float *p = (float *)p_buffer->p_buffer;
    for( size_t i = p_buffer->i_buffer / sizeof(*p); i > 0; i-- )
        *(p++) *= f_multiplier;
    (void) p_volume;
}
