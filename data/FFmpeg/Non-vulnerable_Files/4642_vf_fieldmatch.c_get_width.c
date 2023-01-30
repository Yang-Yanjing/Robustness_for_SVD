AVFILTER_DEFINE_CLASS(fieldmatch);
static int get_width(const FieldMatchContext *fm, const AVFrame *f, int plane)
{
    return plane ? FF_CEIL_RSHIFT(f->width, fm->hsub) : f->width;
}
