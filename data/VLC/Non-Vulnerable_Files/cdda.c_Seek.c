 ****************************************************************************/
static int Seek( access_t *p_access, uint64_t i_pos )
{
    access_sys_t *p_sys = p_access->p_sys;
    /* Next sector to read */
    p_sys->i_sector = p_sys->i_first_sector + i_pos / CDDA_DATA_SIZE;
    assert( p_sys->i_sector >= 0 );
    p_access->info.i_pos = i_pos;
    return VLC_SUCCESS;
}
