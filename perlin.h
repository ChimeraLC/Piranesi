
#include <cstdio>
#include <cmath>
#include <cstdlib>

using namespace std;

/*
 * Perlin
 */
class Perlin
{
public:
    int Primes[15] = {
        7654567,
        5195977,
        1241087,
        1866091,
        3041789,
        3069863,
        1237547,
        7794467,
        7956589,
        2495443,
        4260917,
        7004497,
        1178711,
        1600061,
        9980869,
    };
    int Prime_index = 0;
    int Height;
    int Width;

    /*
     * Constructor that creates the Perlin noise class.
     */
    Perlin(int height, int width)
    {
        Height = height;
        Width = width;
    }

    /* Interpolates between two values */
    double Interpolate(double a, double b, double w)
    {
        /* Clamping */
        w = w > 1 ? 1 : w;
        w = w < 0 ? 0 : w;

        /* Calculate interpolation */
        // return (a-b) * w + b;
        return (b - a) * w + a;
    }

    /*
     * Generates pseudorandom noise value between 0 and 2pi
     */
    double Noise(int i, int x, int y)
    {
        int n = x + y * 59;
        n = (n << 13) ^ n;
        int a = Primes[i * 3], b = Primes[i * 3 + 1], c = Primes[i * 3 + 2];
        int t = (n * (n * n * a + b) + c) & 0x3fffffff;
        double rand = 2.0 - (double)(t) / (1 << 29);
        return rand * 3.14159265;
    }

    /*
     * Calculates the dot product with the gradient
     */
    double Gradient(int i, int ix, int iy, double x, double y)
    {
        /* Calculate noise */
        double noise = Noise(i, ix, iy);

        /* Calculate distances */
        double dx = x - (double)ix;
        double dy = y - (double)iy;

        /* Return dot product */
        return (dx * cos(noise) + dy * sin(noise)) * 0.5 + 0.5;
    }

    /*
     * Calculates perlin at a given location
     */
    double Calc_Perlin(int i, double x, double y)
    {

        /* Grid points */
        int x0 = (int)floor(x);
        int x1 = x0 + 1;
        int y0 = (int)floor(y);
        int y1 = y0 + 1;

        /* Interpolation values */
        double fx = x - (double)x0;
        double fy = y - (double)y0;

        double i1 = Interpolate(Gradient(i, x0, y0, x, y),
                                Gradient(i, x1, y0, x, y), fx);
        double i2 = Interpolate(Gradient(i, x0, y1, x, y),
                                Gradient(i, x1, y1, x, y), fx);

        return Interpolate(i1, i2, fy);
    }

    double Perlin_Val(double x, double y)
    {
        double total = 0;
        double ampl = 1;
        for (int i = 0; i <= 4; i++)
        {
            total += Calc_Perlin(i, x / (1 << (4 + i)), y / (1 << (4 + i))) / ampl;
            ampl *= 2;
        }
        return total;
    }

    double Perlin_Marble(double x, double y)
    {
        double total = Perlin_Val(x, y);
        return sin(2 * (total)) * 0.7 + 0.3;
    }
};