double av_display_rotation_get(const int32_t matrix[9])
{
    double rotation, scale[2];
    scale[0] = hypot(CONV_FP(matrix[0]), CONV_FP(matrix[3]));
    scale[1] = hypot(CONV_FP(matrix[1]), CONV_FP(matrix[4]));
    if (scale[0] == 0.0 || scale[1] == 0.0)
        return NAN;
    rotation = atan2(CONV_FP(matrix[1]) / scale[1],
                     CONV_FP(matrix[0]) / scale[0]) * 180 / M_PI;
    return -rotation;
}
