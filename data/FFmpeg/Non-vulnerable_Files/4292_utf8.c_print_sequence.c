static void print_sequence(const char *p, int l, int indent)
{
    int i;
    for (i = 0; i < l; i++)
        printf("%02X", (uint8_t)p[i]);
    printf("%*s", indent-l*2, "");
}
