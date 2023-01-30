#include <vlc_video_splitter.h>
video_splitter_t *video_splitter_New( vlc_object_t *p_this,
                                      const char *psz_name,
                                      const video_format_t *p_fmt )
{
    video_splitter_t *p_splitter = vlc_custom_create( p_this,
                                       sizeof(*p_splitter), "video splitter" );
    if( !p_splitter )
        return NULL;
    video_format_Copy( &p_splitter->fmt, p_fmt );
    /* */
    p_splitter->p_module = module_need( p_splitter, "video splitter", psz_name, true );
    if( ! p_splitter->p_module )
    {
        video_splitter_Delete( p_splitter );
        return NULL;
    }
    return p_splitter;
}
