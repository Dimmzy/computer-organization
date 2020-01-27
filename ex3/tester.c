#include <stdio.h>

long switch1(long *, long *, long);
extern long switch2(long *, long *, long);
int main() {
    long x0, y0, action;
    long x, y, result;
    long x1, y1, result1;
    for (action = -20; action < 40; action++) {
        printf("--running action %ld--\n", action);
        for (x0 = -100; x0 < 100; x0++) {
            for (y0 = -100; y0 < 100; y0++) {
                x = x0;
                x1 = x0;
                y = y0;
                y1 = y0;
                result = switch1(&x, &y, action);
                result1 = switch2(&x1, &y1, action);
                if (result != result1 || x != x1 || y != y1) {
                    printf("Switch does not match! action=%ld result=%ld result1=%ld x=%ld x1=%ld y=%ld y1=%ld\n",
                           action, result, result1, x, x1, y, y1);
                    return -1;
                }
            }
        }
    }
    return 0;
}
