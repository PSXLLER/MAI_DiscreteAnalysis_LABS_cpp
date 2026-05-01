#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdint>

struct TPosition {
    int Line;
    int Word;
};

std::vector<uint32_t> ReadPattern() {
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<uint32_t> pat;
    uint32_t v;
    while (iss >> v) {
        pat.push_back(v);
    }
    return pat;
}

std::vector<int> BuildZFunction(const std::vector<uint32_t>& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> z(n, 0);
    int l = 0, r = 0;
    for (int i = 1; i < n; i++) {
        if (i < r) {
            z[i] = std::min(r - i, z[i - l]);
        }
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }
    return z;
}

std::vector<int> BuildPrefixFunction(const std::vector<uint32_t>& pat) {
    int m = static_cast<int>(pat.size());

    std::vector<int> z = BuildZFunction(pat);

    std::vector<int> pf(m, 0);
    for (int i = 1; i < m; i++) {
        if (z[i] > 0) {
            pf[i + z[i] - 1] = std::max(pf[i + z[i] - 1], z[i]);
        }
    }
    for (int i = m - 2; i >= 0; i--) {
        if (pf[i + 1] > 0) {
            pf[i] = std::max(pf[i], pf[i + 1] - 1);
        }
    }

    z.clear();
    z.shrink_to_fit();

    return pf;
}

void Search(const std::vector<uint32_t>& pat, const std::vector<int>& pf) {
    int m = static_cast<int>(pat.size());
    std::deque<TPosition> win;
    int matched = 0;
    int textLine = 1;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        uint32_t v;
        int word = 0;
        while (iss >> v) {
            word++;
            win.push_back({textLine, word});
            if (static_cast<int>(win.size()) > m) {
                win.pop_front();
            }
            while (matched > 0 && v != pat[matched]) {
                matched = pf[matched - 1];
            }
            if (v == pat[matched]) {
                matched++;
            }
            if (matched == m) {
                std::cout << win.front().Line << ", " << win.front().Word << "\n";
                matched = pf[matched - 1];
            }
        }
        textLine++;
    }
}

int main() {
    std::vector<uint32_t> pat = ReadPattern();
    if (pat.empty()) {
        return 0;
    }
    std::vector<int> pf = BuildPrefixFunction(pat);
    Search(pat, pf);
    return 0;
}
