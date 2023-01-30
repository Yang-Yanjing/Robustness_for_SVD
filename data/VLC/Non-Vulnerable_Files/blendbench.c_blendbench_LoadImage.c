};
static int blendbench_LoadImage( vlc_object_t *p_this, picture_t **pp_pic,
                                 vlc_fourcc_t i_chroma, char *psz_file, const char *psz_name )
{
    image_handler_t *p_image;
    video_format_t fmt_in, fmt_out;
    memset( &fmt_in, 0, sizeof(video_format_t) );
    memset( &fmt_out, 0, sizeof(video_format_t) );
    fmt_out.i_chroma = i_chroma;
    p_image = image_HandlerCreate( p_this );
    *pp_pic = image_ReadUrl( p_image, psz_file, &fmt_in, &fmt_out );
    image_HandlerDelete( p_image );
    if( *pp_pic == NULL )
    {
        msg_Err( p_this, "Unable to load %s image", psz_name );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_this, "%s image has dim %d x %d (Y plane)", psz_name,
             (*pp_pic)->p[Y_PLANE].i_visible_pitch,
             (*pp_pic)->p[Y_PLANE].i_visible_lines );
    return VLC_SUCCESS;
}
