}
static fe_transmit_mode_t DecodeTransmission( access_t *p_access )
{
    fe_transmit_mode_t  fe_transmission = 0;
    int i_transmission = var_GetInteger( p_access, "dvb-transmission" );
    msg_Dbg( p_access, "using transmission=%d", i_transmission );
    switch( i_transmission )
    {
        case 0: fe_transmission = TRANSMISSION_MODE_AUTO; break;
        case 2: fe_transmission = TRANSMISSION_MODE_2K; break;
        case 8: fe_transmission = TRANSMISSION_MODE_8K; break;
        default:
            msg_Dbg( p_access, "terrestrial dvb has transmission mode not set, using auto");
            fe_transmission = TRANSMISSION_MODE_AUTO;
            break;
    }
    return fe_transmission;
}
