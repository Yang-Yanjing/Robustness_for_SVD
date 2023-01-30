}
static void aout_DecSilence (audio_output_t *aout, mtime_t length, mtime_t pts)
{
    aout_owner_t *owner = aout_owner (aout);
    const audio_sample_format_t *fmt = &owner->mixer_format;
    size_t frames = (fmt->i_rate * length) / CLOCK_FREQ;
    block_t *block;
    if (AOUT_FMT_SPDIF(fmt))
        block = block_Alloc (4 * frames);
    else
        block = block_Alloc (frames * fmt->i_bytes_per_frame);
    if (unlikely(block == NULL))
        return; /* uho! */
    msg_Dbg (aout, "inserting %zu zeroes", frames);
    memset (block->p_buffer, 0, block->i_buffer);
    block->i_nb_samples = frames;
    block->i_pts = pts;
    block->i_dts = pts;
    block->i_length = length;
    aout_OutputPlay (aout, block);
}
