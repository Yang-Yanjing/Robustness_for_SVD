 ****************************************************************************/
static block_t *Encode( encoder_t *p_enc, subpicture_t *p_subpic )
{
    subpicture_t *p_temp = NULL;
    subpicture_region_t *p_region = NULL;
    bs_t bits, *s = &bits;
    block_t *p_block;
    if( !p_subpic || !p_subpic->p_region ) return NULL;
    /* FIXME: this is a hack to convert VLC_CODEC_YUVA into
     *  VLC_CODEC_YUVP
     */
    p_region = p_subpic->p_region;
    if( p_region->fmt.i_chroma == VLC_CODEC_YUVA )
    {
        p_temp = YuvaYuvp( p_subpic );
        if( !p_temp )
        {
            msg_Err( p_enc, "no picture in subpicture" );
            return NULL;
        }
        p_region = p_subpic->p_region;
    }
    /* Sanity check */
    if( !p_region ) return NULL;
    if( ( p_region->fmt.i_chroma != VLC_CODEC_TEXT ) &&
        ( p_region->fmt.i_chroma != VLC_CODEC_YUVP ) )
    {
        char psz_fourcc[5];
        memset( &psz_fourcc, 0, sizeof( psz_fourcc ) );
        vlc_fourcc_to_char( p_region->fmt.i_chroma, &psz_fourcc );
        msg_Err( p_enc, "chroma %4.4s not supported", psz_fourcc );
        return NULL;
    }
    if( p_region->fmt.p_palette )
    {
        switch( p_region->fmt.p_palette->i_entries )
        {
            case 0:
            case 4:
            case 16:
            case 256:
                break;
            default:
                msg_Err( p_enc, "subpicture palette (%d) not handled",
                            p_region->fmt.p_palette->i_entries );
                return NULL;
        }
    }
    /* End of hack */
#ifdef DEBUG_DVBSUB
    msg_Dbg( p_enc, "encoding subpicture" );
#endif
    p_block = block_Alloc( 64000 );
    bs_init( s, p_block->p_buffer, p_block->i_buffer );
    bs_write( s, 8, 0x20 ); /* Data identifier */
    bs_write( s, 8, 0x0 );  /* Subtitle stream id */
    encode_page_composition( p_enc, s, p_subpic );
    encode_region_composition( p_enc, s, p_subpic );
    encode_clut( p_enc, s, p_subpic );
    encode_object( p_enc, s, p_subpic );
    /* End of display */
    bs_write( s, 8, 0x0f ); /* Sync byte */
    bs_write( s, 8, DVBSUB_ST_ENDOFDISPLAY ); /* Segment type */
    bs_write( s, 16, 1 );  /* Page id */
    bs_write( s, 16, 0 );  /* Segment length */
    bs_write( s, 8, 0xff );/* End marker */
    p_block->i_buffer = bs_pos( s ) / 8;
    p_block->i_pts = p_block->i_dts = p_subpic->i_start;
    if( !p_subpic->b_ephemer && ( p_subpic->i_stop > p_subpic->i_start ) )
    {
        block_t *p_block_stop;
        p_block->i_length = p_subpic->i_stop - p_subpic->i_start;
        /* Send another (empty) subtitle to signal the end of display */
        p_block_stop = block_Alloc( 64000 );
        bs_init( s, p_block_stop->p_buffer, p_block_stop->i_buffer );
        bs_write( s, 8, 0x20 ); /* Data identifier */
        bs_write( s, 8, 0x0 );  /* Subtitle stream id */
        encode_page_composition( p_enc, s, 0 );
        bs_write( s, 8, 0x0f ); /* Sync byte */
        bs_write( s, 8, DVBSUB_ST_ENDOFDISPLAY ); /* Segment type */
        bs_write( s, 16, 1 );  /* Page id */
        bs_write( s, 16, 0 );  /* Segment length */
        bs_write( s, 8, 0xff );/* End marker */
        p_block_stop->i_buffer = bs_pos( s ) / 8;
        p_block_stop->i_pts = p_block_stop->i_dts = p_subpic->i_stop;
        block_ChainAppend( &p_block, p_block_stop );
        p_block_stop->i_length = 100000; /* p_subpic->i_stop - p_subpic->i_start; */
    }
#ifdef DEBUG_DVBSUB
    msg_Dbg( p_enc, "subpicture encoded properly" );
#endif
    return p_block;
}
