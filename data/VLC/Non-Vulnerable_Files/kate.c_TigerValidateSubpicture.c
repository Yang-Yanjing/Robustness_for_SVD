}
static int TigerValidateSubpicture( subpicture_t *p_subpic,
                                    bool b_fmt_src, const video_format_t *p_fmt_src,
                                    bool b_fmt_dst, const video_format_t *p_fmt_dst,
                                    mtime_t ts )
{
    VLC_UNUSED(p_fmt_src); VLC_UNUSED(p_fmt_dst);
    decoder_sys_t *p_sys = p_subpic->updater.p_sys->p_dec_sys;
    if( b_fmt_src || b_fmt_dst )
        return VLC_EGENERIC;
    PROFILE_START( TigerValidateSubpicture );
    /* time in seconds from the start of the stream */
    kate_float t = (p_subpic->updater.p_sys->i_start + ts - p_subpic->i_start ) / 1000000.0f;
    /* it is likely that the current region (if any) can be kept as is; test for this */
    vlc_mutex_lock( &p_sys->lock );
    int i_ret;
    if( p_sys->b_dirty || tiger_renderer_is_dirty( p_sys->p_tr ) )
    {
        i_ret = VLC_EGENERIC;
        goto exit;
    }
    if( tiger_renderer_update( p_sys->p_tr, t, 1 ) >= 0 &&
        tiger_renderer_is_dirty( p_sys->p_tr ) )
    {
        i_ret = VLC_EGENERIC;
        goto exit;
    }
    i_ret = VLC_SUCCESS;
exit:
    vlc_mutex_unlock( &p_sys->lock );
    PROFILE_STOP( TigerValidateSubpicture );
    return i_ret;
}
