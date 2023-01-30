 *****************************************************************************/
static fe_spectral_inversion_t DecodeInversion( access_t *p_access )
{
    int i_val;
    fe_spectral_inversion_t fe_inversion = 0;
    i_val = var_GetInteger( p_access, "dvb-inversion" );
    msg_Dbg( p_access, "using inversion=%d", i_val );
    switch( i_val )
    {
        case 0: fe_inversion = INVERSION_OFF; break;
        case 1: fe_inversion = INVERSION_ON; break;
        case 2: fe_inversion = INVERSION_AUTO; break;
        default:
            msg_Dbg( p_access, "dvb has inversion not set, using auto");
            fe_inversion = INVERSION_AUTO;
            break;
    }
    return fe_inversion;
}
