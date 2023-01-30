 ****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( pp_block == NULL )
        return NULL;
    block_t *p_block = *pp_block;
    if( p_block == NULL )
        return NULL;
    *pp_block = NULL;
    if( p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
    else if( !date_Get( &p_sys->end_date ) )
        /* We've just started the stream, wait for the first PTS. */
        goto skip;
    unsigned samples = (8 * p_block->i_buffer) / p_sys->framebits;
    if( samples == 0 )
        goto skip;
    if( p_sys->decode != NULL )
    {
        block_t *p_out = decoder_NewAudioBuffer( p_dec, samples );
        if( p_out == NULL )
            goto skip;
        p_sys->decode( p_out->p_buffer, p_block->p_buffer,
                       samples * p_dec->fmt_in.audio.i_channels );
        block_Release( p_block );
        p_block = p_out;
    }
    else
    {
        decoder_UpdateAudioFormat( p_dec );
        p_block->i_nb_samples = samples;
        p_block->i_buffer = samples * (p_sys->framebits / 8);
    }
    p_block->i_pts = date_Get( &p_sys->end_date );
    p_block->i_length = date_Increment( &p_sys->end_date, samples )
                      - p_block->i_pts;
    return p_block;
skip:
    block_Release( p_block );
    return NULL;
}
