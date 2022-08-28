static int get_height(const FieldMatchContext *fm, const AVFrame *f, int plane)
{
    return plane ? FF_CEIL_RSHIFT(f->height, fm->vsub) : f->height;
}
