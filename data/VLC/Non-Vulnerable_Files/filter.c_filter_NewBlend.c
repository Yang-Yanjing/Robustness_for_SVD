#include <vlc_modules.h>
filter_t *filter_NewBlend( vlc_object_t *p_this,
                           const video_format_t *p_dst_chroma )
{
    filter_t *p_blend = vlc_custom_create( p_this, sizeof(*p_blend), "blend" );
    if( !p_blend )
        return NULL;
    es_format_Init( &p_blend->fmt_in, VIDEO_ES, 0 );
    es_format_Init( &p_blend->fmt_out, VIDEO_ES, 0 );
    p_blend->fmt_out.i_codec        = 
    p_blend->fmt_out.video.i_chroma = p_dst_chroma->i_chroma;
    p_blend->fmt_out.video.i_rmask  = p_dst_chroma->i_rmask;
    p_blend->fmt_out.video.i_gmask  = p_dst_chroma->i_gmask;
    p_blend->fmt_out.video.i_bmask  = p_dst_chroma->i_bmask;
    p_blend->fmt_out.video.i_rrshift= p_dst_chroma->i_rrshift;
    p_blend->fmt_out.video.i_rgshift= p_dst_chroma->i_rgshift;
    p_blend->fmt_out.video.i_rbshift= p_dst_chroma->i_rbshift;
    p_blend->fmt_out.video.i_lrshift= p_dst_chroma->i_lrshift;
    p_blend->fmt_out.video.i_lgshift= p_dst_chroma->i_lgshift;
    p_blend->fmt_out.video.i_lbshift= p_dst_chroma->i_lbshift;
    /* The blend module will be loaded when needed with the real
    * input format */
    p_blend->p_module = NULL;
    return p_blend;
}
