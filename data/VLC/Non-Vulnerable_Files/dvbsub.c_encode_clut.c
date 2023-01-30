}
static void encode_clut( encoder_t *p_enc, bs_t *s, subpicture_t *p_subpic )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    subpicture_region_t *p_region = p_subpic->p_region;
    video_palette_t *p_pal, pal;
    int i;
    /* Sanity check */
    if( !p_region ) return;
    if( p_region->fmt.i_chroma == VLC_CODEC_YUVP )
    {
        p_pal = p_region->fmt.p_palette;
    }
    else
    {
        pal.i_entries = 4;
        for( i = 0; i < 4; i++ )
        {
            pal.palette[i][0] = 0;
            pal.palette[i][1] = 0;
            pal.palette[i][2] = 0;
            pal.palette[i][3] = 0;
        }
        p_pal = &pal;
    }
    bs_write( s, 8, 0x0f ); /* Sync byte */
    bs_write( s, 8, DVBSUB_ST_CLUT_DEFINITION ); /* Segment type */
    bs_write( s, 16, 1 );  /* Page id */
    bs_write( s, 16, p_pal->i_entries * 6 + 2 ); /* Segment length */
    bs_write( s, 8, 1 ); /* Clut id */
    bs_write( s, 4, p_sys->i_clut_ver++ );
    bs_write( s, 4, 0 ); /* Reserved */
    for( i = 0; i < p_pal->i_entries; i++ )
    {
        bs_write( s, 8, i ); /* Clut entry id */
        bs_write( s, 1, p_pal->i_entries == 4 );   /* 2bit/entry flag */
        bs_write( s, 1, p_pal->i_entries == 16 );  /* 4bit/entry flag */
        bs_write( s, 1, p_pal->i_entries == 256 ); /* 8bit/entry flag */
        bs_write( s, 4, 0 ); /* Reserved */
        bs_write( s, 1, 1 ); /* Full range flag */
        bs_write( s, 8, p_pal->palette[i][3] ?  /* Y value */
                  (p_pal->palette[i][0] ? p_pal->palette[i][0] : 16) : 0 );
        bs_write( s, 8, p_pal->palette[i][1] ); /* Cr value */
        bs_write( s, 8, p_pal->palette[i][2] ); /* Cb value */
        bs_write( s, 8, 0xff - p_pal->palette[i][3] ); /* T value */
    }
}
