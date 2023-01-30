}
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( pp_block == NULL )
        return NULL;
    block_t *p_block = *pp_block;
    *pp_block = NULL;
    if( p_block == NULL )
        return NULL;
    if( p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    else if( !date_Get( &p_sys->end_date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( p_block );
        return NULL;
    }
    /* Don't re-use the same pts twice */
    p_block->i_pts = VLC_TS_INVALID;
    unsigned samples = p_block->i_buffer / p_dec->fmt_in.audio.i_channels;
    if( samples == 0 )
    {
        block_Release( p_block );
        return NULL;
    }
    block_t *p_out = decoder_NewAudioBuffer( p_dec, samples );
    if( p_out == NULL )
    {
        block_Release( p_block );
        return NULL;
    }
    assert( p_out->i_nb_samples == samples );
    assert( p_out->i_buffer == samples * 2 * p_dec->fmt_in.audio.i_channels );
    p_out->i_pts = date_Get( &p_sys->end_date );
    p_out->i_length = date_Increment( &p_sys->end_date, samples )
                      - p_out->i_pts;
    const uint8_t *src = p_block->p_buffer;
    int16_t *dst = (int16_t *)p_out->p_buffer;
    samples *= p_dec->fmt_in.audio.i_channels;
    for( unsigned i = 0; i < samples; i++ )
       *(dst++) = p_sys->table[*(src++)];
    block_Release( p_block );
    return p_out;
}
