};
static void LoadMask( filter_t *p_filter, const char *psz_filename )
{
    image_handler_t *p_image;
    video_format_t fmt_in, fmt_out;
    picture_t *p_old_mask = p_filter->p_sys->p_mask;
    memset( &fmt_in, 0, sizeof( video_format_t ) );
    memset( &fmt_out, 0, sizeof( video_format_t ) );
    fmt_out.i_chroma = VLC_CODEC_YUVA;
    p_image = image_HandlerCreate( p_filter );
    char *psz_url = vlc_path2uri( psz_filename, NULL );
    p_filter->p_sys->p_mask =
        image_ReadUrl( p_image, psz_url, &fmt_in, &fmt_out );
    free( psz_url );
    if( p_filter->p_sys->p_mask )
    {
        if( p_old_mask )
            picture_Release( p_old_mask );
    }
    else if( p_old_mask )
    {
        p_filter->p_sys->p_mask = p_old_mask;
        msg_Err( p_filter, "Error while loading new mask. Keeping old mask." );
    }
    else
        msg_Err( p_filter, "Error while loading new mask. No mask available." );
    image_HandlerDelete( p_image );
}
