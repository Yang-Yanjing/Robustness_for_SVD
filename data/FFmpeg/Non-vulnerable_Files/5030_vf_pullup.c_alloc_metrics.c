static int alloc_metrics(PullupContext *s, PullupField *f)
{
    f->diffs = av_calloc(FFALIGN(s->metric_length, 16), sizeof(*f->diffs));
    f->combs = av_calloc(FFALIGN(s->metric_length, 16), sizeof(*f->combs));
    f->vars  = av_calloc(FFALIGN(s->metric_length, 16), sizeof(*f->vars));
    if (!f->diffs || !f->combs || !f->vars) {
        av_freep(&f->diffs);
        av_freep(&f->combs);
        av_freep(&f->vars);
        return AVERROR(ENOMEM);
    }
    return 0;
}
