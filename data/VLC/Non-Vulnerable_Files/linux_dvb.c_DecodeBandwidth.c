 *****************************************************************************/
static fe_bandwidth_t DecodeBandwidth( access_t *p_access )
{
    fe_bandwidth_t      fe_bandwidth = 0;
    int i_bandwidth = var_GetInteger( p_access, "dvb-bandwidth" );
    msg_Dbg( p_access, "using bandwidth=%d", i_bandwidth );
    switch( i_bandwidth )
    {
        case 0: fe_bandwidth = BANDWIDTH_AUTO; break;
        case 6: fe_bandwidth = BANDWIDTH_6_MHZ; break;
        case 7: fe_bandwidth = BANDWIDTH_7_MHZ; break;
        case 8: fe_bandwidth = BANDWIDTH_8_MHZ; break;
        default:
            msg_Dbg( p_access, "terrestrial dvb has bandwidth not set, using auto" );
            fe_bandwidth = BANDWIDTH_AUTO;
            break;
    }
    return fe_bandwidth;
}
