}
static uint8_t *CAPMTES( system_ids_t *p_ids, uint8_t *p_capmt,
                         int i_capmt_size, uint8_t i_type, uint16_t i_pid,
                         int i_size, dvbpsi_descriptor_t *p_dr,
                         uint8_t i_cmd )
{
    uint8_t *p_data;
    int i;
    if ( i_size )
        p_data = xrealloc( p_capmt, i_capmt_size + 6 + i_size );
    else
        p_data = xrealloc( p_capmt, i_capmt_size + 5 );
    i = i_capmt_size;
    p_data[i] = i_type;
    p_data[i + 1] = i_pid >> 8;
    p_data[i + 2] = i_pid & 0xff;
    if ( i_size )
    {
        p_data[i + 3] = (i_size + 1) >> 8;
        p_data[i + 4] = (i_size + 1) & 0xff;
        p_data[i + 5] = i_cmd;
        i += 6;
        while ( p_dr != NULL )
        {
            if( p_dr->i_tag == 0x9 )
            {
                uint16_t i_sysid = ((uint16_t)p_dr->p_data[0] << 8)
                                    | p_dr->p_data[1];
                if ( CheckSystemID( p_ids, i_sysid ) )
                {
                    p_data[i] = 0x9;
                    p_data[i + 1] = p_dr->i_length;
                    memcpy( &p_data[i + 2], p_dr->p_data, p_dr->i_length );
                    i += p_dr->i_length + 2;
                }
            }
            p_dr = p_dr->p_next;
        }
    }
    else
    {
        p_data[i + 3] = 0;
        p_data[i + 4] = 0;
    }
    return p_data;
}
