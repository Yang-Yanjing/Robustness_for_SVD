static void set_pcr_pid(AVFormatContext *s, unsigned int programid, unsigned int pid)
{
    int i;
    for (i = 0; i < s->nb_programs; i++) {
        if (s->programs[i]->id == programid) {
            s->programs[i]->pcr_pid = pid;
            break;
        }
    }
}
