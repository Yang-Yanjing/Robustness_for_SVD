void checkasm_fail_func(const char *msg, ...)
{
    if (state.current_func_ver->cpu && state.current_func_ver->ok) {
        va_list arg;
        print_cpu_name();
        fprintf(stderr, "   %s_%s (", state.current_func->name, cpu_suffix(state.current_func_ver->cpu));
        va_start(arg, msg);
        vfprintf(stderr, msg, arg);
        va_end(arg);
        fprintf(stderr, ")\n");
        state.current_func_ver->ok = 0;
        state.num_failed++;
    }
}
