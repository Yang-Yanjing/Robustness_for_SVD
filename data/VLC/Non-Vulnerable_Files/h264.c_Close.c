 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i;
    if( p_sys->p_frame )
        block_ChainRelease( p_sys->p_frame );
    for( i = 0; i < SPS_MAX; i++ )
    {
        if( p_sys->pp_sps[i] )
            block_Release( p_sys->pp_sps[i] );
    }
    for( i = 0; i < PPS_MAX; i++ )
    {
        if( p_sys->pp_pps[i] )
            block_Release( p_sys->pp_pps[i] );
    }
    packetizer_Clean( &p_sys->packetizer );
    if( p_dec->pf_get_cc )
    {
         cc_Exit( &p_sys->cc_next );
         cc_Exit( &p_sys->cc );
    }
    free( p_sys );
}
