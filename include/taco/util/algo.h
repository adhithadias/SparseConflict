#ifndef TACO_UTIL_ALGO_H
#define TACO_UTIL_ALGO_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <queue>

using namespace std;

namespace taco {
namespace util {

#include "taco/index_notation/index_notation.h"

bool isSparse(const TensorVar& access);

// Function to merge two sequences while maintaining order
struct IndexHash {
    size_t operator()(const IndexVar& index) const {
        return std::hash<std::string>()(index.getName());
    }

    size_t operator()(const string& index) const {
        return std::hash<std::string>()(index);
    }
};

struct CompareSeq {
    template <typename Type>
    bool operator()(const vector<Type>& a, const vector<Type>& b) const {
        // first compare by size 
        if (a.size() < b.size()) {
            return true;
        } else if (a.size() > b.size()) {
            return false;
        }
        return true;
    }
};

// Helper function to print a sequence
template <typename Type>
void printSequence(const vector<Type>& seq) {
    for (const Type& num : seq) {
        cout << num << " ";
    }
    cout << endl;
}

template <typename Type>
void printSequences(const set<vector<Type>>& sequences) {
    for (const vector<Type>& seq : sequences) {
        printSequence(seq);
    }
}

template <typename Type>
vector<Type> mergeSequences(const vector<Type>& seq1, 
    const vector<Type>& seq2, const vector<Type>& fixedSeq = {}) {
    // std::cout << "fixedSeq size: " << fixedSeq.size() << std::endl;
    vector<Type> result;
    unordered_map<Type, int, IndexHash> pos;
    
    // Store positions of elements in seq1
    for (size_t i = 0; i < seq1.size(); ++i) {
        pos[seq1[i]] = i;
    }

    size_t i = 0, j = 0;

    size_t p = 0;
    while (p < fixedSeq.size() && i < seq1.size()) {
        // std::cout << "fixedSeq[p]: " << fixedSeq[p] << " seq1[i]: " << seq1[i] << std::endl;
        i = fixedSeq[p] == seq1[i] ? i + 1 : i;
        ++p;
    }

    p = 0;
    while (p < fixedSeq.size() && j < seq2.size()) {
        // std::cout << "fixedSeq[p]: " << fixedSeq[p] << " seq2[j]: " << seq2[j] << std::endl;
        j = fixedSeq[p] == seq2[j] ? j + 1 : j;
        ++p;
    }
    
    // Merge sequences while maintaining order
    while (i < seq1.size() && j < seq2.size()) {
        if (seq1[i] == seq2[j]) {
            result.push_back(seq1[i]);
            ++i;
            ++j;
        } else if (pos.find(seq2[j]) == pos.end()) { // New element in seq2
            result.push_back(seq2[j]);
            ++j;
        } else {
            result.push_back(seq1[i]);
            ++i;
        }
    }
    
    // Add remaining elements from seq1
    while (i < seq1.size()) {
        result.push_back(seq1[i++]);
    }
    
    // Add remaining elements from seq2
    while (j < seq2.size()) {
        result.push_back(seq2[j++]);
    }

    return result;
}

// Function to compute the shortest common supersequence for multiple sequences
/*
* Given a list of sequences, find the shortest common supersequence
* that contains each sequence as a subsequence.
* If fixedSeq is provided, it will be added at the beginning of the result.
* E.g., for sequences = {{"i", "j"}, {"j", "i"}} and fixedSeq = {},
* the result can be {"i", "j", "i"} or {"j", "i", "j"}.
* If fixedSeq = {"i"}, the result will be {"i", "j", "i"}.
*/
template <typename Type>
vector<Type> findSCS(const vector<vector<Type>>& sequences,
    const vector<Type>& fixedSeq = {}) {
    if (sequences.empty()) return {};

    vector<Type> scs = sequences[0];

    for (size_t i = 1; i < sequences.size(); ++i) {
        scs = mergeSequences(scs, sequences[i], fixedSeq);
    }

    // Add fixed sequence at the beginning
    scs.insert(scs.begin(), fixedSeq.begin(), fixedSeq.end());

    return scs;
}

// The StateNode struct remains the same
template <typename Type>
struct StateNode {
    std::vector<size_t> indices;
    std::vector<Type> path;
};

// New function to handle the fixed prefix
template <typename Type>
std::set<std::vector<Type>> findAllSCSWithPrefix(
    const std::vector<std::vector<Type>>& sequences,
    const std::vector<Type>& fixedSeq) {
    
    std::set<std::vector<Type>> solutions;
    if (sequences.empty()) {
        solutions.insert(fixedSeq);
        return solutions;
    }

    std::vector<std::vector<Type>> filtered_sequences;
    for(const auto& seq : sequences) {
        if (!seq.empty()) {
            filtered_sequences.push_back(seq);
        }
    }
    if (filtered_sequences.empty()) {
        solutions.insert(fixedSeq);
        return solutions;
    }

    int n = filtered_sequences.size();
    
    // =================================================================
    // 1. COMPUTE THE NEW INITIAL STATE based on fixedSeq
    // =================================================================
    std::vector<size_t> start_indices(n, 0);
    for (int i = 0; i < n; ++i) {
        size_t seq_ptr = 0; // Pointer for the sequence S_i
        // Find how many elements of S_i are covered by fixedSeq
        for (size_t prefix_ptr = 0; prefix_ptr < fixedSeq.size() && seq_ptr < filtered_sequences[i].size(); ++prefix_ptr) {
            if (fixedSeq[prefix_ptr] == filtered_sequences[i][seq_ptr]) {
                seq_ptr++;
            }
        }
        start_indices[i] = seq_ptr;
    }

    // =================================================================
    // 2. INITIALIZE BFS from the new state
    // =================================================================
    std::queue<StateNode<Type>> q;
    std::map<std::vector<size_t>, size_t> distances;

    StateNode<Type> start;
    start.indices = start_indices;
    start.path = fixedSeq; // The path starts with the fixed sequence
    
    q.push(start);
    distances[start.indices] = fixedSeq.size();

    size_t min_len = std::numeric_limits<size_t>::max();

    // =================================================================
    // 3. RUN BFS (This part is identical to the original algorithm)
    // =================================================================
    while (!q.empty()) {
        StateNode<Type> current = q.front();
        q.pop();
        
        if (current.path.size() > min_len) {
            continue;
        }

        bool is_goal = true;
        for (int i = 0; i < n; ++i) {
            if (current.indices[i] < filtered_sequences[i].size()) {
                is_goal = false;
                break;
            }
        }

        if (is_goal) {
            if (min_len == std::numeric_limits<size_t>::max()) {
                min_len = current.path.size();
            }
            solutions.insert(current.path);
            continue;
        }

        std::set<Type> next_chars;
        for (int i = 0; i < n; ++i) {
            if (current.indices[i] < filtered_sequences[i].size()) {
                next_chars.insert(filtered_sequences[i][current.indices[i]]);
            }
        }

        for (const Type& ch : next_chars) {
            StateNode<Type> next_node;
            next_node.path = current.path;
            next_node.path.push_back(ch);
            next_node.indices = current.indices;

            for (int i = 0; i < n; ++i) {
                if (next_node.indices[i] < filtered_sequences[i].size() && 
                    filtered_sequences[i][next_node.indices[i]] == ch) {
                    next_node.indices[i]++;
                }
            }

            auto it = distances.find(next_node.indices);
            if (it == distances.end() || next_node.path.size() <= it->second) {
                distances[next_node.indices] = next_node.path.size();
                q.push(next_node);
            }
        }
    }

    return solutions;
}

template <typename Type>
set<vector<Type>> getAllSCS(vector<vector<Type>>& sequences,
    const vector<Type>& fixedSeq = {}) {
    set<vector<Type>> allSCS;

    // shuffle the sequences in all possible ways and print all possible SCS
    std::sort(sequences.begin(), sequences.end());

    // get shortest common sequences for all permutations
    do {
        vector<Type> scs = findSCS(sequences, fixedSeq);
        // printSequence(scs);
        allSCS.insert(scs);
    } while (std::next_permutation(sequences.begin(), sequences.end()));
    return allSCS;
}

bool isSparse(const TensorVar& access) {
    Format format = access.getFormat();
    for (auto &modeFormat : format.getModeFormats()) {
      if (modeFormat.getName() == "compressed" 
        || modeFormat.getName() == "Compressed") {
        return true;
      }
    }
    return false;
}

} // namespace util
} // namespace taco

#endif