int main(int argc, char **argv)
{
    int i;
    int ret = 0;
    const char *hash_name;
    if (argc == 1) {
        usage();
        return 0;
    }
    hash_name = argv[1];
    out_b64 = av_strstart(hash_name, "b64:", &hash_name);
    if ((ret = av_hash_alloc(&hash, hash_name)) < 0) {
        switch(ret) {
        case AVERROR(EINVAL):
            printf("Invalid hash type: %s\n", hash_name);
            break;
        case AVERROR(ENOMEM):
            printf("%s\n", strerror(errno));
            break;
        }
        return 1;
    }
    for (i = 2; i < argc; i++)
        ret |= check(argv[i]);
    if (argc < 3)
        ret |= check(NULL);
    av_hash_freep(&hash);
    return ret;
}
