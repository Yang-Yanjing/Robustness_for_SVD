/* Sets the video output format */
static bool set_vout_format( GstStructure* p_str,
        const es_format_t *restrict p_infmt, es_format_t *restrict p_outfmt )
{
    video_format_t *p_voutfmt = &p_outfmt->video;
    const video_format_t *p_vinfmt = &p_infmt->video;
    gboolean b_ret;
    /* We are interested in system memory raw buffers for now,
     * but support for opaque data formats can also be added.
     * For eg. when using HW decoders for zero-copy */
    p_outfmt->i_codec = vlc_fourcc_GetCodecFromString(
            VIDEO_ES,
            gst_structure_get_string( p_str, "format" ) );
    if( !p_outfmt->i_codec )
        return false;
    gst_structure_get_int( p_str, "width", &p_voutfmt->i_width );
    gst_structure_get_int( p_str, "height", &p_voutfmt->i_height );
    b_ret = gst_structure_get_fraction( p_str,
            "pixel-aspect-ratio",
            &p_voutfmt->i_sar_num,
            &p_voutfmt->i_sar_den );
    if( !b_ret || !p_voutfmt->i_sar_num ||
            !p_voutfmt->i_sar_den )
    {
        p_voutfmt->i_sar_num = 1;
        p_voutfmt->i_sar_den = 1;
    }
    b_ret = gst_structure_get_fraction( p_str, "framerate",
            &p_voutfmt->i_frame_rate,
            &p_voutfmt->i_frame_rate_base );
    if( !b_ret || !p_voutfmt->i_frame_rate ||
            !p_voutfmt->i_frame_rate_base )
    {
        p_voutfmt->i_frame_rate = p_vinfmt->i_frame_rate;
        p_voutfmt->i_frame_rate_base = p_vinfmt->i_frame_rate_base;
    }
    return true;
}
