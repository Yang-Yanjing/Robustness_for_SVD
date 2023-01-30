 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    /* */
    ClosePlayItem( p_demux );
    /* */
    es_out_Delete( p_sys->p_out );
    /* Titles */
    for( int i = 0; i < p_sys->i_title; i++ )
        vlc_input_title_Delete( p_sys->pp_title[i] );
    TAB_CLEAN( p_sys->i_title, p_sys->pp_title );
    /* CLPI */
    for( int i = 0; i < p_sys->i_clpi; i++ )
    {
        bd_clpi_t *p_clpi = p_sys->pp_clpi[i];
        bd_clpi_Clean( p_clpi );
        free( p_clpi );
    }
    TAB_CLEAN( p_sys->i_clpi, p_sys->pp_clpi );
    /* MPLS */
    for( int i = 0; i < p_sys->i_mpls; i++ )
    {
        bd_mpls_t *p_mpls = p_sys->pp_mpls[i];
        bd_mpls_Clean( p_mpls );
        free( p_mpls );
    }
    TAB_CLEAN( p_sys->i_mpls, p_sys->pp_mpls );
    free( p_sys->psz_base );
    free( p_sys );
}
