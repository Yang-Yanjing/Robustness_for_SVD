AVFILTER_DEFINE_CLASS(setfield);
static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    SetFieldContext *setfield = inlink->dst->priv;
    if (setfield->mode == MODE_PROG) {
        picref->interlaced_frame = 0;
    } else if (setfield->mode != MODE_AUTO) {
        picref->interlaced_frame = 1;
        picref->top_field_first = setfield->mode;
    }
    return ff_filter_frame(inlink->dst->outputs[0], picref);
}
