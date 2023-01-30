static void skip_spaces(char **arg)
{
    int len = 0;
    sscanf(*arg, " %n", &len);
    *arg += len;
}
