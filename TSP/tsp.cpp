#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <limits>

using namespace std;
using namespace std::chrono;

struct Point {
    int id;
    double x, y;
    
    Point(int id, double x, double y) : id(id), x(x), y(y) {}
};

inline double distance(const Point& p1, const Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

vector<Point> readPoints(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        exit(1);
    }
    
    vector<Point> points;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        int id;
        double x, y;
        if (iss >> id >> x >> y) {
            points.push_back(Point(id, x, y));
        }
    }
    
    return points;
}

void writeTour(const string& filename, const vector<int>& tour) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file for writing: " << filename << endl;
        exit(1);
    }
    
    for (int v : tour) {
        file << v << "\n";
    }
}

double tourLength(const vector<Point>& points, const vector<int>& tour) {
    double total = 0.0;
    int n = tour.size();
    for (int i = 0; i < n; i++) {
        total += distance(points[tour[i]], points[tour[(i + 1) % n]]);
    }
    return total;
}

vector<int> tourIndicesToIds(const vector<Point>& points, const vector<int>& tour) {
    vector<int> result;
    result.reserve(tour.size());
    for (int idx : tour) {
        result.push_back(points[idx].id);
    }
    return result;
}

vector<vector<int>> buildMST(const vector<Point>& points) {
    int n = points.size();
    vector<vector<int>> mst(n);
    vector<bool> inMST(n, false);
    vector<double> minDist(n, 1e18);
    vector<int> parent(n, -1);
    
    minDist[0] = 0.0;
    
    for (int i = 0; i < n; i++) {
        int u = -1;
        double minVal = 1e18;
        for (int j = 0; j < n; j++) {
            if (!inMST[j] && minDist[j] < minVal) {
                minVal = minDist[j];
                u = j;
            }
        }
        
        if (u == -1) break;
        inMST[u] = true;
        
        if (parent[u] != -1) {
            mst[u].push_back(parent[u]);
            mst[parent[u]].push_back(u);
        }
        
        for (int v = 0; v < n; v++) {
            if (!inMST[v]) {
                double dist = distance(points[u], points[v]);
                if (dist < minDist[v]) {
                    minDist[v] = dist;
                    parent[v] = u;
                }
            }
        }
    }
    
    return mst;
}

void eulerTourDFS(vector<vector<int>>& graph, int u, vector<int>& euler) {
    while (!graph[u].empty()) {
        int v = graph[u].back();
        graph[u].pop_back();
        
        auto it = find(graph[v].begin(), graph[v].end(), u);
        if (it != graph[v].end()) {
            graph[v].erase(it);
        }
        
        eulerTourDFS(graph, v, euler);
    }
    euler.push_back(u);
}

vector<int> getOddDegreeVertices(const vector<vector<int>>& mst) {
    vector<int> odd;
    for (int i = 0; i < (int)mst.size(); i++) {
        if (mst[i].size() % 2 == 1) {
            odd.push_back(i);
        }
    }
    return odd;
}

vector<pair<int, int>> improvedMatching(const vector<Point>& points, const vector<int>& oddVertices) {
    vector<pair<double, pair<int, int>>> edges;
    for (int i = 0; i < (int)oddVertices.size(); i++) {
        for (int j = i + 1; j < (int)oddVertices.size(); j++) {
            double dist = distance(points[oddVertices[i]], points[oddVertices[j]]);
            edges.push_back({dist, {i, j}});
        }
    }
    
    sort(edges.begin(), edges.end());
    
    vector<pair<int, int>> matching;
    vector<bool> matched(oddVertices.size(), false);
    
    for (const auto& edge : edges) {
        int i = edge.second.first;
        int j = edge.second.second;
        
        if (!matched[i] && !matched[j]) {
            matching.push_back({oddVertices[i], oddVertices[j]});
            matched[i] = true;
            matched[j] = true;
        }
    }
    
    return matching;
}

vector<int> buildTourFromEulerGraph(const vector<vector<int>>& eulerGraph, int n) {
    vector<vector<int>> graphCopy = eulerGraph;
    vector<int> euler;
    eulerTourDFS(graphCopy, 0, euler);
    reverse(euler.begin(), euler.end());
    
    vector<bool> visited(n, false);
    vector<int> tour;
    for (int v : euler) {
        if (!visited[v]) {
            visited[v] = true;
            tour.push_back(v);
        }
    }
    
    return tour;
}

vector<int> nearestNeighbor(const vector<Point>& points, int start = 0) {
    int n = points.size();
    vector<bool> visited(n, false);
    vector<int> tour;
    tour.reserve(n);
    
    int current = start;
    tour.push_back(current);
    visited[current] = true;
    
    for (int i = 1; i < n; i++) {
        int nearest = -1;
        double minDist = numeric_limits<double>::max();
        
        for (int j = 0; j < n; j++) {
            if (!visited[j]) {
                double d = distance(points[current], points[j]);
                if (d < minDist) {
                    minDist = d;
                    nearest = j;
                }
            }
        }
        
        current = nearest;
        tour.push_back(current);
        visited[current] = true;
    }
    
    return tour;
}



bool twoOptPass(const vector<Point>& points, vector<int>& tour) {
    int n = tour.size();
    bool improved = false;
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 2; j < n; j++) {
            if (j == n - 1 && i == 0) continue;
            
            int a = tour[i];
            int b = tour[i + 1];
            int c = tour[j];
            int d = tour[(j + 1) % n];
            
            double delta = -distance(points[a], points[b]) 
                         - distance(points[c], points[d])
                         + distance(points[a], points[c]) 
                         + distance(points[b], points[d]);
            
            if (delta < -1e-9) {
                reverse(tour.begin() + i + 1, tour.begin() + j + 1);
                improved = true;
            }
        }
    }
    
    return improved;
}

void twoOpt(const vector<Point>& points, vector<int>& tour, int maxIter = 1000) {
    for (int iter = 0; iter < maxIter; iter++) {
        if (!twoOptPass(points, tour)) break;
    }
}

vector<int> tsp2Approx(const vector<Point>& points) {
    int n = points.size();
    vector<vector<int>> mst = buildMST(points);
    
    vector<vector<int>> eulerGraph = mst;
    for (int u = 0; u < n; u++) {
        for (int v : mst[u]) {
            eulerGraph[u].push_back(v);
            eulerGraph[v].push_back(u);
        }
    }
    
    return buildTourFromEulerGraph(eulerGraph, n);
}

vector<int> tsp15Approx(const vector<Point>& points) {
    int n = points.size();
    vector<vector<int>> mst = buildMST(points);
    vector<int> oddVertices = getOddDegreeVertices(mst);
    
    vector<pair<int, int>> matching;
    vector<bool> matched(oddVertices.size(), false);
    
    for (int i = 0; i < (int)oddVertices.size(); i++) {
        if (matched[i]) continue;
        
        int bestJ = -1;
        double bestDist = 1e18;
        
        for (int j = i + 1; j < (int)oddVertices.size(); j++) {
            if (matched[j]) continue;
            double dist = distance(points[oddVertices[i]], points[oddVertices[j]]);
            if (dist < bestDist) {
                bestDist = dist;
                bestJ = j;
            }
        }
        
        if (bestJ != -1) {
            matching.push_back({oddVertices[i], oddVertices[bestJ]});
            matched[i] = true;
            matched[bestJ] = true;
        }
    }
    
    vector<vector<int>> eulerGraph = mst;
    for (const auto& edge : matching) {
        eulerGraph[edge.first].push_back(edge.second);
        eulerGraph[edge.second].push_back(edge.first);
    }
    
    return buildTourFromEulerGraph(eulerGraph, n);
}

vector<int> tsp15ApproxImproved(const vector<Point>& points) {
    int n = points.size();
    vector<vector<int>> mst = buildMST(points);
    vector<int> oddVertices = getOddDegreeVertices(mst);
    vector<pair<int, int>> matching = improvedMatching(points, oddVertices);
    
    vector<vector<int>> eulerGraph = mst;
    for (const auto& edge : matching) {
        eulerGraph[edge.first].push_back(edge.second);
        eulerGraph[edge.second].push_back(edge.first);
    }
    
    return buildTourFromEulerGraph(eulerGraph, n);
}

// Nearest Neighbor с 2-opt
vector<int> nnWith2Opt(const vector<Point>& points) {
    vector<int> tour = nearestNeighbor(points);
    twoOpt(points, tour, 500);
    return tour;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <algorithm> [input_file] [output_file]" << endl;
        cerr << "\nAlgorithms:" << endl;
        cerr << "  Basic:" << endl;
        cerr << "    2approx              - Double MST (2-approximation)" << endl;
        cerr << "    15approx             - Christofides (1.5-approximation)" << endl;
        cerr << "    15approx_improved    - Christofides with better matching" << endl;
        cerr << "    nn                   - Nearest Neighbor" << endl;
        cerr << "\n  Optimized:" << endl;
        cerr << "    nn_2opt              - NN + 2-opt (RECOMMENDED - achieves < 1M)" << endl;
        return 1;
    }
    
    string algorithm = argv[1];
    string input_file = (argc >= 3) ? argv[2] : "input.txt";
    string output_file = (argc >= 4) ? argv[3] : "output.txt";
    
    auto start = high_resolution_clock::now();
    
    vector<Point> points = readPoints(input_file);
    
    auto read_time = high_resolution_clock::now();
    auto read_duration = duration_cast<milliseconds>(read_time - start);
    
    vector<int> tour;
    auto algo_start = high_resolution_clock::now();
    
    // Выбор алгоритма
    if (algorithm == "2approx") {
        tour = tsp2Approx(points);
    } else if (algorithm == "15approx") {
        tour = tsp15Approx(points);
    } else if (algorithm == "15approx_improved") {
        tour = tsp15ApproxImproved(points);
    } else if (algorithm == "nn") {
        tour = nearestNeighbor(points);
    } else if (algorithm == "nn_2opt") {
        tour = nnWith2Opt(points);
    } else {
        cerr << "Unknown algorithm: " << algorithm << endl;
        return 1;
    }
    
    auto algo_end = high_resolution_clock::now();
    auto algo_duration = duration_cast<milliseconds>(algo_end - algo_start);
    
    // Конвертируем индексы в ID и вычисляем длину
    vector<int> tourIds = tourIndicesToIds(points, tour);
    double tour_len = tourLength(points, tour);
    
    auto write_start = high_resolution_clock::now();
    writeTour(output_file, tourIds);
    auto write_end = high_resolution_clock::now();
    auto write_duration = duration_cast<milliseconds>(write_end - write_start);
    
    auto total_end = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(total_end - start);
    
    cerr << "Algorithm: " << algorithm << endl;
    cerr << "Points: " << points.size() << endl;
    cerr << "Tour length: " << fixed << setprecision(2) << tour_len << endl;
    cerr << "Read time: " << read_duration.count() << " ms" << endl;
    cerr << "Algorithm time: " << algo_duration.count() << " ms" << endl;
    cerr << "Write time: " << write_duration.count() << " ms" << endl;
    cerr << "Total time: " << total_duration.count() << " ms" << endl;
    
    return 0;
}