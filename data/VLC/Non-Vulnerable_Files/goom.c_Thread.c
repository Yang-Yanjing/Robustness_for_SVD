 *****************************************************************************/
static void *Thread( void *p_thread_data )
{
    goom_thread_t *p_thread = (goom_thread_t*)p_thread_data;
    date_t i_pts;
    int16_t p_data[2][512];
    int i_data = 0, i_count = 0;
    PluginInfo *p_plugin_info;
    int canc = vlc_savecancel ();
    p_plugin_info = goom_init( p_thread->i_width, p_thread->i_height );
    for( ;; )
    {
        uint32_t  *plane;
        picture_t *p_pic;
        /* FIXME the way the update is done is not really good.
         *  Supurious wake up from p_thread->wait will make it generates a frame
         * without using new samples (probably rare as we should not be waiting
         * samples).
         *  The frame rate at which the video is generated is not well controlled
         * nor the time at which each frame is displayed (not smooth)
         */
        /* goom_update is damn slow, so just copy data and release the lock */
        vlc_mutex_lock( &p_thread->lock );
        if( !p_thread->b_exit &&
            FillBuffer( (int16_t *)p_data, &i_data,
                        &i_pts, &p_thread->date, p_thread ) != VLC_SUCCESS )
            vlc_cond_wait( &p_thread->wait, &p_thread->lock );
        bool b_exit = p_thread->b_exit;
        vlc_mutex_unlock( &p_thread->lock );
        if( b_exit )
            break;
        /* Speed selection */
        if( p_thread->i_speed && (++i_count % (p_thread->i_speed+1)) ) continue;
        /* Frame dropping if necessary */
        if( date_Get( &i_pts ) + GOOM_DELAY <= mdate() ) continue;
        plane = goom_update( p_plugin_info, p_data, 0, 0.0,
                             NULL, NULL );
        while( !( p_pic = vout_GetPicture( p_thread->p_vout ) ) )
        {
            vlc_mutex_lock( &p_thread->lock );
            bool b_exit = p_thread->b_exit;
            vlc_mutex_unlock( &p_thread->lock );
            if( b_exit )
                break;
            msleep( VOUT_OUTMEM_SLEEP );
        }
        if( p_pic == NULL ) break;
        memcpy( p_pic->p[0].p_pixels, plane, p_thread->i_width * p_thread->i_height * 4 );
        p_pic->date = date_Get( &i_pts ) + GOOM_DELAY;
        vout_PutPicture( p_thread->p_vout, p_pic );
    }
    goom_close( p_plugin_info );
    vlc_restorecancel (canc);
    return NULL;
}
