#include <stdio.h>
int main(void){
    unsigned long n, i;
    long double x;

    //printf("How many terms to calculate pi to? ");
    //scanf("%lu", &n);
    n = 100000000000;

    x = 1.0;

    for (i=1; i<=n; i=i+1){
        if (i%2 == 1)
            x = x - (1.0 / ((2.0*i)+1));
        else
            x = x + (1.0 / ((2.0*i)+1));
    }
    x = x * 4.0;
    printf("The value of pi is: %.20Lf\n", x);
    return 0;
}
