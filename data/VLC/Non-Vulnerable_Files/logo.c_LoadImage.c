 */
static picture_t *LoadImage( vlc_object_t *p_this, const char *psz_filename )
{
    if( !psz_filename )
        return NULL;
    video_format_t fmt_in;
    video_format_Init( &fmt_in, 0 );
    video_format_t fmt_out;
    video_format_Init( &fmt_out, VLC_CODEC_YUVA );
    image_handler_t *p_image = image_HandlerCreate( p_this );
    if( !p_image )
        return NULL;
    char *psz_url = vlc_path2uri( psz_filename, NULL );
    picture_t *p_pic = image_ReadUrl( p_image, psz_url, &fmt_in, &fmt_out );
    free( psz_url );
    image_HandlerDelete( p_image );
    return p_pic;
}
