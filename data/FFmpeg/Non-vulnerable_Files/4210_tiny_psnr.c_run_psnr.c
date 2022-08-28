static int run_psnr(FILE *f[2], int len, int shift, int skip_bytes)
{
    int i, j;
    uint64_t sse = 0;
    double sse_d = 0.0;
    uint8_t buf[2][SIZE];
    int64_t max    = (1LL << (8 * len)) - 1;
    int size0      = 0;
    int size1      = 0;
    uint64_t maxdist = 0;
    double maxdist_d = 0.0;
    int noseek;
    noseek = fseek(f[0], 0, SEEK_SET) ||
             fseek(f[1], 0, SEEK_SET);
    if (!noseek) {
        for (i = 0; i < 2; i++) {
            uint8_t *p = buf[i];
            if (fread(p, 1, 12, f[i]) != 12)
                return -1;
            if (!memcmp(p, "RIFF", 4) &&
                !memcmp(p + 8, "WAVE", 4)) {
                if (fread(p, 1, 8, f[i]) != 8)
                    return -1;
                while (memcmp(p, "data", 4)) {
                    int s = p[4] | p[5] << 8 | p[6] << 16 | p[7] << 24;
                    fseek(f[i], s, SEEK_CUR);
                    if (fread(p, 1, 8, f[i]) != 8)
                        return -1;
                }
            } else {
                fseek(f[i], -12, SEEK_CUR);
            }
        }
        fseek(f[shift < 0], abs(shift), SEEK_CUR);
        fseek(f[0], skip_bytes, SEEK_CUR);
        fseek(f[1], skip_bytes, SEEK_CUR);
    }
    for (;;) {
        int s0 = fread(buf[0], 1, SIZE, f[0]);
        int s1 = fread(buf[1], 1, SIZE, f[1]);
        for (j = 0; j < FFMIN(s0, s1); j += len) {
            switch (len) {
            case 1:
            case 2: {
                int64_t a, b;
                int dist;
                if (len == 2) {
                    a = get_s16l(buf[0] + j);
                    b = get_s16l(buf[1] + j);
                } else {
                    a = buf[0][j];
                    b = buf[1][j];
                }
                sse += (a - b) * (a - b);
                dist = abs(a - b);
                if (dist > maxdist)
                    maxdist = dist;
                break;
            }
            case 4:
            case 8: {
                double dist, a, b;
                if (len == 8) {
                    a = get_f64l(buf[0] + j);
                    b = get_f64l(buf[1] + j);
                } else {
                    a = get_f32l(buf[0] + j);
                    b = get_f32l(buf[1] + j);
                }
                dist = fabs(a - b);
                sse_d += (a - b) * (a - b);
                if (dist > maxdist_d)
                    maxdist_d = dist;
                break;
            }
            }
        }
        size0 += s0;
        size1 += s1;
        if (s0 + s1 <= 0)
            break;
    }
    i = FFMIN(size0, size1) / len;
    if (!i)
        i = 1;
    switch (len) {
    case 1:
    case 2: {
        uint64_t psnr;
        uint64_t dev = int_sqrt(((sse / i) * F * F) + (((sse % i) * F * F) + i / 2) / i);
        if (sse)
            psnr = ((2 * log16(max << 16) + log16(i) - log16(sse)) *
                    284619LL * F + (1LL << 31)) / (1LL << 32);
        else
            psnr = 1000 * F - 1; 
        printf("stddev:%5d.%02d PSNR:%3d.%02d MAXDIFF:%5"PRIu64" bytes:%9d/%9d\n",
               (int)(dev / F), (int)(dev % F),
               (int)(psnr / F), (int)(psnr % F),
               maxdist, size0, size1);
        return psnr;
        }
    case 4:
    case 8: {
        char psnr_str[64];
        double psnr = INT_MAX;
        double dev = sqrt(sse_d / i);
        uint64_t scale = (len == 4) ? (1ULL << 24) : (1ULL << 32);
        if (sse_d) {
            psnr = 2 * log(DBL_MAX) - log(i / sse_d);
            snprintf(psnr_str, sizeof(psnr_str), "%5.02f", psnr);
        } else
            snprintf(psnr_str, sizeof(psnr_str), "inf");
        maxdist = maxdist_d * scale;
        printf("stddev:%10.2f PSNR:%s MAXDIFF:%10"PRIu64" bytes:%9d/%9d\n",
               dev * scale, psnr_str, maxdist, size0, size1);
        return psnr;
    }
    }
    return -1;
}
