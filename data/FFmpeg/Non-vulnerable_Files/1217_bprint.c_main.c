int main(void)
{
    AVBPrint b;
    char buf[256];
    struct tm testtime = { .tm_year = 100, .tm_mon = 11, .tm_mday = 20 };
    av_bprint_init(&b, 0, -1);
    bprint_pascal(&b, 5);
    printf("Short text in unlimited buffer: %u/%u\n", (unsigned)strlen(b.str), b.len);
    printf("%s\n", b.str);
    av_bprint_finalize(&b, NULL);
    av_bprint_init(&b, 0, -1);
    bprint_pascal(&b, 25);
    printf("Long text in unlimited buffer: %u/%u\n", (unsigned)strlen(b.str), b.len);
    av_bprint_finalize(&b, NULL);
    av_bprint_init(&b, 0, 2048);
    bprint_pascal(&b, 25);
    printf("Long text in limited buffer: %u/%u\n", (unsigned)strlen(b.str), b.len);
    av_bprint_finalize(&b, NULL);
    av_bprint_init(&b, 0, 1);
    bprint_pascal(&b, 5);
    printf("Short text in automatic buffer: %u/%u\n", (unsigned)strlen(b.str), b.len);
    av_bprint_init(&b, 0, 1);
    bprint_pascal(&b, 25);
    printf("Long text in automatic buffer: %u/%u\n", (unsigned)strlen(b.str)/8*8, b.len);
    
    av_bprint_init(&b, 0, 0);
    bprint_pascal(&b, 25);
    printf("Long text count only buffer: %u/%u\n", (unsigned)strlen(b.str), b.len);
    av_bprint_init_for_buffer(&b, buf, sizeof(buf));
    bprint_pascal(&b, 25);
    printf("Long text count only buffer: %u/%u\n", (unsigned)strlen(buf), b.len);
    av_bprint_init(&b, 0, -1);
    av_bprint_strftime(&b, "%Y-%m-%d", &testtime);
    printf("strftime full: %u/%u \"%s\"\n", (unsigned)strlen(buf), b.len, b.str);
    av_bprint_finalize(&b, NULL);
    av_bprint_init(&b, 0, 8);
    av_bprint_strftime(&b, "%Y-%m-%d", &testtime);
    printf("strftime truncated: %u/%u \"%s\"\n", (unsigned)strlen(buf), b.len, b.str);
    return 0;
}
