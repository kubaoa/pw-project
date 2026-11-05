## Źródła:
- `hirschberg.cpp` - rozwiązanie jednowątkowe
- `omp_2_threads.cpp` - rozwiązanie dwuwątkowe. Równolegle uruchamiamy 2 `small()`
- `omp_small` - zrównoleglamy `small()`
- `omp_mixed` - uruchamiamy równocześnie 2 `small()`, oba zrównoleglamy niezależnie
- `std_thread` - ręczny scheduling, drzewko wątków. Najlepsze wyniki dla T = 2^k

## Uruchamianie:
`Makefile` buduje wszystko.

`omp_small`, `omp_mixed` oraz `std_thread` oczekują liczby wątków jako argumentu wiersza poleceń. Polecam 4, 6, 8.

## Testy:
- `1.in` - mały smoke test
- `2.in` - maksymalny test z satori
- `3.in` - 10x większy niż max z satori