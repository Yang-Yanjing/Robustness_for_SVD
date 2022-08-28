void checkasm_update_bench(int iterations, uint64_t cycles)
{
    state.current_func_ver->iterations += iterations;
    state.current_func_ver->cycles += cycles;
}
