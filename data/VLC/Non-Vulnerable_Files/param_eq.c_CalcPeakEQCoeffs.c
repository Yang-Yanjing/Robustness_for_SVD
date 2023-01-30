 */
static void CalcPeakEQCoeffs( float f0, float Q, float gainDB, float Fs,
                              float *coeffs )
{
    float A;
    float w0;
    float alpha;
    float b0, b1, b2;
    float a0, a1, a2;
    // Provide sane limits to avoid overflow
    if (Q < 0.1f) Q = 0.1f;
    if (Q > 100) Q = 100;
    if (f0 > Fs/2*0.95f) f0 = Fs/2*0.95f;
    if (gainDB < -40) gainDB = -40;
    if (gainDB > 40) gainDB = 40;
    A = pow(10, gainDB/40);
    w0 = 2*3.141593f*f0/Fs;
    alpha = sin(w0)/(2*Q);
    b0 = 1 + alpha*A;
    b1 = -2*cos(w0);
    b2 = 1 - alpha*A;
    a0 = 1 + alpha/A;
    a1 = -2*cos(w0);
    a2 = 1 - alpha/A;
    // Store values to coeffs and normalize by 1/a0
    coeffs[0] = b0/a0;
    coeffs[1] = b1/a0;
    coeffs[2] = b2/a0;
    coeffs[3] = a1/a0;
    coeffs[4] = a2/a0;
}
