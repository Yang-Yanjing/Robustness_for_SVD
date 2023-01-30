 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t p_buffer[DVD_VIDEO_LB_LEN * DVD_BLOCK_READ_ONCE];
    int i_blocks_once, i_read;
    int i;
    /*
     * Playback by cell in this pgc, starting at the cell for our chapter.
     */
    /*
     * Check end of pack, and select the following one
     */
    if( !p_sys->i_pack_len )
    {
        /* Read NAV packet */
        if( DVDReadBlocks( p_sys->p_title, p_sys->i_next_vobu,
                           1, p_buffer ) != 1 )
        {
            msg_Err( p_demux, "read failed for block %d", p_sys->i_next_vobu );
            dialog_Fatal( p_demux, _("Playback failure"),
                          _("DVDRead could not read block %d."),
                          p_sys->i_next_vobu );
            return -1;
        }
        /* Basic check to be sure we don't have a empty title
         * go to next title if so */
        //assert( p_buffer[41] == 0xbf && p_buffer[1027] == 0xbf );
        /* Parse the contained dsi packet */
        DvdReadHandleDSI( p_demux, p_buffer );
        /* End of title */
        if( p_sys->i_cur_cell >= p_sys->p_cur_pgc->nr_of_cells )
        {
            int k = p_sys->i_title;
            /* Looking for a not broken title */
            while( k < p_sys->i_titles && DvdReadSetArea( p_demux, ++k, 0, -1 ) != VLC_SUCCESS )
            {
                msg_Err(p_demux, "Failed next title, trying another: %i", k );
                if( k >= p_sys->i_titles )
                    return 0; // EOF
            }
        }
        if( p_sys->i_pack_len >= 1024 )
        {
            msg_Err( p_demux, "i_pack_len >= 1024 (%i). "
                     "This shouldn't happen!", p_sys->i_pack_len );
            return 0; /* EOF */
        }
        /* FIXME: Ugly kludge: we send the pack block to the input for it
         * sometimes has a zero scr and restart the sync */
        p_sys->i_cur_block++;
        p_sys->i_title_offset++;
        DemuxBlock( p_demux, p_buffer, DVD_VIDEO_LB_LEN );
    }
    if( p_sys->i_cur_cell >= p_sys->p_cur_pgc->nr_of_cells )
    {
        int k = p_sys->i_title;
        /* Looking for a not broken title */
        while( k < p_sys->i_titles && DvdReadSetArea( p_demux, ++k, 0, -1 ) != VLC_SUCCESS )
        {
            msg_Err(p_demux, "Failed next title, trying another: %i", k );
            if( k >= p_sys->i_titles )
                return 0; // EOF
        }
    }
    /*
     * Read actual data
     */
    i_blocks_once = __MIN( p_sys->i_pack_len, DVD_BLOCK_READ_ONCE );
    p_sys->i_pack_len -= i_blocks_once;
    /* Reads from DVD */
    i_read = DVDReadBlocks( p_sys->p_title, p_sys->i_cur_block,
                            i_blocks_once, p_buffer );
    if( i_read != i_blocks_once )
    {
        msg_Err( p_demux, "read failed for %d/%d blocks at 0x%02x",
                 i_read, i_blocks_once, p_sys->i_cur_block );
        dialog_Fatal( p_demux, _("Playback failure"),
                        _("DVDRead could not read %d/%d blocks at 0x%02x."),
                        i_read, i_blocks_once, p_sys->i_cur_block );
        return -1;
    }
    p_sys->i_cur_block += i_read;
    p_sys->i_title_offset += i_read;
#if 0
    msg_Dbg( p_demux, "i_blocks: %d len: %d current: 0x%02x",
             i_read, p_sys->i_pack_len, p_sys->i_cur_block );
#endif
    for( i = 0; i < i_read; i++ )
    {
        DemuxBlock( p_demux, p_buffer + i * DVD_VIDEO_LB_LEN,
                    DVD_VIDEO_LB_LEN );
    }
#undef p_pgc
    return 1;
}
