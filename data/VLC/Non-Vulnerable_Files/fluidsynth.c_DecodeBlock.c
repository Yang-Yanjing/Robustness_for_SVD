}
static block_t *DecodeBlock (decoder_t *p_dec, block_t **pp_block)
{
    block_t *p_block;
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_out = NULL;
    if (pp_block == NULL)
        return NULL;
    p_block = *pp_block;
    if (p_block == NULL)
        return NULL;
    *pp_block = NULL;
    if (p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED))
    {
        date_Set (&p_sys->end_date, 0);
        //fluid_synth_system_reset (p_sys->synth);
        fluid_synth_program_reset (p_sys->synth);
        for (unsigned channel = 0; channel < 16; channel++)
            for (unsigned note = 0; note < 128; note++)
                fluid_synth_noteoff (p_sys->synth, channel, note);
    }
    if (p_block->i_pts > VLC_TS_INVALID && !date_Get (&p_sys->end_date))
        date_Set (&p_sys->end_date, p_block->i_pts);
    else
    if (p_block->i_pts < date_Get (&p_sys->end_date))
    {
        msg_Warn (p_dec, "MIDI message in the past?");
        goto drop;
    }
    if (p_block->i_buffer < 1)
        goto drop;
    uint8_t event = p_block->p_buffer[0];
    uint8_t channel = p_block->p_buffer[0] & 0xf;
    event &= 0xF0;
    if (event == 0xF0)
        switch (channel)
        {
            case 0:
                if (p_block->p_buffer[p_block->i_buffer - 1] != 0xF7)
                {
            case 7:
                    msg_Warn (p_dec, "fragmented SysEx not implemented");
                    goto drop;
                }
                fluid_synth_sysex (p_sys->synth, (char *)p_block->p_buffer + 1,
                                   p_block->i_buffer - 2, NULL, NULL, NULL, 0);
                break;
            case 0xF:
                fluid_synth_system_reset (p_sys->synth);
                break;
        }
    uint8_t p1 = (p_block->i_buffer > 1) ? (p_block->p_buffer[1] & 0x7f) : 0;
    uint8_t p2 = (p_block->i_buffer > 2) ? (p_block->p_buffer[2] & 0x7f) : 0;
    switch (event & 0xF0)
    {
        case 0x80:
            fluid_synth_noteoff (p_sys->synth, channel, p1);
            break;
        case 0x90:
            fluid_synth_noteon (p_sys->synth, channel, p1, p2);
            break;
        /*case 0xA0: note aftertouch not implemented */
        case 0xB0:
            fluid_synth_cc (p_sys->synth, channel, p1, p2);
            break;
        case 0xC0:
            fluid_synth_program_change (p_sys->synth, channel, p1);
            break;
        case 0xD0:
            fluid_synth_channel_pressure (p_sys->synth, channel, p1);
            break;
        case 0xE0:
            fluid_synth_pitch_bend (p_sys->synth, channel, (p2 << 7) | p1);
            break;
    }
    unsigned samples =
        (p_block->i_pts - date_Get (&p_sys->end_date)) * 441 / 10000;
    if (samples == 0)
        goto drop;
    p_out = decoder_NewAudioBuffer (p_dec, samples);
    if (p_out == NULL)
        goto drop;
    p_out->i_pts = date_Get (&p_sys->end_date );
    p_out->i_length = date_Increment (&p_sys->end_date, samples)
                      - p_out->i_pts;
    fluid_synth_write_float (p_sys->synth, samples, p_out->p_buffer, 0, 2,
                             p_out->p_buffer, 1, 2);
drop:
    block_Release (p_block);
    return p_out;
}
