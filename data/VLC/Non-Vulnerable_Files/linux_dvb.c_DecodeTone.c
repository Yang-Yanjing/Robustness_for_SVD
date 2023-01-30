}
static fe_sec_tone_mode_t DecodeTone( access_t *p_access )
{
    switch( var_GetInteger( p_access, "dvb-tone" ) )
    {
        case 0:  return SEC_TONE_OFF;
        case 1:  return SEC_TONE_ON;
        default: return SEC_TONE_OFF;
    }
}
