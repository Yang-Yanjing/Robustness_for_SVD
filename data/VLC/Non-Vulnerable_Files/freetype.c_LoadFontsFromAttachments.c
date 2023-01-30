 *****************************************************************************/
static int LoadFontsFromAttachments( filter_t *p_filter )
{
    filter_sys_t         *p_sys = p_filter->p_sys;
    input_attachment_t  **pp_attachments;
    int                   i_attachments_cnt;
    if( filter_GetInputAttachments( p_filter, &pp_attachments, &i_attachments_cnt ) )
        return VLC_EGENERIC;
    p_sys->i_font_attachments = 0;
    p_sys->pp_font_attachments = malloc( i_attachments_cnt * sizeof(*p_sys->pp_font_attachments));
    if( !p_sys->pp_font_attachments )
        return VLC_ENOMEM;
    for( int k = 0; k < i_attachments_cnt; k++ )
    {
        input_attachment_t *p_attach = pp_attachments[k];
        if( ( !strcmp( p_attach->psz_mime, "application/x-truetype-font" ) || // TTF
              !strcmp( p_attach->psz_mime, "application/x-font-otf" ) ) &&    // OTF
            p_attach->i_data > 0 && p_attach->p_data )
        {
            p_sys->pp_font_attachments[ p_sys->i_font_attachments++ ] = p_attach;
        }
        else
        {
            vlc_input_attachment_Delete( p_attach );
        }
    }
    free( pp_attachments );
    return VLC_SUCCESS;
}
