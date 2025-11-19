#ifndef TACO_UTIL_ALGO_H
#define TACO_UTIL_ALGO_H

namespace taco {
namespace util {

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <set>

#include "taco/index_notation/index_notation.h"

using namespace std;

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