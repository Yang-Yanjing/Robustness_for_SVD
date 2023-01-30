}
static void Destroy_FT( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->p_stroker )
        FT_Stroker_Done( p_sys->p_stroker );
    FT_Done_Face( p_sys->p_face );
    FT_Done_FreeType( p_sys->p_library );
}
