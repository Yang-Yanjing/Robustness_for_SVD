void av_display_rotation_set(int32_t matrix[9], double angle)
{
    double radians = -angle * M_PI / 180.0f;
    double c = cos(radians);
    double s = sin(radians);
    memset(matrix, 0, 9 * sizeof(int32_t));
    matrix[0] = CONV_DB(c);
    matrix[1] = CONV_DB(-s);
    matrix[3] = CONV_DB(s);
    matrix[4] = CONV_DB(c);
    matrix[8] = 1 << 30;
}
