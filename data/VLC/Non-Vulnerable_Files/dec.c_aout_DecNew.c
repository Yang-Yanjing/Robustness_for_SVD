 */
int aout_DecNew( audio_output_t *p_aout,
                 const audio_sample_format_t *p_format,
                 const audio_replay_gain_t *p_replay_gain,
                 const aout_request_vout_t *p_request_vout )
{
    /* Sanitize audio format */
    if( p_format->i_channels != aout_FormatNbChannels( p_format ) )
    {
        msg_Err( p_aout, "incompatible audio channels count with layout mask" );
        return -1;
    }
    if( p_format->i_rate > 352800 )
    {
        msg_Err( p_aout, "excessive audio sample frequency (%u)",
                 p_format->i_rate );
        return -1;
    }
    if( p_format->i_rate < 4000 )
    {
        msg_Err( p_aout, "too low audio sample frequency (%u)",
                 p_format->i_rate );
        return -1;
    }
    aout_owner_t *owner = aout_owner(p_aout);
    /* TODO: reduce lock scope depending on decoder's real need */
    aout_OutputLock (p_aout);
    var_Destroy( p_aout, "stereo-mode" );
    /* Create the audio output stream */
    owner->volume = aout_volume_New (p_aout, p_replay_gain);
    atomic_store (&owner->restart, 0);
    owner->input_format = *p_format;
    owner->mixer_format = owner->input_format;
    owner->request_vout = *p_request_vout;
    if (aout_OutputNew (p_aout, &owner->mixer_format))
        goto error;
    aout_volume_SetFormat (owner->volume, owner->mixer_format.i_format);
    /* Create the audio filtering "input" pipeline */
    owner->filters = aout_FiltersNew (p_aout, p_format, &owner->mixer_format,
                                      &owner->request_vout);
    if (owner->filters == NULL)
    {
        aout_OutputDelete (p_aout);
error:
        aout_volume_Delete (owner->volume);
        owner->volume = NULL;
        aout_OutputUnlock (p_aout);
        return -1;
    }
    owner->sync.end = VLC_TS_INVALID;
    owner->sync.resamp_type = AOUT_RESAMPLING_NONE;
    owner->sync.discontinuity = true;
    aout_OutputUnlock (p_aout);
    atomic_init (&owner->buffers_lost, 0);
    return 0;
}
 */
int aout_DecNew( audio_output_t *p_aout,
                 const audio_sample_format_t *p_format,
                 const audio_replay_gain_t *p_replay_gain,
                 const aout_request_vout_t *p_request_vout )
{
    /* Sanitize audio format */
    if( p_format->i_channels != aout_FormatNbChannels( p_format ) )
    {
        msg_Err( p_aout, "incompatible audio channels count with layout mask" );
        return -1;
    }
