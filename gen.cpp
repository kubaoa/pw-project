#include <iostream>
#include <chrono>
#include <random>

using namespace std;

int main()
{
    mt19937 gen(chrono::high_resolution_clock().now()
        .time_since_epoch().count());
    int t = gen() % 1 + 1;
    cout<<t<<"\n";
    while(t--) {
        int n = 50000;
        int B = 200000;
        cout<<n<<" "<<B<<"\n";
        while(n--) {
            cout<<gen()%30+1<<" "<<gen()%30+1<<"\n";
        }
    }
}
