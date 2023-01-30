 *****************************************************************************/
static void SavePicture( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    video_format_t fmt_in, fmt_out;
    char *psz_filename = NULL;
    char *psz_temp = NULL;
    int i_ret;
    memset( &fmt_in, 0, sizeof(video_format_t) );
    memset( &fmt_out, 0, sizeof(video_format_t) );
    /* Save snapshot psz_format to a memory zone */
    fmt_in = p_pic->format;
    fmt_out.i_sar_num = fmt_out.i_sar_den = 1;
    fmt_out.i_width = p_sys->i_width;
    fmt_out.i_height = p_sys->i_height;
    fmt_out.i_chroma = p_sys->i_format;
    /*
     * Save the snapshot to a temporary file and
     * switch it to the real name afterwards.
     */
    if( p_sys->b_replace )
        i_ret = asprintf( &psz_filename, "%s" DIR_SEP "%s.%s",
                          p_sys->psz_path, p_sys->psz_prefix,
                          p_sys->psz_format );
    else
        i_ret = asprintf( &psz_filename, "%s" DIR_SEP "%s%05d.%s",
                          p_sys->psz_path, p_sys->psz_prefix,
                          p_sys->i_frames, p_sys->psz_format );
    if( i_ret == -1 )
    {
        msg_Err( p_filter, "could not create snapshot %s", psz_filename );
        goto error;
    }
    path_sanitize( psz_filename );
    i_ret = asprintf( &psz_temp, "%s.swp", psz_filename );
    if( i_ret == -1 )
    {
        msg_Err( p_filter, "could not create snapshot temporarily file %s", psz_temp );
        goto error;
    }
    path_sanitize( psz_temp );
    /* Save the image */
    i_ret = image_WriteUrl( p_sys->p_image, p_pic, &fmt_in, &fmt_out,
                            psz_temp );
    if( i_ret != VLC_SUCCESS )
    {
        msg_Err( p_filter, "could not create snapshot %s", psz_temp );
    }
    else
    {
        /* switch to the final destination */
#if defined (_WIN32) || defined(__OS2__)
        vlc_unlink( psz_filename );
#endif
        i_ret = vlc_rename( psz_temp, psz_filename );
        if( i_ret == -1 )
        {
            msg_Err( p_filter, "could not rename snapshot %s: %s",
                     psz_filename, vlc_strerror_c(errno) );
            goto error;
        }
    }
error:
    free( psz_temp );
    free( psz_filename );
}
