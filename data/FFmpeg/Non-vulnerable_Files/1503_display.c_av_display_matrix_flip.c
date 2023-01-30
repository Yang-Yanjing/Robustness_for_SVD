void av_display_matrix_flip(int32_t matrix[9], int hflip, int vflip)
{
    int i;
    const int flip[] = { 1 - 2 * (!!hflip), 1 - 2 * (!!vflip), 1 };
    if (hflip || vflip)
        for (i = 0; i < 9; i++)
            matrix[i] *= flip[i % 3];
}
