void print_help(const char* name)
{
    printf("usage: %s file|dir [w=%i] [h=%i]\n"
            "generate a test video stream\n",
            name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    exit(1);
}
