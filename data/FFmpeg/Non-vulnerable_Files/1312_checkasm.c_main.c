int main(int argc, char *argv[])
{
    int i, seed, ret = 0;
    if (!tests[0].func || !cpus[0].flag) {
        fprintf(stderr, "checkasm: no tests to perform\n");
        return 0;
    }
    if (argc > 1 && !strncmp(argv[1], "--bench", 7)) {
#ifndef AV_READ_TIME
        fprintf(stderr, "checkasm: --bench is not supported on your system\n");
        return 1;
#endif
        if (argv[1][7] == '=') {
            state.bench_pattern = argv[1] + 8;
            state.bench_pattern_len = strlen(state.bench_pattern);
        } else
            state.bench_pattern = "";
        argc--;
        argv++;
    }
    seed = (argc > 1) ? atoi(argv[1]) : av_get_random_seed();
    fprintf(stderr, "checkasm: using random seed %u\n", seed);
    av_lfg_init(&checkasm_lfg, seed);
    check_cpu_flag(NULL, 0);
    for (i = 0; cpus[i].flag; i++)
        check_cpu_flag(cpus[i].name, cpus[i].flag);
    if (state.num_failed) {
        fprintf(stderr, "checkasm: %d of %d tests have failed\n", state.num_failed, state.num_checked);
        ret = 1;
    } else {
        fprintf(stderr, "checkasm: all %d tests passed\n", state.num_checked);
#ifdef AV_READ_TIME
        if (state.bench_pattern) {
            state.nop_time = measure_nop_time();
            printf("nop: %d.%d\n", state.nop_time/10, state.nop_time%10);
            print_benchs(state.funcs);
        }
#endif
    }
    destroy_func_tree(state.funcs);
    return ret;
}
