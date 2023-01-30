 *****************************************************************************/
static block_t *Block( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    int i_blocks = CDDA_BLOCKS_ONCE;
    block_t *p_block;
    if( p_sys->i_track < 0 ) p_access->info.b_eof = true;
    /* Check end of file */
    if( p_access->info.b_eof ) return NULL;
    if( !p_sys->b_header )
    {
        /* Return only the header */
        p_block = block_Alloc( sizeof( WAVEHEADER ) );
        memcpy( p_block->p_buffer, &p_sys->waveheader, sizeof(WAVEHEADER) );
        p_sys->b_header = true;
        return p_block;
    }
    if( p_sys->i_sector >= p_sys->i_last_sector )
    {
        p_access->info.b_eof = true;
        return NULL;
    }
    /* Don't read too far */
    if( p_sys->i_sector + i_blocks >= p_sys->i_last_sector )
        i_blocks = p_sys->i_last_sector - p_sys->i_sector;
    /* Do the actual reading */
    if( !( p_block = block_Alloc( i_blocks * CDDA_DATA_SIZE ) ) )
    {
        msg_Err( p_access, "cannot get a new block of size: %i",
                 i_blocks * CDDA_DATA_SIZE );
        return NULL;
    }
    if( ioctl_ReadSectors( VLC_OBJECT(p_access), p_sys->vcddev,
            p_sys->i_sector, p_block->p_buffer, i_blocks, CDDA_TYPE ) < 0 )
    {
        msg_Err( p_access, "cannot read sector %i", p_sys->i_sector );
        block_Release( p_block );
        /* Try to skip one sector (in case of bad sectors) */
        p_sys->i_sector++;
        p_access->info.i_pos += CDDA_DATA_SIZE;
        return NULL;
    }
    /* Update a few values */
    p_sys->i_sector += i_blocks;
    p_access->info.i_pos += p_block->i_buffer;
    return p_block;
}
