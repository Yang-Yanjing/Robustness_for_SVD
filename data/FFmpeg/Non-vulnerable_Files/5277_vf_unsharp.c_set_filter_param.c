static void set_filter_param(UnsharpFilterParam *fp, int msize_x, int msize_y, float amount)
{
    fp->msize_x = msize_x;
    fp->msize_y = msize_y;
    fp->amount = amount * 65536.0;
    fp->steps_x = msize_x / 2;
    fp->steps_y = msize_y / 2;
    fp->scalebits = (fp->steps_x + fp->steps_y) * 2;
    fp->halfscale = 1 << (fp->scalebits - 1);
}
