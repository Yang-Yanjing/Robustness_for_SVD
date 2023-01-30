static double get_rotated_w(void *opaque, double angle)
{
    RotContext *rot = opaque;
    double inw = rot->var_values[VAR_IN_W];
    double inh = rot->var_values[VAR_IN_H];
    float sinx = sin(angle);
    float cosx = cos(angle);
    return FFMAX(0, inh * sinx) + FFMAX(0, -inw * cosx) +
           FFMAX(0, inw * cosx) + FFMAX(0, -inh * sinx);
}
