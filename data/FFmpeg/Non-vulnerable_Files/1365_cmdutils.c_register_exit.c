static void (*program_exit)(int ret);
void register_exit(void (*cb)(int ret))
{
    program_exit = cb;
}
