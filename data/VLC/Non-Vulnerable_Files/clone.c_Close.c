 */
static void Close( vlc_object_t *p_this )
{
    video_splitter_t *p_splitter = (video_splitter_t*)p_this;
    for( int i = 0; i < p_splitter->i_output; i++ )
    {
        video_splitter_output_t *p_cfg = &p_splitter->p_output[i];
        free( p_cfg->psz_module );
        video_format_Clean( &p_cfg->fmt );
    }
    free( p_splitter->p_output );
}
