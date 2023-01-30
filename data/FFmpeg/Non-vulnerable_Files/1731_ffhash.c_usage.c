static int out_b64;
static void usage(void)
{
    int i = 0;
    const char *name;
    printf("usage: ffhash [b64:]algorithm [input]...\n");
    printf("Supported hash algorithms:");
    do {
        name = av_hash_names(i);
        if (name)
            printf(" %s", name);
        i++;
    } while(name);
    printf("\n");
}
