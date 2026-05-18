#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <algorithm>

const int INF = INT_MAX / 2;
const int NO_NODE = -1;
const int ROOT = 0;
const char SENTINEL = '{';
const int ALPHABET = 27;
const int SENTINEL_IDX = 26;

struct TNode {
    int Children[ALPHABET];
    int SuffixLink;
    int Start;
    int End;

    TNode(int start, int end) : Start(start), End(end), SuffixLink(NO_NODE) {
        std::fill(Children, Children + ALPHABET, NO_NODE);
    }
};

std::vector<TNode> nodes;
std::string text;
int activeNode;
int activeEdge;
int activeLen;
int remaining;
int globalEnd;
int lastNewInternal;

int CharIdx(char c) {
    if (c == SENTINEL) {
        return SENTINEL_IDX;
    }
    return c - 'a';
}

int EdgeLen(int idx) {
    return std::min(nodes[idx].End, globalEnd) - nodes[idx].Start;
}

int NewNode(int start, int end) {
    nodes.emplace_back(start, end);
    return (int)nodes.size() - 1;
}

void Extend(int pos) {
    globalEnd = pos + 1;
    remaining++;
    lastNewInternal = NO_NODE;

    while (remaining > 0) {
        if (activeLen == 0) {
            activeEdge = pos;
        }
        int ac = CharIdx(text[activeEdge]);
        if (nodes[activeNode].Children[ac] == NO_NODE) {
            nodes[activeNode].Children[ac] = NewNode(pos, INF);
            if (lastNewInternal != NO_NODE) {
                nodes[lastNewInternal].SuffixLink = activeNode;
                lastNewInternal = NO_NODE;
            }
        } else {
            int nxt = nodes[activeNode].Children[ac];
            int el = EdgeLen(nxt);
            if (activeLen >= el) {
                activeEdge += el;
                activeLen -= el;
                activeNode = nxt;
                continue;
            }
            if (text[nodes[nxt].Start + activeLen] == text[pos]) {
                activeLen++;
                if (lastNewInternal != NO_NODE) {
                    nodes[lastNewInternal].SuffixLink = activeNode;
                }
                break;
            }
            int split = NewNode(nodes[nxt].Start, nodes[nxt].Start + activeLen);
            nodes[activeNode].Children[ac] = split;
            nodes[split].Children[CharIdx(text[pos])] = NewNode(pos, INF);
            nodes[nxt].Start += activeLen;
            nodes[split].Children[CharIdx(text[nodes[nxt].Start])] = nxt;
            if (lastNewInternal != NO_NODE) {
                nodes[lastNewInternal].SuffixLink = split;
            }
            lastNewInternal = split;
        }
        remaining--;
        if (activeNode == ROOT && activeLen > 0) {
            activeLen--;
            activeEdge = pos - remaining + 1;
        } else if (nodes[activeNode].SuffixLink != NO_NODE) {
            activeNode = nodes[activeNode].SuffixLink;
        } else {
            activeNode = ROOT;
        }
    }
}

void Build(const std::string& s) {
    text = s;
    nodes.clear();
    NewNode(NO_NODE, NO_NODE);
    nodes[ROOT].SuffixLink = ROOT;
    activeNode = ROOT;
    activeEdge = 0;
    activeLen = 0;
    remaining = 0;
    globalEnd = 0;
    lastNewInternal = NO_NODE;
    for (int i = 0; i < (int)s.size(); i++) {
        Extend(i);
    }
}

int MinChild(int node) {
    for (int c = 0; c < ALPHABET; c++) {
        if (nodes[node].Children[c] != NO_NODE) {
            return nodes[node].Children[c];
        }
    }
    return NO_NODE;
}

std::string FindMinRotation(int n) {
    std::string result;
    int node = ROOT;
    while ((int)result.size() < n) {
        int child = MinChild(node);
        int start = nodes[child].Start;
        int end = std::min(nodes[child].End, globalEnd);
        for (int i = start; i < end && (int)result.size() < n; i++) {
            result += text[i];
        }
        node = child;
    }
    return result;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string s;
    std::cin >> s;
    int n = (int)s.size();

    Build(s + s + SENTINEL);

    std::cout << FindMinRotation(n) << "\n";

    return 0;
}
