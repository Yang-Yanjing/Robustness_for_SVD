}
static void decode_clut( decoder_t *p_dec, bs_t *s )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint16_t      i_segment_length;
    uint16_t      i_processed_length;
    dvbsub_clut_t *p_clut, *p_next;
    int           i_id, i_version;
    i_segment_length = bs_read( s, 16 );
    i_id             = bs_read( s, 8 );
    i_version        = bs_read( s, 4 );
    /* Check if we already have this clut */
    for( p_clut = p_sys->p_cluts; p_clut != NULL; p_clut = p_clut->p_next )
    {
        if( p_clut->i_id == i_id ) break;
    }
    /* Check version number */
    if( p_clut && ( p_clut->i_version == i_version ) )
    {
        /* Nothing to do */
        bs_skip( s, 8 * i_segment_length - 12 );
        return;
    }
    if( !p_clut )
    {
#ifdef DEBUG_DVBSUB
        msg_Dbg( p_dec, "new clut: %i", i_id );
#endif
        p_clut = malloc( sizeof( dvbsub_clut_t ) );
        if( !p_clut )
            return;
        p_clut->p_next = p_sys->p_cluts;
        p_sys->p_cluts = p_clut;
    }
    /* Initialize to default clut */
    p_next = p_clut->p_next;
    *p_clut = p_sys->default_clut;
    p_clut->p_next = p_next;
    /* We don't have this version of the CLUT: Parse it */
    p_clut->i_version = i_version;
    p_clut->i_id = i_id;
    bs_skip( s, 4 ); /* Reserved bits */
    i_processed_length = 2;
    while( i_processed_length < i_segment_length )
    {
        uint8_t y, cb, cr, t;
        uint8_t i_id;
        uint8_t i_type;
        i_id = bs_read( s, 8 );
        i_type = bs_read( s, 3 );
        bs_skip( s, 4 );
        if( bs_read( s, 1 ) )
        {
            y  = bs_read( s, 8 );
            cr = bs_read( s, 8 );
            cb = bs_read( s, 8 );
            t  = bs_read( s, 8 );
            i_processed_length += 6;
        }
        else
        {
            y  = bs_read( s, 6 ) << 2;
            cr = bs_read( s, 4 ) << 4;
            cb = bs_read( s, 4 ) << 4;
            t  = bs_read( s, 2 ) << 6;
            i_processed_length += 4;
        }
        /* We are not entirely compliant here as full transparency is indicated
         * with a luma value of zero, not a transparency value of 0xff
         * (full transparency would actually be 0xff + 1). */
        if( y == 0 )
        {
            cr = cb = 0;
            t  = 0xff;
        }
        /* According to EN 300-743 section 7.2.3 note 1, type should
         * not have more than 1 bit set to one, but some streams don't
         * respect this note. */
        if( ( i_type & 0x04 ) && ( i_id < 4 ) )
        {
            p_clut->c_2b[i_id].Y = y;
            p_clut->c_2b[i_id].Cr = cr;
            p_clut->c_2b[i_id].Cb = cb;
            p_clut->c_2b[i_id].T = t;
        }
        if( ( i_type & 0x02 ) && ( i_id < 16 ) )
        {
            p_clut->c_4b[i_id].Y = y;
            p_clut->c_4b[i_id].Cr = cr;
            p_clut->c_4b[i_id].Cb = cb;
            p_clut->c_4b[i_id].T = t;
        }
        if( i_type & 0x01 )
        {
            p_clut->c_8b[i_id].Y = y;
            p_clut->c_8b[i_id].Cr = cr;
            p_clut->c_8b[i_id].Cb = cb;
            p_clut->c_8b[i_id].T = t;
        }
    }
}
