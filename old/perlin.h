
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
    double xPeriod = 4;
    double yPeriod = 8;
    double power = 5;
    double size = 16;
    int Height;
    int Width;
    double** NoiseGrid;

    /*
     * Constructor that creates the Perlin noise class.
     */
    Perlin(int height, int width)
    {
        Height = height;
        Width = width;

        /* Allocate array */
        NoiseGrid = (double **) malloc(height * sizeof(double*));
        for (int i = 0; i < height; i++) {
            NoiseGrid[i] = (double*) malloc(width * sizeof(double));
        }

        /* Create initial noise */
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                NoiseGrid[i][j] = (rand() % 32768) / 32768.0;
            }
        }
    }

    /**
     * Returns the smoothed noise value at a specific pixel 
     */
    double SmoothNoise(double x, double y)
    {
        /* Fractional parts of x and y */
        double fX = x - (int) x;
        double fY = y - (int) y;

        /* Integer parts */
        int iX1 = (int) x;
        int iY1 = (int) y;

        int iX2 = (iX1 - 1 + Width) % Width;
        int iY2 = (iY1 - 1 + Height) % Height;

        /* Calculate smoothed value */
        double total = 0;
        total += fX * fY * NoiseGrid[iY1][iX1];
        total += (1 - fX) * fY * NoiseGrid[iY1][iX2];
        total += fX * (1 - fY) * NoiseGrid[iY2][iX1];
        total += (1 - fX) * (1 - fY) * NoiseGrid[iY2][iX2];

        return total;
    }

    /* Calculates the turbulence noise based on a given size */
    double Turbulence(double x, double y, double size)
    {
        double total = 0;
        double sizeWalker = size;

        /* Calculate overlapping turbulence */
        while (sizeWalker >= 1)
        {
            total += SmoothNoise(x / sizeWalker, y / sizeWalker) * sizeWalker;
            sizeWalker /= 2;
        }

        return (total / size / 2);
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
     * Generates pseudorandom noise value between 0 and 1
     */
    double Noise(int i, int x, int y)
    {
        int n = x + y * 59;
        n = (n << 13) ^ n;
        int a = Primes[i * 3], b = Primes[i * 3 + 1], c = Primes[i * 3 + 2];
        int t = (n * (n * n * a + b) + c) & 0x3fffffff;
        double rand = 1.0 - (double)(t) / (1 << 30);
        return rand;
    }
    /*
     * Generates pseudorandom noise value between 0 and 2pi
     */
    double Cos_Noise(int i, int x, int y)
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
        double noise = Cos_Noise(i, ix, iy);

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
        double val = x * xPeriod / Width + y * yPeriod / Height +
            power * Turbulence(x, y, size);
        return abs(sin(val * 3.141592));
    }
};