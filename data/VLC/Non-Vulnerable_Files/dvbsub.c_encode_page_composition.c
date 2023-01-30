}
static void encode_page_composition( encoder_t *p_enc, bs_t *s,
                                     subpicture_t *p_subpic )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    subpicture_region_t *p_region;
    bool b_mode_change = false;
    int i_regions, i_timeout;
    bs_write( s, 8, 0x0f ); /* Sync byte */
    bs_write( s, 8, DVBSUB_ST_PAGE_COMPOSITION ); /* Segment type */
    bs_write( s, 16, 1 ); /* Page id */
    for( i_regions = 0, p_region = p_subpic ? p_subpic->p_region : 0;
         p_region; p_region = p_region->p_next, i_regions++ )
    {
        if( i_regions >= p_sys->i_regions )
        {
            encoder_region_t region;
            region.i_width = region.i_height = 0;
            p_sys->p_regions = xrealloc( p_sys->p_regions,
                          sizeof(encoder_region_t) * (p_sys->i_regions + 1) );
            p_sys->p_regions[p_sys->i_regions++] = region;
        }
        if( ( p_sys->p_regions[i_regions].i_width <
              (int)p_region->fmt.i_visible_width ) ||
            ( p_sys->p_regions[i_regions].i_width >
              (int)p_region->fmt.i_visible_width ) )
        {
            b_mode_change = true;
            msg_Dbg( p_enc, "region %i width change: %i -> %i",
                     i_regions, p_sys->p_regions[i_regions].i_width,
                     p_region->fmt.i_visible_width );
            p_sys->p_regions[i_regions].i_width =
                p_region->fmt.i_visible_width;
        }
        if( p_sys->p_regions[i_regions].i_height <
             (int)p_region->fmt.i_visible_height )
        {
            b_mode_change = true;
            msg_Dbg( p_enc, "region %i height change: %i -> %i",
                     i_regions, p_sys->p_regions[i_regions].i_height,
                     p_region->fmt.i_visible_height );
            p_sys->p_regions[i_regions].i_height =
                p_region->fmt.i_visible_height;
        }
    }
    bs_write( s, 16, i_regions * 6 + 2 ); /* Segment length */
    i_timeout = 0;
    if( p_subpic && !p_subpic->b_ephemer &&
        ( p_subpic->i_stop > p_subpic->i_start ) )
    {
        i_timeout = (p_subpic->i_stop - p_subpic->i_start) / 1000000;
    }
    bs_write( s, 8, i_timeout ); /* Timeout */
    bs_write( s, 4, p_sys->i_page_ver++ );
    bs_write( s, 2, b_mode_change ?
              DVBSUB_PCS_STATE_CHANGE : DVBSUB_PCS_STATE_ACQUISITION );
    bs_write( s, 2, 0 ); /* Reserved */
    for( i_regions = 0, p_region = p_subpic ? p_subpic->p_region : 0;
         p_region; p_region = p_region->p_next, i_regions++ )
    {
        bs_write( s, 8, i_regions );
        bs_write( s, 8, 0 ); /* Reserved */
        if( (p_sys->i_offset_x > 0) && (p_sys->i_offset_y > 0) )
        {
            bs_write( s, 16, p_sys->i_offset_x ); /* override x position */
            bs_write( s, 16, p_sys->i_offset_y ); /* override y position */
        }
        else
        {
            bs_write( s, 16, p_region->i_x );
            bs_write( s, 16, p_region->i_y );
        }
    }
}
