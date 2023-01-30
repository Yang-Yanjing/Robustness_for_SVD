void ff_acelp_filter_init_mips(ACELPFContext *c)
{
#if HAVE_INLINE_ASM
    c->acelp_interpolatef                      = ff_acelp_interpolatef_mips;
    c->acelp_apply_order_2_transfer_function   = ff_acelp_apply_order_2_transfer_function_mips;
#endif
}
