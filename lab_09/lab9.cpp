#include <iostream>
#include <vector>
#include <limits>

const long INF = std::numeric_limits<long>::max();

struct Edge {
    unsigned long from, to;
    long weight;

    friend std::istream& operator>>(std::istream& is, Edge& e);
};

std::istream& operator>>(std::istream& is, Edge& e) {
    unsigned long from, to;
    is >> from;
    is >> to;
    e.from = from - 1;
    e.to = to - 1;
    is >> e.weight;
    return is;
}

long BellmanFord(const std::vector<Edge>& edges, const unsigned long n, const unsigned long m, 
                        const unsigned long start, const unsigned long finish) {
    std::vector<long> minDistFromStart(n, INF);
    minDistFromStart[start] = 0;
    for (unsigned long relaxN = 1; relaxN <= n - 1; ++relaxN) {
        bool relaxation = false;
        for (auto edge : edges) {
            if (minDistFromStart[edge.from] < INF) {
                if (minDistFromStart[edge.from] + edge.weight < minDistFromStart[edge.to]) {
                    minDistFromStart[edge.to] = minDistFromStart[edge.from] + edge.weight;
                    relaxation = true;
                }
            }
        }
        if (relaxation == false) {
            break;
        }
    }

    return minDistFromStart[finish];
}

int main() {
    std::ios_base::sync_with_stdio(false);

    unsigned long n, m, start, finish;
    std::cin >> n >> m >> start >> finish;
    --start;
    --finish;

    std::vector<Edge> edges(m);
    for (auto i = 0; i < m; ++i) {
        std::cin >> edges[i];
    }

    long ans = BellmanFord(edges, n, m, start, finish);
    if (ans == INF) {
        std::cout << "No solution" << std::endl;
    } else {
        std::cout << ans << std::endl;
    }

    return 0;
}
