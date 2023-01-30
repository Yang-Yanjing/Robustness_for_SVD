}
static int DiscoverAVC( access_t *p_access, int* port, uint64_t guid )
{
    rom1394_directory rom_dir;
    raw1394handle_t handle = NULL;
    int device = -1;
    int i, j = 0;
    int m = Raw1394GetNumPorts( p_access );
    if( *port >= 0 )
    {
        /* search on explicit port */
        j = *port;
        m = *port + 1;
    }
    for( ; j < m && device == -1; j++ )
    {
        handle = Raw1394Open( p_access, j );
        if( !handle )
            return -1;
        for( i = 0; i < raw1394_get_nodecount( handle ); ++i )
        {
            if( guid != 0 )
            {
                /* select explicitly by GUID */
                if( guid == rom1394_get_guid( handle, i ) )
                {
                    device = i;
                    *port = j;
                    break;
                }
            }
            else
            {
                /* select first AV/C Tape Reccorder Player node */
                if( rom1394_get_directory( handle, i, &rom_dir ) < 0 )
                {
                    msg_Err( p_access, "error reading config rom directory for node %d", i );
                    continue;
                }
                if( ( rom1394_get_node_type( &rom_dir ) == ROM1394_NODE_TYPE_AVC ) &&
                        avc1394_check_subunit_type( handle, i, AVC1394_SUBUNIT_TYPE_VCR ) )
                {
                    device = i;
                    *port = j;
                    break;
                }
            }
        }
        Raw1394Close( handle );
    }
    return device;
}
