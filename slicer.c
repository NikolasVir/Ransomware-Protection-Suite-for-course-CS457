double random_in_range(double min, double max)
{
    return min + (double)rand() / RAND_MAX * (max - min);
}

// Function to generate a random polynomial and print it and 10 points that satisfy it
void generate_random_polynomial_and_points(double c)
{
    srand(time(NULL));

    double a = random_in_range(0, 1000);
    double b = random_in_range(0, 1000);

    // Print the polynomial
    // printf("The polynomial is: f(x) = %fx^2 + %fx + %f\n", a, b, c);

    // Generate and print 10 points that satisfy the polynomial
    for (int i = 0; i < 10; i++)
    {
        double x = random_in_range(-10, 10);
        double y = a * x * x + b * x + c;
        printf("Point %d: (%f, %f)\n", i + 1, x, y);
    }
}