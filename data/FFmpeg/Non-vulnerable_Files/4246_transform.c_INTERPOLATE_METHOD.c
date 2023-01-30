INTERPOLATE_METHOD(interpolate_biquadratic)
{
    int     x_c, x_f, y_c, y_f;
    uint8_t v1,  v2,  v3,  v4;
    float   f1,  f2,  f3,  f4;
    if (x < - 1 || x > width || y < -1 || y > height)
        return def;
    else {
        x_f = (int)x;
        x_c = x_f + 1;
        y_f = (int)y;
        y_c = y_f + 1;
        v1 = PIXEL(src, x_c, y_c, width, height, stride, def);
        v2 = PIXEL(src, x_c, y_f, width, height, stride, def);
        v3 = PIXEL(src, x_f, y_c, width, height, stride, def);
        v4 = PIXEL(src, x_f, y_f, width, height, stride, def);
        f1 = 1 - sqrt((x_c - x) * (y_c - y));
        f2 = 1 - sqrt((x_c - x) * (y - y_f));
        f3 = 1 - sqrt((x - x_f) * (y_c - y));
        f4 = 1 - sqrt((x - x_f) * (y - y_f));
        return (v1 * f1 + v2 * f2 + v3 * f3 + v4 * f4) / (f1 + f2 + f3 + f4);
    }
}
