static double set_colorquant_ratio_meta(AVFrame *out, int nb_out, int nb_in)
{
    char buf[32];
    const double ratio = (double)nb_out / nb_in;
    snprintf(buf, sizeof(buf), "%f", ratio);
    av_dict_set(&out->metadata, "lavfi.color_quant_ratio", buf, 0);
    return ratio;
}
