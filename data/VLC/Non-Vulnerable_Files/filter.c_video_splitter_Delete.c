}
void video_splitter_Delete( video_splitter_t *p_splitter )
{
    if( p_splitter->p_module )
        module_unneed( p_splitter, p_splitter->p_module );
    video_format_Clean( &p_splitter->fmt );
    vlc_object_release( p_splitter );
}
