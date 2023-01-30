 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->pp_font_attachments )
    {
        for( int k = 0; k < p_sys->i_font_attachments; k++ )
            vlc_input_attachment_Delete( p_sys->pp_font_attachments[k] );
        free( p_sys->pp_font_attachments );
    }
    if( p_sys->p_xml ) xml_ReaderDelete( p_sys->p_xml );
    free( p_sys->style.psz_fontname );
    free( p_sys->style.psz_monofontname );
    Destroy_FT( p_this );
    free( p_sys );
}
