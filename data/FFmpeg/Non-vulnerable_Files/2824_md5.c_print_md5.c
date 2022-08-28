static void print_md5(uint8_t *md5)
{
    int i;
    for (i = 0; i < 16; i++)
        printf("%02x", md5[i]);
    printf("\n");
}
