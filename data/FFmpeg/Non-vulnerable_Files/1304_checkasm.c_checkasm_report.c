void checkasm_report(const char *name, ...)
{
    static int prev_checked, prev_failed, max_length;
    if (state.num_checked > prev_checked) {
        int pad_length = max_length + 4;
        va_list arg;
        print_cpu_name();
        pad_length -= fprintf(stderr, " - %s.", state.current_test_name);
        va_start(arg, name);
        pad_length -= vfprintf(stderr, name, arg);
        va_end(arg);
        fprintf(stderr, "%*c", FFMAX(pad_length, 0) + 2, '[');
        if (state.num_failed == prev_failed)
            color_printf(COLOR_GREEN, "OK");
        else
            color_printf(COLOR_RED, "FAILED");
        fprintf(stderr, "]\n");
        prev_checked = state.num_checked;
        prev_failed  = state.num_failed;
    } else if (!state.cpu_flag) {
        
        int length = strlen(state.current_test_name);
        va_list arg;
        va_start(arg, name);
        length += vsnprintf(NULL, 0, name, arg);
        va_end(arg);
        if (length > max_length)
            max_length = length;
    }
}
