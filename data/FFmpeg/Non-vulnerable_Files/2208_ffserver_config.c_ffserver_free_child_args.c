void ffserver_free_child_args(void *argsp)
{
    int i;
    char **args;
    if (!argsp)
        return;
    args = *(char ***)argsp;
    if (!args)
        return;
    for (i = 0; i < MAX_CHILD_ARGS; i++)
        av_free(args[i]);
    av_freep(argsp);
}
