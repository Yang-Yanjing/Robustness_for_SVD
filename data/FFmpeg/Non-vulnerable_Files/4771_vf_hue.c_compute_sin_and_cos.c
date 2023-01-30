AVFILTER_DEFINE_CLASS(hue);
static inline void compute_sin_and_cos(HueContext *hue)
{
    




    hue->hue_sin = rint(sin(hue->hue) * (1 << 16) * hue->saturation);
    hue->hue_cos = rint(cos(hue->hue) * (1 << 16) * hue->saturation);
}
