 */
static block_t *aout_FiltersPipelinePlay(filter_t *const *filters,
                                         unsigned count, block_t *block)
{
    /* TODO: use filter chain */
    for (unsigned i = 0; (i < count) && (block != NULL); i++)
    {
        filter_t *filter = filters[i];
        /* Please note that p_block->i_nb_samples & i_buffer
         * shall be set by the filter plug-in. */
        block = filter->pf_audio_filter (filter, block);
    }
    return block;
}
