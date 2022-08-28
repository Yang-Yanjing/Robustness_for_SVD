static int request_frame(AVFilterLink *outlink)
{
    ShowSpectrumContext *s = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    unsigned i;
    int ret;
    s->req_fullfilled = 0;
    do {
        ret = ff_request_frame(inlink);
        if (ret == AVERROR_EOF && s->sliding == FULLFRAME && s->xpos > 0 &&
            s->outpicref) {
            for (i = 0; i < outlink->h; i++) {
                memset(s->outpicref->data[0] + i * s->outpicref->linesize[0] + s->xpos,   0, outlink->w - s->xpos);
                memset(s->outpicref->data[1] + i * s->outpicref->linesize[1] + s->xpos, 128, outlink->w - s->xpos);
                memset(s->outpicref->data[2] + i * s->outpicref->linesize[2] + s->xpos, 128, outlink->w - s->xpos);
            }
            ret = ff_filter_frame(outlink, s->outpicref);
            s->outpicref = NULL;
            s->req_fullfilled = 1;
        }
    } while (!s->req_fullfilled && ret >= 0);
    return ret;
}
