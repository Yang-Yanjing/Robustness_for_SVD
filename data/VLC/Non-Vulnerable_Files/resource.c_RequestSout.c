}
static sout_instance_t *RequestSout( input_resource_t *p_resource,
                                     sout_instance_t *p_sout, const char *psz_sout )
{
#ifdef ENABLE_SOUT
    if( !p_sout && !psz_sout )
    {
        if( p_resource->p_sout )
            msg_Dbg( p_resource->p_sout, "destroying useless sout" );
        DestroySout( p_resource );
        return NULL;
    }
    assert( !p_sout || ( !p_resource->p_sout && !psz_sout ) );
    /* Check the validity of the sout */
    if( p_resource->p_sout &&
        strcmp( p_resource->p_sout->psz_sout, psz_sout ) )
    {
        msg_Dbg( p_resource->p_parent, "destroying unusable sout" );
        DestroySout( p_resource );
    }
    if( psz_sout )
    {
        if( p_resource->p_sout )
        {
            /* Reuse it */
            msg_Dbg( p_resource->p_parent, "reusing sout" );
            msg_Dbg( p_resource->p_parent, "you probably want to use gather stream_out" );
        }
        else
        {
            /* Create a new one */
            p_resource->p_sout = sout_NewInstance( p_resource->p_parent, psz_sout );
        }
        p_sout = p_resource->p_sout;
        p_resource->p_sout = NULL;
        return p_sout;
    }
    else
    {
        p_resource->p_sout = p_sout;
        return NULL;
    }
#else
    VLC_UNUSED (p_resource); VLC_UNUSED (p_sout); VLC_UNUSED (psz_sout);
    return NULL;
#endif
}
