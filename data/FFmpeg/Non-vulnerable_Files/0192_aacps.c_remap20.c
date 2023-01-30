static void remap20(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 34 || num_par == 17) {
        for (e = 0; e < num_env; e++) {
            map_idx_34_to_20(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_20(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}
}
static void remap20(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 34 || num_par == 17) {
        for (e = 0; e < num_env; e++) {
            map_idx_34_to_20(par_mapped[e], par[e], full);
        }
