static int init_demo(const char *filename)
{
    int i, j;
    int h;
    int radian;
    char line[3 * W];
    FILE *input_file;
    input_file = fopen(filename, "rb");
    if (!input_file) {
        perror(filename);
        return 1;
    }
    if (fread(line, 1, 15, input_file) != 15)
        return 1;
    for (i = 0; i < H; i++) {
        if (fread(line, 1, 3 * W, input_file) != 3 * W)
            return 1;
        for (j = 0; j < W; j++) {
            tab_r[W * i + j] = line[3 * j    ];
            tab_g[W * i + j] = line[3 * j + 1];
            tab_b[W * i + j] = line[3 * j + 2];
        }
    }
    fclose(input_file);
    
    for (i = 0; i < 360; i++) {
        radian   = 2 * i * MY_PI / 360;
        h        = 2 * FIXP + int_sin(radian);
        h_cos[i] = h * int_sin(radian + MY_PI / 2) / 2 / FIXP;
        h_sin[i] = h * int_sin(radian)             / 2 / FIXP;
    }
    return 0;
}
