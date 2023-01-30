static int is_planar_yuv(const AVPixFmtDescriptor *desc)
{
    int i;
    if (desc->flags & ~(AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA) ||
        desc->nb_components < 3 ||
        (desc->comp[1].depth_minus1 != desc->comp[2].depth_minus1))
        return 0;
    for (i = 0; i < desc->nb_components; i++) {
        if (desc->comp[i].offset_plus1 != 1 ||
            desc->comp[i].shift != 0 ||
            desc->comp[i].plane != i)
            return 0;
    }
    return 1;
}
