}
static void aout_DecSynchronize (audio_output_t *aout, mtime_t dec_pts,
                                 int input_rate)
{
    aout_owner_t *owner = aout_owner (aout);
    mtime_t drift;
    /**
     * Depending on the drift between the actual and intended playback times,
     * the audio core may ignore the drift, trigger upsampling or downsampling,
     * insert silence or even discard samples.
     * Future VLC versions may instead adjust the input rate.
     *
     * The audio output plugin is responsible for estimating its actual
     * playback time, or rather the estimated time when the next sample will
     * be played. (The actual playback time is always the current time, that is
     * to say mdate(). It is not an useful statistic.)
     *
     * Most audio output plugins can estimate the delay until playback of
     * the next sample to be written to the buffer, or equally the time until
     * all samples in the buffer will have been played. Then:
     *    pts = mdate() + delay
     */
    if (aout_OutputTimeGet (aout, &drift) != 0)
        return; /* nothing can be done if timing is unknown */
    drift += mdate () - dec_pts;
    /* Late audio output.
     * This can happen due to insufficient caching, scheduling jitter
     * or bug in the decoder. Ideally, the output would seek backward. But that
     * is not portable, not supported by some hardware and often unsafe/buggy
     * where supported. The other alternative is to flush the buffers
     * completely. */
    if (drift > (owner->sync.discontinuity ? 0
                  : +3 * input_rate * AOUT_MAX_PTS_DELAY / INPUT_RATE_DEFAULT))
    {
        if (!owner->sync.discontinuity)
            msg_Warn (aout, "playback way too late (%"PRId64"): "
                      "flushing buffers", drift);
        else
            msg_Dbg (aout, "playback too late (%"PRId64"): "
                     "flushing buffers", drift);
        aout_OutputFlush (aout, false);
        aout_StopResampling (aout);
        owner->sync.end = VLC_TS_INVALID;
        owner->sync.discontinuity = true;
        /* Now the output might be too early... Recheck. */
        if (aout_OutputTimeGet (aout, &drift) != 0)
            return; /* nothing can be done if timing is unknown */
        drift += mdate () - dec_pts;
    }
    /* Early audio output.
     * This is rare except at startup when the buffers are still empty. */
    if (drift < (owner->sync.discontinuity ? 0
                : -3 * input_rate * AOUT_MAX_PTS_ADVANCE / INPUT_RATE_DEFAULT))
    {
        if (!owner->sync.discontinuity)
            msg_Warn (aout, "playback way too early (%"PRId64"): "
                      "playing silence", drift);
        aout_DecSilence (aout, -drift, dec_pts);
        aout_StopResampling (aout);
        owner->sync.discontinuity = true;
        drift = 0;
    }
    /* Resampling */
    if (drift > +AOUT_MAX_PTS_DELAY
     && owner->sync.resamp_type != AOUT_RESAMPLING_UP)
    {
        msg_Warn (aout, "playback too late (%"PRId64"): up-sampling",
                  drift);
        owner->sync.resamp_type = AOUT_RESAMPLING_UP;
        owner->sync.resamp_start_drift = +drift;
    }
    if (drift < -AOUT_MAX_PTS_ADVANCE
     && owner->sync.resamp_type != AOUT_RESAMPLING_DOWN)
    {
        msg_Warn (aout, "playback too early (%"PRId64"): down-sampling",
                  drift);
        owner->sync.resamp_type = AOUT_RESAMPLING_DOWN;
        owner->sync.resamp_start_drift = -drift;
    }
    if (owner->sync.resamp_type == AOUT_RESAMPLING_NONE)
        return; /* Everything is fine. Nothing to do. */
    if (llabs (drift) > 2 * owner->sync.resamp_start_drift)
    {   /* If the drift is ever increasing, then something is seriously wrong.
         * Cease resampling and hope for the best. */
        msg_Warn (aout, "timing screwed (drift: %"PRId64" us): "
                  "stopping resampling", drift);
        aout_StopResampling (aout);
        return;
    }
    /* Resampling has been triggered earlier. This checks if it needs to be
     * increased or decreased. Resampling rate changes must be kept slow for
     * the comfort of listeners. */
    int adj = (owner->sync.resamp_type == AOUT_RESAMPLING_UP) ? +2 : -2;
    if (2 * llabs (drift) <= owner->sync.resamp_start_drift)
        /* If the drift has been reduced from more than half its initial
         * value, then it is time to switch back the resampling direction. */
        adj *= -1;
    if (!aout_FiltersAdjustResampling (owner->filters, adj))
    {   /* Everything is back to normal: stop resampling. */
        owner->sync.resamp_type = AOUT_RESAMPLING_NONE;
        msg_Dbg (aout, "resampling stopped (drift: %"PRId64" us)", drift);
    }
}
