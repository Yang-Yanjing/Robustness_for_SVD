int main(int argc, char *argv[])
{
    FILE *f[2];
    int len = 1;
    int shift_first= argc < 5 ? 0 : atoi(argv[4]);
    int skip_bytes = argc < 6 ? 0 : atoi(argv[5]);
    int shift_last = shift_first + (argc < 7 ? 0 : atoi(argv[6]));
    int shift;
    int max_psnr   = -1;
    int max_psnr_shift = 0;
    if (shift_last > shift_first)
        shift_first -= shift_last - shift_first;
    if (argc > 3) {
        if (!strcmp(argv[3], "u8")) {
            len = 1;
        } else if (!strcmp(argv[3], "s16")) {
            len = 2;
        } else if (!strcmp(argv[3], "f32")) {
            len = 4;
        } else if (!strcmp(argv[3], "f64")) {
            len = 8;
        } else {
            char *end;
            len = strtol(argv[3], &end, 0);
            if (*end || len < 1 || len > 2) {
                fprintf(stderr, "Unsupported sample format: %s\nSupported: u8, s16, f32, f64\n", argv[3]);
                return 1;
            }
        }
    }
    if (argc < 3) {
        printf("tiny_psnr <file1> <file2> [<elem size>|u8|s16|f32|f64 [<shift> [<skip bytes> [<shift search range>]]]]\n");
        printf("WAV headers are skipped automatically.\n");
        return 1;
    }
    f[0] = fopen(argv[1], "rb");
    f[1] = fopen(argv[2], "rb");
    if (!f[0] || !f[1]) {
        fprintf(stderr, "Could not open input files.\n");
        return 1;
    }
    for (shift = shift_first; shift <= shift_last; shift++) {
        int psnr = run_psnr(f, len, shift, skip_bytes);
        if (psnr > max_psnr || (shift < 0 && psnr == max_psnr)) {
            max_psnr = psnr;
            max_psnr_shift = shift;
        }
    }
    if (max_psnr < 0)
        return 2;
    if (shift_last > shift_first)
        printf("Best PSNR is %3d.%02d for shift %i\n", (int)(max_psnr / F), (int)(max_psnr % F), max_psnr_shift);
    return 0;
}
