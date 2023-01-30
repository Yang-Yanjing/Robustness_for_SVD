}
void vout_SetInterlacingState(vout_thread_t *vout, vout_interlacing_support_t *state, bool is_interlaced)
{
     /* Wait 30s before quiting interlacing mode */
    const int interlacing_change = (!!is_interlaced) - (!!state->is_interlaced);
    if ((interlacing_change == 1) ||
        (interlacing_change == -1 && state->date + 30000000 < mdate())) {
        msg_Dbg(vout, "Detected %s video",
                 is_interlaced ? "interlaced" : "progressive");
        var_SetBool(vout, "deinterlace-needed", is_interlaced);
        state->is_interlaced = is_interlaced;
    }
    if (is_interlaced)
        state->date = mdate();
}
