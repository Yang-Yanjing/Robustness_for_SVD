int plan9_main(int argc, char **argv);
int main(int argc, char **argv)
{
    
    short fcr;
    __asm__ volatile ("fstcw        %0 \n"
                      "or      $63, %0 \n"
                      "fldcw        %0 \n"
                      : "=m"(fcr));
    return plan9_main(argc, argv);
}
