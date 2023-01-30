 *****************************************************************************/
static void PushPicture( sout_stream_t *p_stream, picture_t *p_picture )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    bridged_es_t *p_es = p_sys->p_es;
    vlc_global_lock( VLC_MOSAIC_MUTEX );
    *p_es->pp_last = p_picture;
    p_picture->p_next = NULL;
    p_es->pp_last = &p_picture->p_next;
    vlc_global_unlock( VLC_MOSAIC_MUTEX );
}
