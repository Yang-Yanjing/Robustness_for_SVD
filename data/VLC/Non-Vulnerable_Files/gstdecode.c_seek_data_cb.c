 * done by VLC framework */
static gboolean seek_data_cb( GstAppSrc *p_src, guint64 l_offset,
        gpointer p_data )
{
    VLC_UNUSED( p_src );
    decoder_t *p_dec = p_data;
    msg_Dbg( p_dec, "appsrc seeking to %"G_GUINT64_FORMAT, l_offset );
    return TRUE;
}
