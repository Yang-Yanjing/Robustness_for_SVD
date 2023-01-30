int main(void)
{
    ps_tableinit();
    write_fileheader();
    printf("static const %s pd_re_smooth[8*8*8] = {\n", TYPE_NAME);
    ARRAY_RENAME(array)(pd_re_smooth, 8*8*8);
    printf("};\n");
    printf("static const %s pd_im_smooth[8*8*8] = {\n", TYPE_NAME);
    ARRAY_RENAME(array)(pd_im_smooth, 8*8*8);
    printf("};\n");
    printf("static const %s HA[46][8][4] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(HA, 46, 8, 4);
    printf("};\n");
    printf("static const %s HB[46][8][4] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(HB, 46, 8, 4);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, f20_0_8)[8][8][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(f20_0_8, 8, 8, 2);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, f34_0_12)[12][8][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(f34_0_12, 12, 8, 2);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, f34_1_8)[8][8][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(f34_1_8, 8, 8, 2);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, f34_2_4)[4][8][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(f34_2_4, 4, 8, 2);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, Q_fract_allpass)[2][50][3][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(4d_array)(Q_fract_allpass, 2, 50, 3, 2);
    printf("};\n");
    printf("static const DECLARE_ALIGNED(16, %s, phi_fract)[2][50][2] = {\n", TYPE_NAME);
    ARRAY_RENAME(3d_array)(phi_fract, 2, 50, 2);
    printf("};\n");
    return 0;
}
