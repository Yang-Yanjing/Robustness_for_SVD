}
static int ReadEvents (demux_t *demux, uint64_t *restrict pulse,
                       es_out_t *out)
{
    uint64_t cur_pulse = *pulse, next_pulse = UINT64_MAX;
    demux_sys_t *sys = demux->p_sys;
    for (unsigned i = 0; i < sys->trackc; i++)
    {
        mtrk_t *track = sys->trackv + i;
        while (track->next <= cur_pulse)
        {
            if (HandleMessage (demux, track, out)
             || ReadDeltaTime (demux->s, track))
            {
                msg_Err (demux, "fatal parsing error");
                return -1;
            }
        }
        if (next_pulse > track->next)
            next_pulse = track->next;
    }
    if (next_pulse != UINT64_MAX)
        date_Increment (&sys->pts, next_pulse - cur_pulse);
    *pulse = next_pulse;
    return 0;
}
