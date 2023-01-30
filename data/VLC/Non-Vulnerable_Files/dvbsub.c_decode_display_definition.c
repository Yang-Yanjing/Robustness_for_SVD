/* ETSI 300 743 [7.2.1] */
static void decode_display_definition( decoder_t *p_dec, bs_t *s )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint16_t      i_segment_length;
    uint16_t      i_processed_length = 40;
    int           i_version;
    i_segment_length = bs_read( s, 16 );
    i_version        = bs_read( s, 4 );
    /* Check version number */
    if( p_sys->display.i_version == i_version )
    {
        /* The definition did not change */
        bs_skip( s, 8*i_segment_length - 4 );
        return;
    }
#ifdef DEBUG_DVBSUB
    msg_Dbg( p_dec, "new display definition: %i", i_version );
#endif
    /* We don't have this version of the display definition: Parse it */
    p_sys->display.i_version = i_version;
    p_sys->display.b_windowed = bs_read( s, 1 );
    bs_skip( s, 3 ); /* Reserved bits */
    p_sys->display.i_width = bs_read( s, 16 )+1;
    p_sys->display.i_height = bs_read( s, 16 )+1;
    if( p_sys->display.b_windowed )
    {
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "display definition with offsets (windowed)" );
#endif
        /* Coordinates are measured from the top left corner */
        p_sys->display.i_x     = bs_read( s, 16 );
        p_sys->display.i_max_x = bs_read( s, 16 );
        p_sys->display.i_y     = bs_read( s, 16 );
        p_sys->display.i_max_y = bs_read( s, 16 );
        i_processed_length += 64;
    }
    else
    {
        /* if not windowed, setup the window variables to good defaults */
        /* not necessary, but to avoid future confusion.. */
        p_sys->display.i_x     = 0;
        p_sys->display.i_max_x = p_sys->display.i_width-1;
        p_sys->display.i_y     = 0;
        p_sys->display.i_max_y = p_sys->display.i_height-1;
    }
    if( i_processed_length != i_segment_length*8 )
    {
        msg_Err( p_dec, "processed length %d bytes != segment length %d bytes",
                 i_processed_length / 8 , i_segment_length );
    }
#ifdef DEBUG_DVBSUB
    msg_Dbg( p_dec, "version: %d, width: %d, height: %d",
             p_sys->display.i_version, p_sys->display.i_width, p_sys->display.i_height );
    if( p_sys->display.b_windowed )
        msg_Dbg( p_dec, "xmin: %d, xmax: %d, ymin: %d, ymax: %d",
                 p_sys->display.i_x, p_sys->display.i_max_x, p_sys->display.i_y, p_sys->display.i_max_y );
#endif
}
