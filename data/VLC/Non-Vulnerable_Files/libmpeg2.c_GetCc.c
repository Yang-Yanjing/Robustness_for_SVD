 *****************************************************************************/
static block_t *GetCc( decoder_t *p_dec, bool pb_present[4] )
{
    decoder_sys_t   *p_sys = p_dec->p_sys;
    block_t         *p_cc = NULL;
    int i;
    for( i = 0; i < 4; i++ )
        pb_present[i] = p_sys->cc.pb_present[i];
    if( p_sys->cc.i_data <= 0 )
        return NULL;
    p_cc = block_Alloc( p_sys->cc.i_data);
    if( p_cc )
    {
        memcpy( p_cc->p_buffer, p_sys->cc.p_data, p_sys->cc.i_data );
        p_cc->i_dts =
        p_cc->i_pts = p_sys->cc.b_reorder ? p_sys->i_cc_pts : p_sys->i_cc_dts;
        p_cc->i_flags = ( p_sys->cc.b_reorder  ? p_sys->i_cc_flags : BLOCK_FLAG_TYPE_P ) & ( BLOCK_FLAG_TYPE_I|BLOCK_FLAG_TYPE_P|BLOCK_FLAG_TYPE_B);
    }
    cc_Flush( &p_sys->cc );
    return p_cc;
}
