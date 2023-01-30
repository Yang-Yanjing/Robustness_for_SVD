 *****************************************************************************/
int aout_DecPlay (audio_output_t *aout, block_t *block, int input_rate)
{
    aout_owner_t *owner = aout_owner (aout);
    assert (input_rate >= INPUT_RATE_DEFAULT / AOUT_MAX_INPUT_RATE);
    assert (input_rate <= INPUT_RATE_DEFAULT * AOUT_MAX_INPUT_RATE);
    assert (block->i_pts >= VLC_TS_0);
    block->i_length = CLOCK_FREQ * block->i_nb_samples
                                 / owner->input_format.i_rate;
    aout_OutputLock (aout);
    if (unlikely(aout_CheckReady (aout)))
        goto drop; /* Pipeline is unrecoverably broken :-( */
    const mtime_t now = mdate (), advance = block->i_pts - now;
    if (advance < -AOUT_MAX_PTS_DELAY)
    {   /* Late buffer can be caused by bugs in the decoder, by scheduling
         * latency spikes (excessive load, SIGSTOP, etc.) or if buffering is
         * insufficient. We assume the PTS is wrong and play the buffer anyway:
         * Hopefully video has encountered a similar PTS problem as audio. */
        msg_Warn (aout, "buffer too late (%"PRId64" us): dropped", advance);
        goto drop;
    }
    if (advance > AOUT_MAX_ADVANCE_TIME)
    {   /* Early buffers can only be caused by bugs in the decoder. */
        msg_Err (aout, "buffer too early (%"PRId64" us): dropped", advance);
        goto drop;
    }
    if (block->i_flags & BLOCK_FLAG_DISCONTINUITY)
        owner->sync.discontinuity = true;
    block = aout_FiltersPlay (owner->filters, block, input_rate);
    if (block == NULL)
        goto lost;
    /* Software volume */
    aout_volume_Amplify (owner->volume, block);
    /* Drift correction */
    aout_DecSynchronize (aout, block->i_pts, input_rate);
    /* Output */
    owner->sync.end = block->i_pts + block->i_length + 1;
    owner->sync.discontinuity = false;
    aout_OutputPlay (aout, block);
out:
    aout_OutputUnlock (aout);
    return 0;
drop:
    owner->sync.discontinuity = true;
    block_Release (block);
lost:
    atomic_fetch_add(&owner->buffers_lost, 1);
    goto out;
}
