static void count_ports(const LADSPA_Descriptor *desc,
                        unsigned long *nb_inputs, unsigned long *nb_outputs)
{
    LADSPA_PortDescriptor pd;
    int i;
    for (i = 0; i < desc->PortCount; i++) {
        pd = desc->PortDescriptors[i];
        if (LADSPA_IS_PORT_AUDIO(pd)) {
            if (LADSPA_IS_PORT_INPUT(pd)) {
                (*nb_inputs)++;
            } else if (LADSPA_IS_PORT_OUTPUT(pd)) {
                (*nb_outputs)++;
            }
        }
    }
}
