av_cold void AAC_RENAME(ff_ps_ctx_init)(PSContext *ps)
{
    AAC_RENAME(ff_psdsp_init)(&ps->dsp);
}
