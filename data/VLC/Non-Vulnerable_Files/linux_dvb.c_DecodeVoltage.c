 *****************************************************************************/
static fe_sec_voltage_t DecodeVoltage( access_t *p_access )
{
    switch( var_GetInteger( p_access, "dvb-voltage" ) )
    {
        case 0:  return SEC_VOLTAGE_OFF;
        case 13: return SEC_VOLTAGE_13;
        case 18: return SEC_VOLTAGE_18;
        default: return SEC_VOLTAGE_OFF;
    }
}
