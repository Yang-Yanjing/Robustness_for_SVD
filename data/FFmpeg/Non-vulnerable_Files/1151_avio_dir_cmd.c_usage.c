static void usage(const char *program_name)
{
    fprintf(stderr, "usage: %s OPERATION entry1 [entry2]\n"
            "API example program to show how to manipulate resources "
            "accessed through AVIOContext.\n"
            "OPERATIONS:\n"
            "list      list content of the directory\n"
            "move      rename content in directory\n"
            "del       delete content in directory\n",
            program_name);
}
