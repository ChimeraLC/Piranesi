
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
            7654567, 5195977, 1241087,
            1866091, 3041789, 3069863,
            1237547, 7794467, 7956589,
            2495443, 4260917, 7004497,
            1178711, 1600061, 9980869,
        };
        int Prime_index = 0;
        int Height;
        int Width;

        /*
         * Constructor that creates the Perlin noise class.
         */
        Perlin(int height, int width) {
            Height = height;
            Width = width;
        }

        /* 
         * Generates pseudorandom noise
         */
        double Noise(int i, int x, int y) {
            int n = x + y * 59;
            n = (n << 13) ^ n;
            int a = Primes[i * 3], b = Primes[i * 3 + 1], c = Primes[i * 3 + 2];
            int t = (n * (n * n * a + b) + c) & 0x3fffffff;
            return 1.0 - (double)(t)/(1 << 30);
        }

        double Turbulence(int x, int y) {
            double total = 0;
            for (int i = 1; i < 4; i++) {
                total += Noise(i, x * (1 << i), y * (1 << i));
            }
            for (int i = 0; i < 5; i++) {
                total += Noise(i, x / (1 << i), y / (1 << i));
            }
            return total / 8;
        }

};