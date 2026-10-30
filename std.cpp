#include <iostream>
#include <vector>
#include <queue>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;
using ll = long long;

constexpr int T = 8;
array<mutex, T> mutexes{};
array<condition_variable, T> cvs{};
array<function<void()>, T> f_ptrs{};
array<int, T> new_master{};
array<int, T> dead{};
int counter = 0;

vector<ll> small(vector<pair<int, int>>& vec, int direction,
           int ln, int rn, int m);

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, vector<char>& result);

int main()
{
    cin.tie(0)->sync_with_stdio(0);
    int t;
    cin>>t;
    vector<thread> thread_pool;
    for(int tid=0; tid<T; tid++) {
        thread_pool.emplace_back([tid]{
            cout<<"thread "<<tid<<" helloing..."<<endl;
            int master_tid = 0;
            unique_lock lk(mutexes[tid]);
            while(1) {
                cvs[tid].wait(lk, [tid]{return f_ptrs[tid] != 0 || dead[tid];});
                cout<<"thread "<<tid<<" working..."<<endl;
                if(dead[tid])
                    break;
                f_ptrs[tid]();
                f_ptrs[tid] = nullptr;
            }
            cout<<"thread "<<tid<<" dying..."<<endl;
        });
    }
    while(t--) {
        int n, m;
        cin>>n>>m;
        if(n==0)
        {
            cout<<"0 0\n\n";
            continue;
        }
        vector<pair<int, int>> vec(n);
        for(auto& [x, y] : vec)
            cin>>x>>y;
        vector<char> res(n + 1, 0); 
        cout<<hirschberg(vec, 0, n-1, m, res)<<" ";
        int ans = 0;
        for(auto x: res){
            ans += (int)x;
        }
        cout<<ans<<"\n";
        for(int i=0; i<=n; i++)
            if(res[i])
                cout<<i<<" ";
        cout<<"\n";
    }
    for(int i=0; i<T; i++) {
        unique_lock lk(mutexes[i]);
        dead[i] = 1;
        cvs[i].notify_one();
        cout<<"Notified "<<i<<endl;
    }
    for(auto& t : thread_pool)
        t.join();
}

vector<ll> small(vector<pair<int, int>>& vec, int direction,
           int ln, int rn, int m)
{
//    cout<<"Small "<<ln<<" "<<rn<<" "<<m<<" "<<direction<<":\n";
    array<vector<ll>, 2> dp{vector<ll>(m+1), vector<ll>(m+1)};
    for(int i=ln;; i+=direction) {
        int mod_two = i % 2;
        for(int j=0; j <= m; j++) {
            if(j >= vec[i].first) {
                dp[mod_two][j] = max(dp[1-mod_two][j], dp[1-mod_two][j-vec[i].first] + vec[i].second);
            } else {
                dp[mod_two][j] = dp[1-mod_two][j];
            }
        }
        if(i==rn)
            break;
    }
    return dp[rn%2];
}

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, vector<char>& result)
{
//    cout<<"Hirsc "<<ln<<" "<<rn<<" "<<m<<":\n";
    if(ln>rn)
    {
        throw;
    }
    if(ln==rn)
    {
        if(vec[ln].first <= m) {
            result[ln+1] = 1;
            return vec[ln].second;
        }
        return 0;
    }
    int z = (rn+ln)/2;
    vector<ll> dp1, dp2;
    {
        unique_lock l1(mutexes[0]);
        {
            unique_lock l2(mutexes[1]);
            counter = 2;
            f_ptrs[0] = [&]{
                dp1 = small(vec, 1, ln, z, m);
                --counter;
            };
            f_ptrs[1] = [&]{
                dp2 = small(vec, -1, rn, z+1, m);
                unique_lock l3(mutexes[0]);
                cvs[0].wait(l3, []{return counter == 1;});
                --counter;
                cvs[0].notify_one();
            };
        }
        cvs[0].notify_one();
        cvs[1].notify_one();
        cvs[0].wait(l1, []{return counter == 0;});
    }
    cout<<ln<<" "<<rn<<" "<<m<<endl;
    int max_x = 0;
    ll max_value = dp1[0] + dp2[m];
    for(int i=1; i<=m; i++)
        if(ll new_value = dp1[i] + dp2[m-i]; new_value > max_value)
        {
            max_x=i; max_value = new_value;
        }
    hirschberg(vec, ln, z, max_x, result);
    hirschberg(vec, z+1, rn, m-max_x, result);
    return max_value;
}
