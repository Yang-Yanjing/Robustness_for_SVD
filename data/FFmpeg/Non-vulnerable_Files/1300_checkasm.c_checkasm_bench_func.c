int checkasm_bench_func(void)
{
    return !state.num_failed && state.bench_pattern &&
           !strncmp(state.current_func->name, state.bench_pattern, state.bench_pattern_len);
}
