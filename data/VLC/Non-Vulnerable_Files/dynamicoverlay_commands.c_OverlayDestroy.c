}
int OverlayDestroy( overlay_t *p_ovl )
{
    free( p_ovl->data.p_text );
    text_style_Delete( p_ovl->p_fontstyle );
    return VLC_SUCCESS;
}
