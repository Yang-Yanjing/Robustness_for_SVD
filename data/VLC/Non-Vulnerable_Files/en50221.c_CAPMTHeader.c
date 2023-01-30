}
static uint8_t *CAPMTHeader( system_ids_t *p_ids, uint8_t i_list_mgt,
                             uint16_t i_program_number, uint8_t i_version,
                             int i_size, dvbpsi_descriptor_t *p_dr,
                             uint8_t i_cmd )
{
    uint8_t *p_data;
    if ( i_size )
        p_data = xmalloc( 7 + i_size );
    else
        p_data = xmalloc( 6 );
    p_data[0] = i_list_mgt;
    p_data[1] = i_program_number >> 8;
    p_data[2] = i_program_number & 0xff;
    p_data[3] = ((i_version & 0x1f) << 1) | 0x1;
    if ( i_size )
    {
        int i;
        p_data[4] = (i_size + 1) >> 8;
        p_data[5] = (i_size + 1) & 0xff;
        p_data[6] = i_cmd;
        i = 7;
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
//                    p_data[i+4] &= 0x1f;
                    i += p_dr->i_length + 2;
                }
            }
            p_dr = p_dr->p_next;
        }
    }
    else
    {
        p_data[4] = 0;
        p_data[5] = 0;
    }
    return p_data;
}
