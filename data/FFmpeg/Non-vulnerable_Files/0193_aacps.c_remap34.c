static void remap34(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 20 || num_par == 11) {
        for (e = 0; e < num_env; e++) {
            map_idx_20_to_34(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_34(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}
}
static void remap34(int8_t (**p_par_mapped)[PS_MAX_NR_IIDICC],
                    int8_t           (*par)[PS_MAX_NR_IIDICC],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[PS_MAX_NR_IIDICC] = *p_par_mapped;
    int e;
    if (num_par == 20 || num_par == 11) {
        for (e = 0; e < num_env; e++) {
            map_idx_20_to_34(par_mapped[e], par[e], full);
        }
