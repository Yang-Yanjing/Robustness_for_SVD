}
void filter_DeleteBlend( filter_t *p_blend )
{
    if( p_blend->p_module )
        module_unneed( p_blend, p_blend->p_module );
    vlc_object_release( p_blend );
}
