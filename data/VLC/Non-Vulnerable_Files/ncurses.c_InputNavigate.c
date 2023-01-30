}
static void InputNavigate(input_thread_t* p_input, const char *var)
{
    if (p_input)
        var_TriggerCallback(p_input, var);
}
