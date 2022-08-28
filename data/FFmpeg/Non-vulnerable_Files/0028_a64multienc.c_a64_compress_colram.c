static void a64_compress_colram(unsigned char *buf, int *charmap, uint8_t *colram)
{
    int a;
    uint8_t temp;
    
    
    for (a = 0; a < 256; a++) {
        temp  = colram[charmap[a + 0x000]] << 0;
        temp |= colram[charmap[a + 0x100]] << 1;
        temp |= colram[charmap[a + 0x200]] << 2;
        if (a < 0xe8) temp |= colram[charmap[a + 0x300]] << 3;
        buf[a] = temp << 2;
    }
}
