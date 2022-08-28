void exit_program(int ret)
{
    if (program_exit)
        program_exit(ret);
    exit(ret);
}
