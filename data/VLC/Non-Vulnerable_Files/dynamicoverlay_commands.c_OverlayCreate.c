 *****************************************************************************/
overlay_t *OverlayCreate( void )
{
    overlay_t *p_ovl = calloc( 1, sizeof( overlay_t ) );
    if( p_ovl == NULL )
       return NULL;
    p_ovl->i_x = p_ovl->i_y = 0;
    p_ovl->i_alpha = 0xFF;
    p_ovl->b_active = false;
    video_format_Setup( &p_ovl->format, VLC_FOURCC( '\0','\0','\0','\0') , 0, 0,
                        0, 0, 1, 1 );
    p_ovl->p_fontstyle = text_style_New();
    p_ovl->data.p_text = NULL;
    return p_ovl;
}
