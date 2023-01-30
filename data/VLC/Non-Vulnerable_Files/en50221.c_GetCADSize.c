 *****************************************************************************/
static int GetCADSize( system_ids_t *p_ids, dvbpsi_descriptor_t *p_dr )
{
    int i_cad_size = 0;
    while ( p_dr != NULL )
    {
        if( p_dr->i_tag == 0x9 )
        {
            uint16_t i_sysid = ((uint16_t)p_dr->p_data[0] << 8)
                                    | p_dr->p_data[1];
            if ( CheckSystemID( p_ids, i_sysid ) )
                i_cad_size += p_dr->i_length + 2;
        }
        p_dr = p_dr->p_next;
    }
    return i_cad_size;
}
