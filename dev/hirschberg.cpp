#include <iostream>
#include <vector>
#include <queue>

using namespace std;
using ll = long long;

void small(vector<pair<int, int>>& vec, int inv,
           int ln, int rn, int m, vector<ll>& dp);

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, queue<int>& result);

int main()
{
    cin.tie(0)->sync_with_stdio(0);
    int t;
    cin>>t;
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
        queue<int> res;
        cout<<hirschberg(vec, 0, n-1, m, res)<<" ";
        cout<<res.size()<<"\n";
        while(!res.empty())
        {
            cout<<res.front()<<" ";
            res.pop();
        }
        cout<<"\n";
    }
}

void small(vector<pair<int, int>>& vec, int inv,
           int ln, int rn, int m, vector<ll>& dp)
{
//    cout<<"Small "<<ln<<" "<<rn<<" "<<m<<" "<<inv<<":\n";
    dp.clear();
    dp.resize(m+1);
    for(int i=ln;; i+=inv) {
        for(int j=m; j >= vec[i].first; j--)
            dp[j] = max(dp[j], dp[j-vec[i].first] + vec[i].second);
        if(i==rn)
            break;
    }
}

ll hirschberg(vector<pair<int, int>>& vec,
           int ln, int rn, int m, queue<int>& result)
{
//    cout<<"Hirsc "<<ln<<" "<<rn<<" "<<m<<":\n";
    if(ln>rn)
    {
        throw;
    }
    if(ln==rn)
    {
        if(vec[ln].first <= m) {
            result.push(ln+1);
            return vec[ln].second;
        }
        return 0;
    }
    vector<ll> dp1, dp2;
    int z = (rn+ln)/2;
    small(vec, 1, ln, z, m, dp1);
    small(vec, -1, rn, z+1, m, dp2);
    int mx = 0;
    ll mv = dp1[0] + dp2[m];
    for(int i=1; i<=m; i++)
        if(ll nv = dp1[i] + dp2[m-i]; nv > mv)
        {
            mx=i; mv = nv;
        }
    hirschberg(vec, ln, z, mx, result);
    hirschberg(vec, z+1, rn, m-mx, result);
    return mv;
}
