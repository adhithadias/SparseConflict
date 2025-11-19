#include "test.h"
#include "taco/util/algo.h"
#include "taco/index_notation/index_notation.h"

#include <iostream>

using namespace std;
using namespace taco::util;
using namespace taco;

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <limits>

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
        
        if (current.path.size() >= min_len) {
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

// Helper for printing
template <typename Type>
void printSolutions(const std::string& title, const std::set<std::vector<Type>>& solutions) {
    std::cout << title << std::endl;
    if (solutions.empty()) {
        std::cout << "  No solutions found." << std::endl;
        return;
    }
    std::cout << "  Found " << solutions.size() << " shortest solution(s) of length " 
              << solutions.begin()->size() << ":" << std::endl;
    for (const auto& scs : solutions) {
        std::cout << "    { ";
        for (const auto& item : scs) {
            std::cout << item << " ";
        }
        std::cout << "}" << std::endl;
    }
}


// int main() {
//     std::vector<std::vector<char>> sequences = {{'A', 'B', 'C'}, {'B', 'X', 'A'}};

//     // Case 1: No prefix (for comparison)
//     auto solutions1 = findAllSCSWithPrefix<char>(sequences, {});
//     printSolutions("Case 1: No fixed prefix", solutions1);
//     // Expected: {B, X, A, B, C} and others of length 5

//     std::cout << "\n-----------------\n" << std::endl;

//     // Case 2: With a fixed prefix
//     std::vector<char> prefix = {'B', 'X'};
//     auto solutions2 = findAllSCSWithPrefix(sequences, prefix);
//     printSolutions("Case 2: Fixed prefix {'B', 'X'}", solutions2);
//     // Explanation: 'B', 'X' satisfies the start of {'B', 'X', 'A'}.
//     // It also satisfies the 'B' in {'A', 'B', 'C'}.
//     // The search starts looking for 'A' (from 2nd seq) and 'A', 'C' (from 1st seq).
//     // Expected: {B, X, A, B, C} of length 5

//     std::cout << "\n-----------------\n" << std::endl;

//     // Case 3: A prefix that changes the optimal completion
//     std::vector<char> prefix2 = {'A'};
//     auto solutions3 = findAllSCSWithPrefix(sequences, prefix2);
//     printSolutions("Case 3: Fixed prefix {'A'}", solutions3);
//     // Explanation: 'A' satisfies the start of {'A', 'B', 'C'}.
//     // The search starts looking for 'B', 'C' (from 1st seq) and 'B', 'X', 'A' (from 2nd seq).
//     // Expected: {A, B, X, A, C} of length 5
    
//     return 0;
// }

TEST(util, looporder1_1) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<taco::IndexVar>> sequences = {
      {i, k},
      {k, j},
  };
  vector<taco::IndexVar> fixedSeq = {i, j};

  auto seqs = findAllSCSWithPrefix(sequences, fixedSeq);
  ASSERT_EQ((int) seqs.size(), 1);
  vector<taco::IndexVar> scs = *seqs.begin();
  taco::util::printSequence(scs);

  vector<taco::IndexVar> result = {i, j, k, j};
  ASSERT_EQ((int) scs.size(), 4);
  ASSERT_EQ(scs, result);
}

TEST(util, looporder1_2) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<IndexVar>> sequences = {
      {i, k},
      {k, j},
      {k, j}
  };
  vector<IndexVar> fixedSeq = {i, j};

  auto seqs = findAllSCSWithPrefix(sequences, fixedSeq);
  ASSERT_EQ((int) seqs.size(), 1);
  vector<IndexVar> scs = *seqs.begin();
  // vector<IndexVar> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

  vector<IndexVar> result = {i, j, k, j};
  ASSERT_EQ((int) scs.size(), 4);
  ASSERT_EQ(scs, result);
}

TEST(util, looporder2) {
  taco::IndexVar i("i"), j("j");
  vector<vector<std::string>> sequences = {
    {"i", "j"},
    {"j", "i"},
  };
  vector<std::string> fixedSeq = {"i", "j"};

  auto seqs = findAllSCSWithPrefix(sequences, fixedSeq);
  ASSERT_EQ((int) seqs.size(), 1);
  vector<std::string> scs = *seqs.begin();
  // vector<std::string> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

  vector<string> result = {"i", "j", "i"};
  ASSERT_EQ((int) scs.size(), 3);
  ASSERT_EQ(scs, result);
}

TEST(util, looporder3) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<std::string>> sequences = {
    {"i", "j"},
    {"j", "i"},
  };
  vector<std::string> fixedSeq = {};

  vector<std::string> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

  vector<string> result = {"i", "j", "i"};
  ASSERT_EQ((int) scs.size(), 3);
  ASSERT_EQ(scs, result);
}

TEST(util, looporder4) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<std::string>> sequences = {
    {"j", "i"},
    {"i", "j"},
  };
  vector<std::string> fixedSeq = {};

  vector<std::string> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

  vector<string> result = {"j", "i", "j"};
  ASSERT_EQ(scs.size(), (std::size_t) 3);
  ASSERT_EQ(scs, result);

  set<vector<string>> allScs = getAllSCS(sequences, fixedSeq);
  ASSERT_EQ((int) allScs.size(), 2);
  vector<string> result1 = {"i", "j", "i"};
  vector<string> result2 = {"j", "i", "j"};

  // check if result 1 and result 2 are in allScs
  ASSERT_NE(std::find(allScs.begin(), allScs.end(), result1), allScs.end());
  ASSERT_NE(std::find(allScs.begin(), allScs.end(), result2), allScs.end());
}

TEST(util, lattice1) {
  taco::IndexVar i("i"), j("j"), k("k");

  int N = 16;
  Tensor<double> A("A", {N, N}, Format{Dense, Sparse});
  Tensor<double> B("B", {N, N}, Format{Dense, Sparse});
  Tensor<double> C("C", {N, N}, Format{Dense, Sparse});

  A(i, j) = B(i, j) * C(j, i);

  IndexStmt stmt = A.getAssignment();

  ASSERT_TRUE(isa<Assignment>(stmt));

  Assignment assignment = to<Assignment>(stmt);
  // AssignmentNode* assignmentNode = assignment.ptr;
  // taco_iassert(assignmentNode != nullptr);

  Access lhs = assignment.getLhs();
  taco_iassert(isa<Access>(lhs));

  vector<IndexVar> freeVars = lhs.getIndexVars();

  std::cout << stmt << std::endl;

  vector<vector<taco::IndexVar>> sequences = {
      {i, j},
      {j, i},
  };
  vector<taco::IndexVar> fixedSeq = {i, j};

  vector<taco::IndexVar> scs = findSCS(sequences, fixedSeq);
  taco::util::printSequence(scs);

  vector<taco::IndexVar> result = {i, j, i};
  ASSERT_EQ((int) scs.size(), 3);
  ASSERT_EQ(scs, result);

  
}
