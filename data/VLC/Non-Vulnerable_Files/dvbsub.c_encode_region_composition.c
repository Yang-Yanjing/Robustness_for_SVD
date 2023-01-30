}
static void encode_region_composition( encoder_t *p_enc, bs_t *s,
                                       subpicture_t *p_subpic )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    subpicture_region_t *p_region;
    int i_region;
    for( i_region = 0, p_region = p_subpic->p_region; p_region;
         p_region = p_region->p_next, i_region++ )
    {
        int i_entries = 4, i_depth = 0x1, i_bg = 0;
        bool b_text =
            ( p_region->fmt.i_chroma == VLC_CODEC_TEXT );
        if( !b_text )
        {
            video_palette_t *p_pal = p_region->fmt.p_palette;
            if( !p_pal )
            {
                msg_Err( p_enc, "subpicture has no palette - ignoring it" );
                break;
            }
            i_entries = p_pal->i_entries;
            i_depth = i_entries == 4 ? 0x1 : i_entries == 16 ? 0x2 : 0x3;
            for( i_bg = 0; i_bg < p_pal->i_entries; i_bg++ )
            {
                if( !p_pal->palette[i_bg][3] ) break;
            }
        }
        bs_write( s, 8, 0x0f ); /* Sync byte */
        bs_write( s, 8, DVBSUB_ST_REGION_COMPOSITION ); /* Segment type */
        bs_write( s, 16, 1 );   /* Page id */
        bs_write( s, 16, 10 + 6 + (b_text ? 2 : 0) ); /* Segment length */
        bs_write( s, 8, i_region );
        bs_write( s, 4, p_sys->i_region_ver++ );
        /* Region attributes */
        bs_write( s, 1, i_bg < i_entries ); /* Fill */
        bs_write( s, 3, 0 ); /* Reserved */
        bs_write( s, 16, p_sys->p_regions[i_region].i_width );
        bs_write( s, 16, p_sys->p_regions[i_region].i_height );
        bs_write( s, 3, i_depth );  /* Region level of compatibility */
        bs_write( s, 3, i_depth  ); /* Region depth */
        bs_write( s, 2, 0 ); /* Reserved */
        bs_write( s, 8, 1 ); /* Clut id */
        bs_write( s, 8, i_bg ); /* region 8bit pixel code */
        bs_write( s, 4, i_bg ); /* region 4bit pixel code */
        bs_write( s, 2, i_bg ); /* region 2bit pixel code */
        bs_write( s, 2, 0 ); /* Reserved */
        /* In our implementation we only have 1 object per region */
        bs_write( s, 16, i_region );
        bs_write( s, 2, b_text ? DVBSUB_OT_BASIC_CHAR:DVBSUB_OT_BASIC_BITMAP );
        bs_write( s, 2, 0 ); /* object provider flag */
        bs_write( s, 12, 0 );/* object horizontal position */
        bs_write( s, 4, 0 ); /* Reserved */
        bs_write( s, 12, 0 );/* object vertical position */
        if( b_text )
        {
            bs_write( s, 8, 1 ); /* foreground pixel code */
            bs_write( s, 8, 0 ); /* background pixel code */
        }
    }
}
