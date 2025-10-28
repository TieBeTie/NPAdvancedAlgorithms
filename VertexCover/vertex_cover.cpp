#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <cassert>

using namespace std;
using namespace std::chrono;

class Graph {
public:
    int n;
    vector<unordered_set<int>> adj;
    vector<pair<int, int>> edges;
    
    Graph(int num_vertices) : n(num_vertices), adj(num_vertices + 1) {}
    
    void addEdge(int u, int v) {
        adj[u].insert(v);
        adj[v].insert(u);
        edges.push_back({u, v});
    }
};

// Algorithm 1, жадный, выбираем вершину с максимальной степенью
vector<int> greedyVertexCover(Graph& g) {
    vector<int> cover;
    vector<unordered_set<int>> adj_copy = g.adj;
    vector<bool> in_cover(g.n + 1, false);
    
    bool has_edges = true;
    while (has_edges) {
        has_edges = false;
        int max_degree = -1;
        int best_vertex = -1;
        
        for (int i = 1; i <= g.n; i++) {
            if (!in_cover[i] && static_cast<int>(adj_copy[i].size()) > max_degree) {
                max_degree = static_cast<int>(adj_copy[i].size());
                best_vertex = i;
                has_edges = true;
            }
        }
        
        if (best_vertex == -1) break;

        cover.push_back(best_vertex);
        in_cover[best_vertex] = true;
        
        for (int neighbor : adj_copy[best_vertex]) {
            adj_copy[neighbor].erase(best_vertex);
        }
        adj_copy[best_vertex].clear();
    }
    
    return cover;
}


// Algorithm 2, через паросочетания
vector<int> matchingVertexCover(Graph& g) {
    vector<int> cover;
    vector<bool> in_cover(g.n + 1, false);
    
    for (const auto& edge : g.edges) {
        int u = edge.first;
        int v = edge.second;
        
        // Если хотя бы одна вершина уже в покрытии, ребро покрыто
        if (in_cover[u] || in_cover[v]) continue;
        
        // Добавляем обе вершины
        if (!in_cover[u]) {
            cover.push_back(u);
            in_cover[u] = true;
        }
        if (!in_cover[v]) {
            cover.push_back(v);
            in_cover[v] = true;
        }
    }
    
    return cover;
}

// Постпроцессинг паросочетания, 
vector<int> postprocessMatchingCover(Graph& g, const vector<int>& cover) {
    vector<int> result;
    vector<bool> in_cover(g.n + 1, false);
    for (int v : cover) {
        in_cover[v] = true;
    }
    
    for (int v : cover) {
        if (std::any_of(g.adj[v].begin(), g.adj[v].end(), [&](int neighbor) {
            return !in_cover[neighbor];
        })) {
            result.push_back(v);
        } else {
            in_cover[v] = false;
        }
    }
    return result;
}

void validateCover(const Graph& g, const vector<int>& cover) {
    vector<bool> in_cover(g.n + 1, false);
    for (int v : cover) {
        in_cover[v] = true;
    }
    for (const auto& edge : g.edges) {
        assert(in_cover[edge.first] || in_cover[edge.second]);
    }
}
Graph readGraph(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        exit(1);
    }
    
    int max_vertex = 0;
    vector<pair<int, int>> edges_list;
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        int u, v;
        if (iss >> u >> v) {
            max_vertex = max(max_vertex, max(u, v));
            edges_list.push_back({u, v});
        }
    }
    
    Graph g(max_vertex);
    for (const auto& edge : edges_list) {
        g.addEdge(edge.first, edge.second);
    }
    
    return g;
}

void writeOutput(const string& filename, const vector<int>& cover) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file for writing: " << filename << endl;
        exit(1);
    }
    
    for (int v : cover) {
        file << v << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <algorithm> [input_file] [output_file]" << endl;
        cerr << "Algorithms: greedy, matching, matching_fast, matching_post, matching_post_simple" << endl;
        return 1;
    }
    
    string algorithm = argv[1];
    string input_file = (argc >= 3) ? argv[2] : "input.txt";
    string output_file = (argc >= 4) ? argv[3] : "output.txt";
    
    auto start = high_resolution_clock::now();
    
    Graph g = readGraph(input_file);
    
    auto read_time = high_resolution_clock::now();
    auto read_duration = duration_cast<milliseconds>(read_time - start);
    
    vector<int> cover;
    auto algo_start = high_resolution_clock::now();
    
    if (algorithm == "greedy") {
        cover = greedyVertexCover(g);
    } else if (algorithm == "matching_fast") {
        cover = matchingVertexCover(g);
    } else if (algorithm == "matching_post") {
        cover = matchingVertexCover(g);
        cover = postprocessMatchingCover(g, cover);
    } else {
        cerr << "Unknown algorithm: " << algorithm << endl;
        return 1;
    }
    
    auto algo_end = high_resolution_clock::now();
    auto algo_duration = duration_cast<milliseconds>(algo_end - algo_start);
    
    sort(cover.begin(), cover.end());

    validateCover(g, cover);
    
    auto write_start = high_resolution_clock::now();
    writeOutput(output_file, cover);
    auto write_end = high_resolution_clock::now();
    auto write_duration = duration_cast<milliseconds>(write_end - write_start);
    
    auto total_end = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(total_end - start);
    
    cerr << "Algorithm: " << algorithm << endl;
    cerr << "Cover size: " << cover.size() << endl;
    cerr << "Read time: " << read_duration.count() << " ms" << endl;
    cerr << "Algorithm time: " << algo_duration.count() << " ms" << endl;
    cerr << "Write time: " << write_duration.count() << " ms" << endl;
    cerr << "Total time: " << total_duration.count() << " ms" << endl;
    
    return 0;
}
