 */
static void CalcShelfEQCoeffs( float f0, float slope, float gainDB, int high,
                               float Fs, float *coeffs )
{
    float A;
    float w0;
    float alpha;
    float b0, b1, b2;
    float a0, a1, a2;
    // Provide sane limits to avoid overflow
    if (f0 > Fs/2*0.95f) f0 = Fs/2*0.95f;
    if (gainDB < -40) gainDB = -40;
    if (gainDB > 40) gainDB = 40;
    A = pow(10, gainDB/40);
    w0 = 2*3.141593f*f0/Fs;
    alpha = sin(w0)/2 * sqrt( (A + 1/A)*(1/slope - 1) + 2 );
    if (high)
    {
        b0 =    A*( (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha );
        b1 = -2*A*( (A-1) + (A+1)*cos(w0) );
        b2 =    A*( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha );
        a0 =        (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha;
        a1 =    2*( (A-1) - (A+1)*cos(w0) );
        a2 =        (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha;
    }
    else
    {
        b0 =    A*( (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha );
        b1 =  2*A*( (A-1) - (A+1)*cos(w0));
        b2 =    A*( (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha );
        a0 =        (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha;
        a1 =   -2*( (A-1) + (A+1)*cos(w0));
        a2 =        (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha;
    }
    // Store values to coeffs and normalize by 1/a0
    coeffs[0] = b0/a0;
    coeffs[1] = b1/a0;
    coeffs[2] = b2/a0;
    coeffs[3] = a1/a0;
    coeffs[4] = a2/a0;
}
