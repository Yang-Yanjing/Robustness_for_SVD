static inline void vc1_b_mc(VC1Context *v, int dmv_x[2], int dmv_y[2],
                            int direct, int mode)
{
    if (direct) {
        ff_vc1_mc_1mv(v, 0);
        ff_vc1_interp_mc(v);
        return;
    }
    if (mode == BMV_TYPE_INTERPOLATED) {
        ff_vc1_mc_1mv(v, 0);
        ff_vc1_interp_mc(v);
        return;
    }
    ff_vc1_mc_1mv(v, (mode == BMV_TYPE_BACKWARD));
}
