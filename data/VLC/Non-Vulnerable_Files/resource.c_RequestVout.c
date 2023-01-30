}
static vout_thread_t *RequestVout( input_resource_t *p_resource,
                                   vout_thread_t *p_vout,
                                   video_format_t *p_fmt, unsigned dpb_size,
                                   bool b_recycle )
{
    vlc_assert_locked( &p_resource->lock );
    if( !p_vout && !p_fmt )
    {
        if( p_resource->p_vout_free )
        {
            msg_Dbg( p_resource->p_vout_free, "destroying useless vout" );
            vout_CloseAndRelease( p_resource->p_vout_free );
            p_resource->p_vout_free = NULL;
        }
        return NULL;
    }
    if( p_fmt )
    {
        /* */
        if( !p_vout && p_resource->p_vout_free )
        {
            msg_Dbg( p_resource->p_parent, "trying to reuse free vout" );
            p_vout = p_resource->p_vout_free;
            p_resource->p_vout_free = NULL;
        }
        else if( p_vout )
        {
            assert( p_vout != p_resource->p_vout_free );
            vlc_mutex_lock( &p_resource->lock_hold );
            TAB_REMOVE( p_resource->i_vout, p_resource->pp_vout, p_vout );
            vlc_mutex_unlock( &p_resource->lock_hold );
        }
        /* */
        vout_configuration_t cfg = {
            .vout       = p_vout,
            .input      = VLC_OBJECT(p_resource->p_input),
            .change_fmt = true,
            .fmt        = p_fmt,
            .dpb_size   = dpb_size,
        };
        p_vout = vout_Request( p_resource->p_parent, &cfg );
        if( !p_vout )
            return NULL;
        DisplayVoutTitle( p_resource, p_vout );
        vlc_mutex_lock( &p_resource->lock_hold );
        TAB_APPEND( p_resource->i_vout, p_resource->pp_vout, p_vout );
        vlc_mutex_unlock( &p_resource->lock_hold );
        return p_vout;
    }
    else
    {
        assert( p_vout );
        vlc_mutex_lock( &p_resource->lock_hold );
        TAB_REMOVE( p_resource->i_vout, p_resource->pp_vout, p_vout );
        const int i_vout_active = p_resource->i_vout;
        vlc_mutex_unlock( &p_resource->lock_hold );
        if( p_resource->p_vout_free || i_vout_active > 0 || !b_recycle )
        {
            if( b_recycle )
                msg_Dbg( p_resource->p_parent, "destroying vout (already one saved or active)" );
            vout_CloseAndRelease( p_vout );
        }
        else
        {
            msg_Dbg( p_resource->p_parent, "saving a free vout" );
            vout_Flush( p_vout, 1 );
            vout_FlushSubpictureChannel( p_vout, -1 );
            vout_configuration_t cfg = {
                .vout       = p_vout,
                .input      = NULL,
                .change_fmt = false,
                .fmt        = NULL,
                .dpb_size   = 0,
            };
            p_resource->p_vout_free = vout_Request( p_resource->p_parent, &cfg );
        }
        return NULL;
    }
}
}
static vout_thread_t *RequestVout( input_resource_t *p_resource,
                                   vout_thread_t *p_vout,
                                   video_format_t *p_fmt, unsigned dpb_size,
                                   bool b_recycle )
{
    vlc_assert_locked( &p_resource->lock );
    if( !p_vout && !p_fmt )
    {
        if( p_resource->p_vout_free )
        {
            msg_Dbg( p_resource->p_vout_free, "destroying useless vout" );
            vout_CloseAndRelease( p_resource->p_vout_free );
            p_resource->p_vout_free = NULL;
        }
        return NULL;
    }
