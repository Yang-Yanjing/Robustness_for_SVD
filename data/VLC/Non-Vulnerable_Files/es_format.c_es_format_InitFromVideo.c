}
void es_format_InitFromVideo( es_format_t *p_es, const video_format_t *p_fmt )
{
    es_format_Init( p_es, VIDEO_ES, p_fmt->i_chroma );
    video_format_Copy( &p_es->video, p_fmt );
}
