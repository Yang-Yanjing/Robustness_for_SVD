}
int dvb_set_sec (dvb_device_t *d, uint64_t freq_Hz, char pol,
                 uint32_t lowf, uint32_t highf, uint32_t switchf)
{
    uint32_t freq = freq_Hz / 1000;
    /* Always try to configure high voltage, but only warn on enable failure */
    int val = var_InheritBool (d->obj, "dvb-high-voltage");
    if (ioctl (d->frontend, FE_ENABLE_HIGH_LNB_VOLTAGE, &val) < 0 && val)
        msg_Err (d->obj, "cannot enable high LNB voltage: %s",
                 vlc_strerror_c(errno));
    /* Windows BDA exposes a higher-level API covering LNB oscillators.
     * So lets pretend this is platform-specific stuff and do it here. */
    if (!lowf)
    {   /* Default oscillator frequencies */
        static const struct
        {
             uint16_t min, max, low, high;
        } tab[] =
        {    /*  min    max    low   high */
             { 10700, 13250,  9750, 10600 }, /* Ku band */
             {  4500,  4800,  5950,     0 }, /* C band (high) */
             {  3400,  4200,  5150,     0 }, /* C band (low) */
             {  2500,  2700,  3650,     0 }, /* S band */
             {   950,  2150,     0,     0 }, /* adjusted IF (L band) */
        };
        uint_fast16_t mHz = freq / 1000;
        for (size_t i = 0; i < sizeof (tab) / sizeof (tab[0]); i++)
             if (mHz >= tab[i].min && mHz <= tab[i].max)
             {
                 lowf = tab[i].low * 1000;
                 highf = tab[i].high * 1000;
                 goto known;
             }
        msg_Err (d->obj, "no known band for frequency %u kHz", freq);
known:
        msg_Dbg (d->obj, "selected LNB low: %u kHz, LNB high: %u kHz",
                 lowf, highf);
    }
    /* Use high oscillator frequency? */
    bool high = highf != 0 && freq > switchf;
    freq -= high ? highf : lowf;
    if ((int32_t)freq < 0)
        freq *= -1;
    assert (freq < 0x7fffffff);
    int tone;
    switch (var_InheritInteger (d->obj, "dvb-tone"))
    {
        case 0:  tone = SEC_TONE_OFF; break;
        case 1:  tone = SEC_TONE_ON;  break;
        default: tone = high ? SEC_TONE_ON : SEC_TONE_OFF;
    }
    /*** LNB selection / DiSEqC ***/
    unsigned voltage = dvb_parse_polarization (pol);
    if (dvb_set_props (d, 2, DTV_TONE, SEC_TONE_OFF, DTV_VOLTAGE, voltage))
        return -1;
    unsigned satno = var_InheritInteger (d->obj, "dvb-satno");
    if (satno > 0)
    {
#undef msleep /* we know what we are doing! */
        /* DiSEqC Bus Specification:
 http://www.eutelsat.com/satellites/pdf/Diseqc/Reference%20docs/bus_spec.pdf */
        /* DiSEqC 1.1 */
        struct dvb_diseqc_master_cmd uncmd;
        /* DiSEqC 1.0 */
        struct dvb_diseqc_master_cmd cmd;
        satno = (satno - 1) & 3;
        cmd.msg[0] = 0xE0; /* framing: master, no reply, 1st TX */
        cmd.msg[1] = 0x10; /* address: all LNB/switch */
        cmd.msg[2] = 0x38; /* command: Write Port Group 0 (committed) */
        cmd.msg[3] = 0xF0  /* data[0]: clear all bits */
                   | (satno << 2) /* LNB (A, B, C or D) */
                   | ((voltage == SEC_VOLTAGE_18) << 1) /* polarization */
                   | (tone == SEC_TONE_ON); /* option */
        cmd.msg[4] = cmd.msg[5] = 0; /* unused */
        cmd.msg_len = 4; /* length */
        msleep (15000); /* wait 15 ms before DiSEqC command */
        unsigned uncommitted = var_InheritInteger (d->obj, "dvb-uncommitted");
        if (uncommitted > 0)
        {
          uncommitted = (uncommitted - 1) & 3;
          uncmd.msg[0] = 0xE0; /* framing: master, no reply, 1st TX */
          uncmd.msg[1] = 0x10; /* address: all LNB/switch */
          uncmd.msg[2] = 0x39; /* command: Write Port Group 1 (uncommitted) */
          uncmd.msg[3] = 0xF0  /* data[0]: clear all bits */
                       | (uncommitted << 2) /* LNB (A, B, C or D) */
                       | ((voltage == SEC_VOLTAGE_18) << 1) /* polarization */
                       | (tone == SEC_TONE_ON); /* option */
          uncmd.msg[4] = uncmd.msg[5] = 0; /* unused */
          uncmd.msg_len = 4; /* length */
          if (ioctl (d->frontend, FE_DISEQC_SEND_MASTER_CMD, &uncmd) < 0)
          {
              msg_Err (d->obj, "cannot send uncommitted DiSEqC command: %s",
                       vlc_strerror_c(errno));
              return -1;
          }
          /* Repeat uncommitted command */
          uncmd.msg[0] = 0xE1; /* framing: master, no reply, repeated TX */
          if (ioctl (d->frontend, FE_DISEQC_SEND_MASTER_CMD, &uncmd) < 0)
          {
              msg_Err (d->obj,
                       "cannot send repeated uncommitted DiSEqC command: %s",
                       vlc_strerror_c(errno));
              return -1;
          }
          msleep(125000); /* wait 125 ms before committed DiSEqC command */
        }
        if (ioctl (d->frontend, FE_DISEQC_SEND_MASTER_CMD, &cmd) < 0)
        {
            msg_Err (d->obj, "cannot send committed DiSEqC command: %s",
                     vlc_strerror_c(errno));
            return -1;
        }
        msleep (54000 + 15000);
        /* Mini-DiSEqC */
        satno &= 1;
        if (ioctl (d->frontend, FE_DISEQC_SEND_BURST,
                   satno ? SEC_MINI_B : SEC_MINI_A) < 0)
        {
            msg_Err (d->obj, "cannot send Mini-DiSEqC tone burst: %s",
                     vlc_strerror_c(errno));
            return -1;
        }
        msleep (15000);
    }
    /* Continuous tone (to select high oscillator frequency) */
    return dvb_set_props (d, 2, DTV_FREQUENCY, freq, DTV_TONE, tone);
}
