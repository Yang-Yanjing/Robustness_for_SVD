static double g_func(void *p, double x)
{
    x = av_clipd(x, 0.0, 1.0);
    return (int)(x*255.0+0.5) << 8;
}
