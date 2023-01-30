static float search(FOCContext *foc, int pass, int maxpass, int xmin, int xmax, int ymin, int ymax, int *best_x, int *best_y, float best_score)
{
    int x, y;
    if (pass + 1 <= maxpass) {
        int sub_x, sub_y;
        search(foc, pass+1, maxpass, xmin>>1, (xmax+1)>>1, ymin>>1, (ymax+1)>>1, &sub_x, &sub_y, 1.0);
        xmin = FFMAX(xmin, 2*sub_x - 4);
        xmax = FFMIN(xmax, 2*sub_x + 4);
        ymin = FFMAX(ymin, 2*sub_y - 4);
        ymax = FFMIN(ymax, 2*sub_y + 4);
    }
    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            float score = compare(foc->haystack_frame[pass], foc->needle_frame[pass], x, y);
            av_assert0(score != 0);
            if (score < best_score) {
                best_score = score;
                *best_x = x;
                *best_y = y;
            }
        }
    }
    return best_score;
}
