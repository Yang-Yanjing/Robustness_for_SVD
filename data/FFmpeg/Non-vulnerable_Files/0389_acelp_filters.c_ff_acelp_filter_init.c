void ff_acelp_filter_init(ACELPFContext *c)
{
    c->acelp_interpolatef                      = ff_acelp_interpolatef;
    c->acelp_apply_order_2_transfer_function   = ff_acelp_apply_order_2_transfer_function;
    if(HAVE_MIPSFPU)
        ff_acelp_filter_init_mips(c);
}
