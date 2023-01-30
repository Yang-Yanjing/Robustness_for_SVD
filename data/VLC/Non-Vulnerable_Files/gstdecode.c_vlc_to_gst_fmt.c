}
static GstStructure* vlc_to_gst_fmt( const es_format_t *p_fmt )
{
    const video_format_t *p_vfmt = &p_fmt->video;
    GstStructure *p_str = NULL;
    switch( p_fmt->i_codec ){
    case VLC_CODEC_H264:
        p_str = gst_structure_new_empty( "video/x-h264" );
        gst_structure_set( p_str, "alignment", G_TYPE_STRING, "au", NULL );
        break;
    case VLC_CODEC_MP4V:
        p_str = gst_structure_new_empty( "video/mpeg" );
        gst_structure_set( p_str, "mpegversion", G_TYPE_INT, 4,
                "systemstream", G_TYPE_BOOLEAN, FALSE, NULL );
        break;
    case VLC_CODEC_VP8:
        p_str = gst_structure_new_empty( "video/x-vp8" );
        break;
    case VLC_CODEC_MPGV:
        p_str = gst_structure_new_empty( "video/mpeg" );
        gst_structure_set( p_str, "mpegversion", G_TYPE_INT, 2,
                "systemstream", G_TYPE_BOOLEAN, FALSE, NULL );
        break;
    case VLC_CODEC_FLV1:
        p_str = gst_structure_new_empty( "video/x-flash-video" );
        gst_structure_set( p_str, "flvversion", G_TYPE_INT, 1, NULL );
        break;
    case VLC_CODEC_WMV1:
        p_str = gst_structure_new_empty( "video/x-wmv" );
        gst_structure_set( p_str, "wmvversion", G_TYPE_INT, 1,
                "format", G_TYPE_STRING, "WMV1", NULL );
        break;
    case VLC_CODEC_WMV2:
        p_str = gst_structure_new_empty( "video/x-wmv" );
        gst_structure_set( p_str, "wmvversion", G_TYPE_INT, 2,
                "format", G_TYPE_STRING, "WMV2", NULL );
        break;
    case VLC_CODEC_WMV3:
        p_str = gst_structure_new_empty( "video/x-wmv" );
        gst_structure_set( p_str, "wmvversion", G_TYPE_INT, 3,
                "format", G_TYPE_STRING, "WMV3", NULL );
        break;
    case VLC_CODEC_VC1:
        p_str = gst_structure_new_empty( "video/x-wmv" );
        gst_structure_set( p_str, "wmvversion", G_TYPE_INT, 3,
                "format", G_TYPE_STRING, "WVC1", NULL );
        break;
    default:
        /* unsupported codec */
        return NULL;
    }
    if( p_vfmt->i_width && p_vfmt->i_height )
        gst_structure_set( p_str,
                "width", G_TYPE_INT, p_vfmt->i_width,
                "height", G_TYPE_INT, p_vfmt->i_height, NULL );
    if( p_vfmt->i_frame_rate && p_vfmt->i_frame_rate_base )
        gst_structure_set( p_str, "framerate", GST_TYPE_FRACTION,
                p_vfmt->i_frame_rate,
                p_vfmt->i_frame_rate_base, NULL );
    if( p_vfmt->i_sar_num && p_vfmt->i_sar_den )
        gst_structure_set( p_str, "pixel-aspect-ratio", GST_TYPE_FRACTION,
                p_vfmt->i_sar_num,
                p_vfmt->i_sar_den, NULL );
    if( p_fmt->i_extra )
    {
        GstBuffer *p_buf;
        p_buf = gst_buffer_new_wrapped_full( GST_MEMORY_FLAG_READONLY,
                p_fmt->p_extra, p_fmt->i_extra, 0,
                p_fmt->i_extra, NULL, NULL );
        if( p_buf == NULL )
        {
            gst_structure_free( p_str );
            return NULL;
        }
        gst_structure_set( p_str, "codec_data", GST_TYPE_BUFFER, p_buf, NULL );
        gst_buffer_unref( p_buf );
    }
    return p_str;
}
