static void yae_advance_to_next_frag(ATempoContext *atempo)
{
    const double fragment_step = atempo->tempo * (double)(atempo->window / 2);
    const AudioFragment *prev;
    AudioFragment       *frag;
    atempo->nfrag++;
    prev = yae_prev_frag(atempo);
    frag = yae_curr_frag(atempo);
    frag->position[0] = prev->position[0] + (int64_t)fragment_step;
    frag->position[1] = prev->position[1] + atempo->window / 2;
    frag->nsamples    = 0;
}
