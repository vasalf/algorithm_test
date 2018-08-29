#include <bits/stdc++.h>
#include "include/solutions/nlogn.h"

using namespace std;

int main() {
    string s;
    cin >> s;
    auto sa = nlogn::sufarray(s);
    for (auto u : sa)
        cout << u << " ";
    cout << endl;
    return 0;
}
