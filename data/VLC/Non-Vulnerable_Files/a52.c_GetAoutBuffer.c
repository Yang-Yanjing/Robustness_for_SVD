 *****************************************************************************/
static block_t *GetAoutBuffer( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_buf = decoder_NewAudioBuffer( p_dec, p_sys->frame.i_samples );
    if( p_buf )
    {
        p_buf->i_pts = date_Get( &p_sys->end_date );
        p_buf->i_length = date_Increment( &p_sys->end_date,
                                          p_sys->frame.i_samples ) - p_buf->i_pts;
    }
    return p_buf;
}
