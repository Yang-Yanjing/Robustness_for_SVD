 *****************************************************************************/
static void mosaic_ParseSetOffsets( vlc_object_t *p_this,
                                      filter_sys_t *p_sys,
                                      char *psz_offsets )
{
    if( *psz_offsets )
    {
        char *psz_end = NULL;
        int i_index = 0;
        do
        {
            i_index++;
            p_sys->pi_x_offsets = xrealloc( p_sys->pi_x_offsets,
                                                   i_index * sizeof(int) );
            p_sys->pi_x_offsets[i_index - 1] = atoi( psz_offsets );
            psz_end = strchr( psz_offsets, ',' );
            psz_offsets = psz_end + 1;
            p_sys->pi_y_offsets = xrealloc( p_sys->pi_y_offsets,
                                                   i_index * sizeof(int) );
            p_sys->pi_y_offsets[i_index - 1] = atoi( psz_offsets );
            psz_end = strchr( psz_offsets, ',' );
            psz_offsets = psz_end + 1;
            msg_Dbg( p_this, CFG_PREFIX "offset: id %d, x=%d, y=%d",
                     i_index, p_sys->pi_x_offsets[i_index - 1],
                              p_sys->pi_y_offsets[i_index - 1]  );
        } while( psz_end );
        p_sys->i_offsets_length = i_index;
    }
}
