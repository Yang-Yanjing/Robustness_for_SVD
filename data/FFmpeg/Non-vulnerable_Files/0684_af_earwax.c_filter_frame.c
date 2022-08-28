static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterLink *outlink = inlink->dst->outputs[0];
    int16_t *taps, *endin, *in, *out;
    AVFrame *outsamples = ff_get_audio_buffer(inlink, insamples->nb_samples);
    int len;
    if (!outsamples) {
        av_frame_free(&insamples);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outsamples, insamples);
    taps  = ((EarwaxContext *)inlink->dst->priv)->taps;
    out   = (int16_t *)outsamples->data[0];
    in    = (int16_t *)insamples ->data[0];
    len = FFMIN(NUMTAPS, 2*insamples->nb_samples);
    
    memcpy(taps+NUMTAPS, in, len * sizeof(*taps));
    out   = scalarproduct(taps, taps + len, out);
    
    if (2*insamples->nb_samples >= NUMTAPS ){
        endin = in + insamples->nb_samples * 2 - NUMTAPS;
        scalarproduct(in, endin, out);
        
        memcpy(taps, endin, NUMTAPS * sizeof(*taps));
    } else
        memmove(taps, taps + 2*insamples->nb_samples, NUMTAPS * sizeof(*taps));
    av_frame_free(&insamples);
    return ff_filter_frame(outlink, outsamples);
}
