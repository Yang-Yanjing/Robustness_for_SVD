void avfilter_get_matrix(float x_shift, float y_shift, float angle, float zoom, float *matrix) {
    matrix[0] = zoom * cos(angle);
    matrix[1] = -sin(angle);
    matrix[2] = x_shift;
    matrix[3] = -matrix[1];
    matrix[4] = matrix[0];
    matrix[5] = y_shift;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 1;
}
