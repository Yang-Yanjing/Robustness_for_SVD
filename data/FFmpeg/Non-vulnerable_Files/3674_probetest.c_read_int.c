static int read_int(char *arg) {
    int ret;
    if (!arg || !*arg)
        return -1;
    ret = strtol(arg, &arg, 0);
    if (*arg)
        return -1;
    return ret;
}
