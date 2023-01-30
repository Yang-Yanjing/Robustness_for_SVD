static void free_script(struct sbg_script *s)
{
    av_freep(&s->def);
    av_freep(&s->synth);
    av_freep(&s->tseq);
    av_freep(&s->block_tseq);
    av_freep(&s->events);
    av_freep(&s->opt_mix);
}
