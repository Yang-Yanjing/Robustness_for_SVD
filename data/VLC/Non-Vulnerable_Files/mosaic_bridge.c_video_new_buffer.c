}
static picture_t *video_new_buffer( vlc_object_t *p_this,
                                    decoder_owner_sys_t *p_sys,
                                    es_format_t *fmt_out )
{
    VLC_UNUSED(p_this);
    if( fmt_out->video.i_width != p_sys->video.i_width ||
        fmt_out->video.i_height != p_sys->video.i_height ||
        fmt_out->video.i_chroma != p_sys->video.i_chroma ||
        (int64_t)fmt_out->video.i_sar_num * p_sys->video.i_sar_den !=
        (int64_t)fmt_out->video.i_sar_den * p_sys->video.i_sar_num )
    {
        vlc_ureduce( &fmt_out->video.i_sar_num,
                     &fmt_out->video.i_sar_den,
                     fmt_out->video.i_sar_num,
                     fmt_out->video.i_sar_den, 0 );
        if( !fmt_out->video.i_visible_width ||
            !fmt_out->video.i_visible_height )
        {
            fmt_out->video.i_visible_width = fmt_out->video.i_width;
            fmt_out->video.i_visible_height = fmt_out->video.i_height;
        }
        fmt_out->video.i_chroma = fmt_out->i_codec;
        p_sys->video = fmt_out->video;
    }
    /* */
    fmt_out->video.i_chroma = fmt_out->i_codec;
    return picture_NewFromFormat( &fmt_out->video );
}
