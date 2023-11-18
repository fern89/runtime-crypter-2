#include <windows.h>
#include <stdio.h>
#include "veh.h"
BEGIN_ENC
int isPrime(int number){
    for(int i=3; (i*i)<=number; i+=2){
        if(number % i == 0 ) return 0;
    }
    return 1;
}
long long hant(){
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    QueryPerformanceCounter(&start);
    int j=1;
    int pn=0;
    while(pn<10000){
        j+=2;
        if(isPrime(j)){
            pn++;
        }
    }
    QueryPerformanceCounter(&end);
    long long fy = end.QuadPart - start.QuadPart;
    printf("%lld\n", fy);
    return fy;
}
int main(){
    double a = hant(); //before crypt
    if(!hook()) return -1;
    double b = hant(); //after crypt
    printf("Perf drop: %fx\n", b/a); //~6000x on my machine
    getchar();
}
END_ENC
