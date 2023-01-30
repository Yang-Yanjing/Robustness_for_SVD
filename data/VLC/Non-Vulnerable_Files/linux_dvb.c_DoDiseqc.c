};
static int DoDiseqc( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    int i_val;
    bool b_val;
    int i_frequency, i_lnb_slof;
    fe_sec_voltage_t fe_voltage;
    fe_sec_tone_mode_t fe_tone;
    i_frequency = var_GetInteger( p_access, "dvb-frequency" );
    i_lnb_slof = var_GetInteger( p_access, "dvb-lnb-slof" );
    i_val = var_GetInteger( p_access, "dvb-tone" );
    if( i_val == -1 /* auto */ )
    {
        if( i_frequency >= i_lnb_slof )
            i_val = 1;
        else
            i_val = 0;
        var_SetInteger( p_access, "dvb-tone", i_val );
    }
    fe_voltage = DecodeVoltage( p_access );
    fe_tone = DecodeTone( p_access );
    /* Switch off continuous tone. */
    if( ioctl( p_sys->i_frontend_handle, FE_SET_TONE, SEC_TONE_OFF ) < 0 )
    {
        msg_Err( p_access, "switching tone %s error: %s", "off",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    /* Configure LNB voltage. */
    if( ioctl( p_sys->i_frontend_handle, FE_SET_VOLTAGE, fe_voltage ) < 0 )
    {
        msg_Err( p_access, "voltage error: %s", vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    b_val = var_GetBool( p_access, "dvb-high-voltage" );
    if( ioctl( p_sys->i_frontend_handle,
               FE_ENABLE_HIGH_LNB_VOLTAGE, b_val ) < 0 && b_val )
    {
        msg_Err( p_access, "high LNB voltage error: %s",
                 vlc_strerror_c(errno) );
    }
    /* Wait for at least 15 ms. */
    msleep(15000);
    i_val = var_GetInteger( p_access, "dvb-satno" );
    if( i_val > 0 && i_val < 5 )
    {
        /* digital satellite equipment control,
         * specification is available from http://www.eutelsat.com/
         */
        /* 1.x compatible equipment */
        struct diseqc_cmd_t cmd =  { {{0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00}, 4}, 0 };
        /* param: high nibble: reset bits, low nibble set bits,
         * bits are: option, position, polarization, band
         */
        cmd.cmd.msg[3] = 0xf0 /* reset bits */
                          | (((i_val - 1) * 4) & 0xc)
                          | (fe_voltage == SEC_VOLTAGE_13 ? 0 : 2)
                          | (fe_tone == SEC_TONE_ON ? 1 : 0);
        if( ioctl( p_sys->i_frontend_handle, FE_DISEQC_SEND_MASTER_CMD,
                   &cmd.cmd ) )
        {
            msg_Err( p_access, "master command sending error: %s",
                     vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        msleep(15000 + cmd.wait * 1000);
        /* A or B simple diseqc ("diseqc-compatible") */
        if( ioctl( p_sys->i_frontend_handle, FE_DISEQC_SEND_BURST,
                  ((i_val - 1) % 2) ? SEC_MINI_B : SEC_MINI_A ) )
        {
            msg_Err( p_access, "burst sending error: %s",
                     vlc_strerror_c(errno) );
            return VLC_EGENERIC;
        }
        msleep(15000);
    }
    if( ioctl( p_sys->i_frontend_handle, FE_SET_TONE, fe_tone ) )
    {
        msg_Err( p_access, "switching tone %s error: %s",
                 (fe_tone == SEC_TONE_ON) ? "on" : "off",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    msleep(50000);
    return 0;
}
