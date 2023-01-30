 */
static void SetFilterMethod( filter_t *p_filter, const char *mode, bool pack )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if( mode == NULL )
        mode = "blend";
    p_sys->i_mode = DEINTERLACE_BLEND; /* default */
    p_sys->b_double_rate = false;
    p_sys->b_half_height = false;
    p_sys->b_use_frame_history = false;
    if( !strcmp( mode, "discard" ) )
    {
        p_sys->i_mode = DEINTERLACE_DISCARD;
        p_sys->b_half_height = true;
    }
    else if( !strcmp( mode, "bob" ) || !strcmp( mode, "progressive-scan" ) )
    {
        p_sys->i_mode = DEINTERLACE_BOB;
        p_sys->b_double_rate = true;
    }
    else if( !strcmp( mode, "linear" ) )
    {
        p_sys->i_mode = DEINTERLACE_LINEAR;
        p_sys->b_double_rate = true;
    }
    else if( !strcmp( mode, "mean" ) )
    {
        p_sys->i_mode = DEINTERLACE_MEAN;
        p_sys->b_half_height = true;
    }
    else if( !strcmp( mode, "blend" ) )
    {
    }
    else if( pack )
    {
        msg_Err( p_filter, "unknown or incompatible deinterlace mode \"%s\""
                 " for packed format", mode );
        mode = "blend";
    }
    else if( !strcmp( mode, "yadif" ) )
    {
        p_sys->i_mode = DEINTERLACE_YADIF;
        p_sys->b_use_frame_history = true;
    }
    else if( !strcmp( mode, "yadif2x" ) )
    {
        p_sys->i_mode = DEINTERLACE_YADIF2X;
        p_sys->b_double_rate = true;
        p_sys->b_use_frame_history = true;
    }
    else if( p_sys->chroma->pixel_size > 1 )
    {
        msg_Err( p_filter, "unknown or incompatible deinterlace mode \"%s\""
                 " for high depth format", mode );
        mode = "blend";
    }
    else if( !strcmp( mode, "x" ) )
    {
        p_sys->i_mode = DEINTERLACE_X;
    }
    else if( !strcmp( mode, "phosphor" ) )
    {
        p_sys->i_mode = DEINTERLACE_PHOSPHOR;
        p_sys->b_double_rate = true;
        p_sys->b_use_frame_history = true;
    }
    else if( !strcmp( mode, "ivtc" ) )
    {
        p_sys->i_mode = DEINTERLACE_IVTC;
        p_sys->b_use_frame_history = true;
    }
    else
        msg_Err( p_filter, "unknown deinterlace mode \"%s\"", mode );
    msg_Dbg( p_filter, "using %s deinterlace method", mode );
}
