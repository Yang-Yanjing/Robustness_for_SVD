static int check(char *file)
{
    uint8_t buffer[SIZE];
    int fd, flags = O_RDONLY;
    int ret = 0;
#ifdef O_BINARY
    flags |= O_BINARY;
#endif
    if (file) fd = open(file, flags);
    else      fd = 0;
    if (fd == -1) {
        printf("%s=OPEN-FAILED: %s:", av_hash_get_name(hash), strerror(errno));
        ret = 1;
        goto end;
    }
    av_hash_init(hash);
    for (;;) {
        int size = read(fd, buffer, SIZE);
        if (size < 0) {
            int err = errno;
            close(fd);
            finish();
            printf("+READ-FAILED: %s", strerror(err));
            ret = 2;
            goto end;
        } else if(!size)
            break;
        av_hash_update(hash, buffer, size);
    }
    close(fd);
    finish();
end:
    if (file)
        printf(" *%s", file);
    printf("\n");
    return ret;
}
