 *****************************************************************************/
static block_t *GetAoutBuffer( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_buf;
    /* Hack for DTS S/PDIF filter which needs to send 3 frames at a time
     * (plus a few header bytes) */
    p_buf = decoder_NewAudioBuffer( p_dec, p_sys->i_frame_length * 4 );
    if( p_buf == NULL ) return NULL;
    p_buf->i_nb_samples = p_sys->i_frame_length;
    p_buf->i_buffer = p_sys->i_frame_size;
    p_buf->i_pts = date_Get( &p_sys->end_date );
    p_buf->i_length = date_Increment( &p_sys->end_date, p_sys->i_frame_length )
                      - p_buf->i_pts;
    return p_buf;
}
