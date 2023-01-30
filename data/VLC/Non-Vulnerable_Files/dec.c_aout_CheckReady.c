}
static int aout_CheckReady (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    int restart = atomic_exchange (&owner->restart, 0);
    if (unlikely(restart))
    {
        if (owner->mixer_format.i_format)
            aout_FiltersDelete (aout, owner->filters);
        if (restart & AOUT_RESTART_OUTPUT)
        {   /* Reinitializes the output */
            msg_Dbg (aout, "restarting output...");
            if (owner->mixer_format.i_format)
                aout_OutputDelete (aout);
            owner->mixer_format = owner->input_format;
            if (aout_OutputNew (aout, &owner->mixer_format))
                owner->mixer_format.i_format = 0;
            aout_volume_SetFormat (owner->volume,
                                   owner->mixer_format.i_format);
        }
        msg_Dbg (aout, "restarting filters...");
        owner->sync.end = VLC_TS_INVALID;
        owner->sync.resamp_type = AOUT_RESAMPLING_NONE;
        if (owner->mixer_format.i_format)
        {
            owner->filters = aout_FiltersNew (aout, &owner->input_format,
                                              &owner->mixer_format,
                                              &owner->request_vout);
            if (owner->filters == NULL)
            {
                aout_OutputDelete (aout);
                owner->mixer_format.i_format = 0;
            }
        }
        /* TODO: This would be a good time to call clean up any video output
         * left over by an audio visualization:
        input_resource_TerminatVout(MAGIC HERE); */
    }
    return (owner->mixer_format.i_format) ? 0 : -1;
}
