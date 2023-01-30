static av_cold void psy_3gpp_end(FFPsyContext *apc)
{
    AacPsyContext *pctx = (AacPsyContext*) apc->model_priv_data;
    av_freep(&pctx->ch);
    av_freep(&apc->model_priv_data);
}
