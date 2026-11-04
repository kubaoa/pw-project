#include <iostream>
#include <vector>
#include <queue>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

using namespace std;
using ll = long long;

constexpr int T = 8;
array<mutex, T> f_mut{};
array<condition_variable, T> f_cv{};
array<function<void()>, T> f_ptrs{};
array<int, T> new_master{};
array<int, T> dead{};
array<array<atomic<int>, T>, T> sub_alive{};

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, vector<char>& result, int tl, int tr);

int main()
{
    cin.tie(0)->sync_with_stdio(0);
    int t;
    cin>>t;
    vector<thread> thread_pool;
    for(int tid=0; tid<T; tid++) {
        thread_pool.emplace_back([tid]{
//            cout<<"thread "<<tid<<" helloing..."<<endl;
            unique_lock lk(f_mut[tid]);
            while(1) {
                f_cv[tid].notify_all();
                f_cv[tid].wait(lk, [tid]{return f_ptrs[tid] != 0 || dead[tid];});
//                cout<<"thread "<<tid<<" working..."<<endl;
                if(dead[tid])
                    break;
                f_ptrs[tid]();
                f_ptrs[tid] = nullptr;
            }
//            cout<<"thread "<<tid<<" dying..."<<endl;
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
        cout<<hirschberg(vec, 0, n-1, m, res, 0, T-1)<<" ";
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
        unique_lock lk(f_mut[i]);
        dead[i] = 1;
        f_cv[i].notify_one();
//        cout<<"Notified "<<i<<endl;
    }
    for(auto& t : thread_pool)
        t.join();
}

vector<ll> small(vector<pair<int, int>>& vec, int direction,
           int ln, int rn, int m)
{
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

vector<ll> small_multithread(vector<pair<int, int>>& vec, int direction,
           int ln, int rn, int m, int tl, int tr)
{
    if((tl == tr) || (m < 500))
        return small(vec, direction, ln, rn, m);
    array<vector<ll>, 2> dp{vector<ll>(m+1), vector<ll>(m+1)};
    mutex g_mut;
    condition_variable g_cv;
    int thread_count = tr-tl+1;
    int numdone = 0;
    sub_alive[tl][tr] = thread_count;
//    cout<<"Have "<<thread_count<<" threads, working on "<<ln<<".."<<rn<<endl;
    vector<function<void()>> f_ptrs_vec(thread_count);
    for(int tid2=0; tid2<thread_count; tid2++) {
        f_ptrs_vec[tid2] = [&]{
            int k = 1, tid = -1;
            {
                unique_lock lock(g_mut);
                tid = numdone;
                ++numdone;
//                cout<<"Locked! "<<tid<<endl;
                if(numdone == thread_count) {
                    g_cv.notify_all();
                } else {
                    g_cv.wait(lock, [&]{return numdone >= thread_count;});
                }
//                cout<<"Working! "<<tid<<endl;
            }
            
            for(int i=ln;; i+=direction) {
                int mod_two = i % 2;
                int rb = ((m+1)*(tid+1)/thread_count);
                for(int j=(m+1)*tid/thread_count; j < rb; j++) {
                    if(j >= vec[i].first) {
                        dp[mod_two][j] = max(dp[1-mod_two][j], dp[1-mod_two][j-vec[i].first] + vec[i].second);
                    } else {
                        dp[mod_two][j] = dp[1-mod_two][j];
                    }
                }
                unique_lock lock(g_mut);
//                cout<<"Done some part "<<tid<<";"<<k<<endl;
                ++numdone;
                if(numdone == (k+1)*thread_count) {
                    g_cv.notify_all();
                } else {
                    g_cv.wait(lock, [&]{return numdone >= (k+1)*thread_count;});
                }
//                cout<<"Waited some part "<<tid<<"; "<<k<<endl;
//                cout<<"Aftermath: "<<numdone<<" "<<(k+1)*thread_count<<endl;
                if(i==rn)
                    break;
                ++k;
            }
            {
                if(!--sub_alive[tl][tr]) {
                    sub_alive[tl][tr].notify_one();
                }
            }
        };
    }
    for(int tid=1; tid<thread_count; tid++) {
//        cout<<"ACQ lock "<<tid+tl<<endl;
        unique_lock lk(f_mut[tid+tl]);
        f_ptrs[tid+tl] = f_ptrs_vec[tid];
        f_cv[tid+tl].notify_one();
    }
    f_ptrs_vec[0]();
    while (int count = sub_alive[tl][tr]) {
        sub_alive[tl][tr].wait(count);
    }
//    cout<<"Gonna return! "<<ln<<" "<<rn<<"!"<<endl;
    return dp[rn%2];
}

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, vector<char>& result, int tl, int tr)
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
    if((rn-ln < 500) || (tl == tr)) {
        dp1 = small(vec, 1, ln, z, m);
        dp2 = small(vec, -1, rn, z+1, m);
    }
    else
    {
        int counter = 0;
        mutex g_mut;
        condition_variable g_cv;
        int mid_thread = (tl+tr)/2;
        {
            unique_lock lg(g_mut);
            counter = 2;
        }
        {
            unique_lock l2(f_mut[mid_thread+1]);
            f_ptrs[mid_thread+1] = [&]{
                dp1 = small_multithread(vec, 1, ln, z, m, mid_thread+1, tr);
                unique_lock lg(g_mut);
                --counter;
                g_cv.notify_one();
            };
        }
        f_cv[mid_thread+1].notify_one();
        dp2 = small_multithread(vec, -1, rn, z+1, m, tl, mid_thread);
        {
            unique_lock lg(g_mut);
            --counter;
            g_cv.wait(lg, [&]{return counter == 0;});
        }
    }
//    cout<<ln<<" "<<rn<<" "<<m<<endl;
    int max_x = 0;
    ll max_value = dp1[0] + dp2[m];
    for(int i=1; i<=m; i++)
        if(ll new_value = dp1[i] + dp2[m-i]; new_value > max_value)
        {
            max_x=i; max_value = new_value;
        }
    return max_value;
    hirschberg(vec, ln, z, max_x, result, tl, tr);
    hirschberg(vec, z+1, rn, m-max_x, result, tl, tr);
    return max_value;
}
