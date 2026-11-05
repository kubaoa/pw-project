#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <omp.h>

using namespace std;
using ll = long long;

vector<ll> small(vector<pair<int, int>>& vec, int direction,
                 int ln, int rn, int m);

ll hirschberg(vector<pair<int, int>>& vec,
               int ln, int rn, int m, vector<char>& result);

int main()
{
    // KLUCZOWE: Pozwalamy na dokładnie 2 poziomy zagnieżdżenia OpenMP
    // Poziom 1 = 2 sekcje w hirschberg
    // Poziom 2 = 4 wątki wewnątrz pętli small
    omp_set_max_active_levels(2);

    cin.tie(0)->sync_with_stdio(0);
    int t;
    if (!(cin >> t)) return 0;

    while (t--) {
        int n, m;
        cin >> n >> m;
        if (n == 0) {
            cout << "0 0\n\n";
            continue;
        }
        vector<pair<int, int>> vec(n);
        for (auto& [x, y] : vec)
            cin >> x >> y;

        vector<char> res(n + 1, 0);

        cout << hirschberg(vec, 0, n - 1, m, res) << " ";

        int ans = 0;
        for (auto x : res) {
            ans += (int)x;
        }
        cout << ans << "\n";
        for (int i = 1; i <= n; i++)
            if (res[i])
                cout << i << " ";
        cout << "\n";
    }
    return 0;
}

vector<ll> small(vector<pair<int, int>>& vec, int direction,
                 int ln, int rn, int m)
{
    array<vector<ll>, 2> dp{vector<ll>(m + 1), vector<ll>(m + 1)};

    // POZIOM 2: Każdy z 2 'smalli' tworzy 4 wątki dla swojej pętli j (razem 8 wątków w systemie)
    #pragma omp parallel num_threads(4) if(m > 200)
    for (int i = ln;; i += direction) {
        int mod_two = i % 2;
        
        #pragma omp for schedule(static)
        for (int j = 0; j <= m; j++) {
            if (j >= vec[i].first) {
                dp[mod_two][j] = max(dp[1 - mod_two][j], dp[1 - mod_two][j - vec[i].first] + vec[i].second);
            } else {
                dp[mod_two][j] = dp[1 - mod_two][j];
            }
        }
        if (i == rn)
            break;
    }
    return dp[rn % 2];
}

ll hirschberg(vector<pair<int, int>>& vec,
               int ln, int rn, int m, vector<char>& result)
{
    if (ln > rn) throw;
    if (ln == rn) {
        if (vec[ln].first <= m) {
            result[ln + 1] = 1;
            return vec[ln].second;
        }
        return 0;
    }

    int z = (rn + ln) / 2;
    vector<ll> dp1, dp2;

    // POZIOM 1: Uruchamiamy dp1 i dp2 RÓWNOLEGLE na 2 osobnych wątkach
    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {
            dp1 = small(vec, 1, ln, z, m);
        }

        #pragma omp section
        {
            dp2 = small(vec, -1, rn, z + 1, m);
        }
    } // Niewidoczna bariera: kod czeka, aż OBA smalle się zakończą

    int max_x = 0;
    ll max_value = dp1[0] + dp2[m];
    for (int i = 1; i <= m; i++) {
        if (ll new_value = dp1[i] + dp2[m - i]; new_value > max_value) {
            max_x = i;
            max_value = new_value;
        }
    }

    hirschberg(vec, ln, z, max_x, result);
    hirschberg(vec, z + 1, rn, m - max_x, result);
    return max_value;
}
