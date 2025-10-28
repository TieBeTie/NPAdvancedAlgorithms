#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <limits>

using namespace std;
using namespace std::chrono;

vector<string> readStrings(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        exit(1);
    }
    
    vector<string> strings;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            strings.push_back(line);
        }
    }
    file.close();
    return strings;
}

void writeOutput(const string& filename, const string& result) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot open output file " << filename << endl;
        exit(1);
    }
    file << result << endl;
    file.close();
}

bool isSubstring(const string& s1, const string& s2) {
    return s2.find(s1) != string::npos;
}

vector<string> removeSubstrings(const vector<string>& strings) {
    vector<string> result;
    int n = strings.size();
    
    for (int i = 0; i < n; i++) {
        bool isSub = false;
        for (int j = 0; j < n; j++) {
            if (i != j && isSubstring(strings[i], strings[j])) {
                isSub = true;
                break;
            }
        }
        if (!isSub) {
            result.push_back(strings[i]);
        }
    }
    
    return result;
}

int computeOverlap(const string& s1, const string& s2) {
    int maxLen = min(s1.length(), s2.length());
    for (int k = maxLen; k > 0; k--) {
        if (s1.substr(s1.length() - k) == s2.substr(0, k)) {
            return k;
        }
    }
    return 0;
}

string mergeWithOverlap(const string& s1, const string& s2, int overlap) {
    return s1 + s2.substr(overlap);
}

vector<vector<int>> buildOverlapMatrix(const vector<string>& strings) {
    int n = strings.size();
    vector<vector<int>> overlap(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                overlap[i][j] = computeOverlap(strings[i], strings[j]);
            }
        }
    }
    return overlap;
}

bool validateResult(const vector<string>& strings, const string& result) {
    for (const auto& s : strings) {
        if (result.find(s) == string::npos) {
            cerr << "Validation failed: missing string" << endl;
            return false;
        }
    }
    return true;
}

string mergePath(const vector<string>& strings, const vector<int>& path, 
                 const vector<vector<int>>& overlap) {
    if (path.empty()) return "";
    
    string result = strings[path[0]];
    for (size_t i = 1; i < path.size(); i++) {
        int ovl = overlap[path[i-1]][path[i]];
        result = mergeWithOverlap(result, strings[path[i]], ovl);
    }
    return result;
}

string dpBitmask(const vector<string>& strings) {
    int n = strings.size();
    if (n == 0) return "";
    if (n == 1) return strings[0];
    if (n > 18) return "";
    
    auto overlap = buildOverlapMatrix(strings);
    
    int fullMask = (1 << n) - 1;
    vector<vector<int>> dp(1 << n, vector<int>(n, 1e9));
    vector<vector<int>> parent(1 << n, vector<int>(n, -1));
    
    for (int i = 0; i < n; i++) {
        dp[1 << i][i] = strings[i].length();
    }
    
    for (int mask = 0; mask <= fullMask; mask++) {
        for (int i = 0; i < n; i++) {
            if (!(mask & (1 << i)) || dp[mask][i] == 1e9) continue;
            
            for (int j = 0; j < n; j++) {
                if (mask & (1 << j)) continue;
                
                int newMask = mask | (1 << j);
                int newLen = dp[mask][i] + (int)strings[j].length() - overlap[i][j];
                
                if (newLen < dp[newMask][j]) {
                    dp[newMask][j] = newLen;
                    parent[newMask][j] = i;
                }
            }
        }
    }
    
    int bestLen = 1e9;
    int lastIdx = -1;
    for (int i = 0; i < n; i++) {
        if (dp[fullMask][i] < bestLen) {
            bestLen = dp[fullMask][i];
            lastIdx = i;
        }
    }
    
    vector<int> path;
    int mask = fullMask;
    int cur = lastIdx;
    while (cur != -1) {
        path.push_back(cur);
        int prev = parent[mask][cur];
        if (prev != -1) {
            mask ^= (1 << cur);
        }
        cur = prev;
    }
    reverse(path.begin(), path.end());
    
    return mergePath(strings, path, overlap);
}

string greedyWithLookahead(const vector<string>& strings, int lookAhead) {
    int n = strings.size();
    if (n == 0) return "";
    if (n == 1) return strings[0];
    
    auto overlap = buildOverlapMatrix(strings);
    vector<bool> used(n, false);
    vector<int> path;
    
    path.push_back(0);
    used[0] = true;
    
    for (int iter = 1; iter < n; iter++) {
        int current = path.back();
        int bestNext = -1;
        int bestScore = -1e9;
        
        for (int j = 0; j < n; j++) {
            if (used[j]) continue;
            
            int score = overlap[current][j];
            
            if (lookAhead > 0) {
                int maxFutureOvl = 0;
                for (int k = 0; k < n; k++) {
                    if (!used[k] && k != j) {
                        maxFutureOvl = max(maxFutureOvl, overlap[j][k]);
                    }
                }
                score += maxFutureOvl / 2;
            }
            
            if (score > bestScore) {
                bestScore = score;
                bestNext = j;
            }
        }
        
        if (bestNext != -1) {
            path.push_back(bestNext);
            used[bestNext] = true;
        }
    }
    
    return mergePath(strings, path, overlap);
}

string greedyBidirectional(const vector<string>& strings) {
    int n = strings.size();
    if (n == 0) return "";
    if (n == 1) return strings[0];
    
    auto overlap = buildOverlapMatrix(strings);
    
    vector<pair<int, int>> maxOvl(n);
    for (int i = 0; i < n; i++) {
        int mx = 0;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                mx = max(mx, max(overlap[i][j], overlap[j][i]));
            }
        }
        maxOvl[i] = {mx, i};
    }
    sort(maxOvl.rbegin(), maxOvl.rend());
    
    vector<bool> used(n, false);
    string result = strings[maxOvl[0].second];
    used[maxOvl[0].second] = true;
    
    while (count(used.begin(), used.end(), false) > 0) {
        int bestIdx = -1;
        int bestOvl = 0;
        bool appendRight = true;
        
        for (int i = 0; i < n; i++) {
            if (!used[i]) {
                int ovl = computeOverlap(result, strings[i]);
                if (ovl > bestOvl) {
                    bestOvl = ovl;
                    bestIdx = i;
                    appendRight = true;
                }
            }
        }
        
        for (int i = 0; i < n; i++) {
            if (!used[i]) {
                int ovl = computeOverlap(strings[i], result);
                if (ovl > bestOvl) {
                    bestOvl = ovl;
                    bestIdx = i;
                    appendRight = false;
                }
            }
        }
        
        if (bestIdx == -1) {
            for (int i = 0; i < n; i++) {
                if (!used[i]) {
                    result += strings[i];
                    used[i] = true;
                    break;
                }
            }
        } else {
            used[bestIdx] = true;
            if (appendRight) {
                result = mergeWithOverlap(result, strings[bestIdx], bestOvl);
            } else {
                result = mergeWithOverlap(strings[bestIdx], result, bestOvl);
            }
        }
    }
    
    return result;
}

vector<vector<int>> partitionIntoClusters(const vector<string>& strings, int maxClusterSize) {
    int n = strings.size();
    auto overlap = buildOverlapMatrix(strings);
    
    vector<bool> used(n, false);
    vector<vector<int>> clusters;
    
    for (int start = 0; start < n; start++) {
        if (used[start]) continue;
        
        vector<int> cluster;
        cluster.push_back(start);
        used[start] = true;
        
        while ((int)cluster.size() < maxClusterSize) {
            int bestNext = -1;
            int bestOvl = 0;
            
            for (int i : cluster) {
                for (int j = 0; j < n; j++) {
                    if (!used[j] && overlap[i][j] > bestOvl) {
                        bestOvl = overlap[i][j];
                        bestNext = j;
                    }
                }
            }
            
            if (bestNext == -1) break;
            cluster.push_back(bestNext);
            used[bestNext] = true;
        }
        
        clusters.push_back(cluster);
    }
    
    return clusters;
}

string solveCluster(const vector<string>& clusterStrings) {
    if (clusterStrings.size() <= 15) {
        return dpBitmask(clusterStrings);
    }
    return greedyWithLookahead(clusterStrings, 2);
}

string hybridClusterDP(const vector<string>& strings) {
    int n = strings.size();
    if (n == 0) return "";
    if (n <= 15) return dpBitmask(strings);
    
    const int clusterSize = 14;
    auto clusters = partitionIntoClusters(strings, clusterSize);
    
    vector<string> clusterResults;
    for (const auto& cluster : clusters) {
        vector<string> clusterStrings;
        for (int idx : cluster) {
            clusterStrings.push_back(strings[idx]);
        }
        clusterResults.push_back(solveCluster(clusterStrings));
    }
    
    if (clusterResults.size() <= 15) {
        return dpBitmask(clusterResults);
    }
    return greedyWithLookahead(clusterResults, 3);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <algorithm> [input_file] [output_file]" << endl;
        cerr << "Algorithms: lookahead, bidirectional, hybrid" << endl;
        return 1;
    }
    
    string algorithm = argv[1];
    string input_file = (argc >= 3) ? argv[2] : "input.txt";
    string output_file = (argc >= 4) ? argv[3] : "output.txt";
    
    auto start = high_resolution_clock::now();
    
    vector<string> strings = readStrings(input_file);
    vector<string> cleaned = removeSubstrings(strings);
    
    auto read_time = high_resolution_clock::now();
    auto read_duration = duration_cast<milliseconds>(read_time - start);
    
    string result;
    auto algo_start = high_resolution_clock::now();
    
    if (algorithm == "lookahead") {
        result = greedyWithLookahead(cleaned, 3);
    } else if (algorithm == "bidirectional") {
        result = greedyBidirectional(cleaned);
    } else if (algorithm == "hybrid") {
        result = hybridClusterDP(cleaned);
    } else {
        cerr << "Unknown algorithm: " << algorithm << endl;
        return 1;
    }
    
    auto algo_end = high_resolution_clock::now();
    auto algo_duration = duration_cast<milliseconds>(algo_end - algo_start);
    
    writeOutput(output_file, result);
    
    if (!validateResult(strings, result)) {
        return 1;
    }
    
    auto total_end = high_resolution_clock::now();
    auto total_duration = duration_cast<milliseconds>(total_end - start);
    
    cerr << "Algorithm: " << algorithm << endl;
    cerr << "Strings: " << strings.size() << endl;
    cerr << "Superstring length: " << result.length() << endl;
    cerr << "Read time: " << read_duration.count() << " ms" << endl;
    cerr << "Algorithm time: " << algo_duration.count() << " ms" << endl;
    cerr << "Total time: " << total_duration.count() << " ms" << endl;
    
    return 0;
}