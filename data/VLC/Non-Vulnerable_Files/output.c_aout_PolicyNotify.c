}
static void aout_PolicyNotify (audio_output_t *aout, bool cork)
{
    (cork ? var_IncInteger : var_DecInteger) (aout->p_parent, "corks");
}
