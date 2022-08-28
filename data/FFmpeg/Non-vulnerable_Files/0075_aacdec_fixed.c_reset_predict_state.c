static av_always_inline void reset_predict_state(PredictorState *ps)
{
    ps->r0.mant   = 0;
    ps->r0.exp   = 0;
    ps->r1.mant   = 0;
    ps->r1.exp   = 0;
    ps->cor0.mant = 0;
    ps->cor0.exp = 0;
    ps->cor1.mant = 0;
    ps->cor1.exp = 0;
    ps->var0.mant = 0x20000000;
    ps->var0.exp = 1;
    ps->var1.mant = 0x20000000;
    ps->var1.exp = 1;
}
