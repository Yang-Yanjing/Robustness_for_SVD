 *****************************************************************************/
static block_t *GetAoutBuffer( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_buf;
    p_buf = decoder_NewAudioBuffer( p_dec, p_sys->i_frame_length );
    if( p_buf == NULL ) return NULL;
    p_buf->i_pts = date_Get( &p_sys->end_date );
    p_buf->i_length = date_Increment( &p_sys->end_date, p_sys->i_frame_length )
                      - p_buf->i_pts;
    if( p_sys->b_discontinuity )
        p_buf->i_flags |= BLOCK_FLAG_DISCONTINUITY;
    p_sys->b_discontinuity = false;
    /* Hack for libmad filter */
    p_buf = block_Realloc( p_buf, 0, p_sys->i_frame_size + MAD_BUFFER_GUARD );
    return p_buf;
}
