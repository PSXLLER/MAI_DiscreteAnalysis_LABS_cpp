#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <cstdint>

struct TPosition {
    int Line;
    int Word;
};

std::vector<int> BuildPrefixFunction(const std::vector<uint32_t>& pat) {
    int m = static_cast<int>(pat.size());
    std::vector<int> pf(m, 0);
    for (int i = 1; i < m; i++) {
        int j = pf[i - 1];
        while (j > 0 && pat[i] != pat[j]) {
            j = pf[j - 1];
        }
        if (pat[i] == pat[j]) {
            j++;
        }
        pf[i] = j;
    }
    return pf;
}

int main() {
    std::string line;
    std::getline(std::cin, line);

    std::vector<uint32_t> pat;
    {
        std::istringstream iss(line);
        uint32_t v;
        while (iss >> v) {
            pat.push_back(v);
        }
    }

    int m = static_cast<int>(pat.size());
    if (m == 0) {
        return 0;
    }

    std::vector<int> pf = BuildPrefixFunction(pat);

    std::deque<TPosition> win;
    int matched = 0;
    int textLine = 1;

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

    return 0;
}
