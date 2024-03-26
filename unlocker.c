void calculate_coefficients(double x1, double y1, double x2, double y2, double x3, double y3, double *a, double *b, double *c)
{
    *a = (x1 * (y3 - y2) + x2 * (y1 - y3) + x3 * (y2 - y1)) / ((x1 - x2) * (x1 - x3) * (x2 - x3));
    *b = (y2 - y1) / (x2 - x1) - *a * (x1 + x2);
    *c = y1 - *a * x1 * x1 - *b * x1;
}