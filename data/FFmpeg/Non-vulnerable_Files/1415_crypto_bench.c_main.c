int main(int argc, char **argv)
{
    uint8_t *input = av_malloc(MAX_INPUT_SIZE * 2);
    uint8_t *output = input + MAX_INPUT_SIZE;
    unsigned i, impl, size;
    int opt;
    while ((opt = getopt(argc, argv, "hl:a:r:")) != -1) {
        switch (opt) {
        case 'l':
            enabled_libs = optarg;
            break;
        case 'a':
            enabled_algos = optarg;
            break;
        case 'r':
            specified_runs = strtol(optarg, NULL, 0);
            break;
        case 'h':
        default:
            fprintf(stderr, "Usage: %s [-l libs] [-a algos] [-r runs]\n",
                    argv[0]);
            if ((USE_EXT_LIBS)) {
                char buf[1024];
                snprintf(buf, sizeof(buf), "%s%s%s",
                         ((USE_EXT_LIBS) & USE_crypto)   ? "+crypto"   : "",
                         ((USE_EXT_LIBS) & USE_gcrypt)   ? "+gcrypt"   : "",
                         ((USE_EXT_LIBS) & USE_tomcrypt) ? "+tomcrypt" : "");
                fprintf(stderr, "Built with the following external libraries:\n"
                        "make VERSUS=%s\n", buf + 1);
            } else {
                fprintf(stderr, "Built without external libraries; use\n"
                        "make VERSUS=crypto+gcrypt+tomcrypt tools/crypto_bench\n"
                        "to enable them.\n");
            }
            exit(opt != 'h');
        }
    }
    if (!input)
        fatal_error("out of memory");
    for (i = 0; i < MAX_INPUT_SIZE; i += 4)
        AV_WB32(input + i, i);
    size = MAX_INPUT_SIZE;
    for (impl = 0; impl < FF_ARRAY_ELEMS(implementations); impl++)
        run_implementation(input, output, &implementations[impl], size);
    av_free(input);
    return 0;
}
