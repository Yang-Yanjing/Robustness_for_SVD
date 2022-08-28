static float get_f32l(uint8_t *p)
{
    union av_intfloat32 v;
    v.i = p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
    return v.f;
}
