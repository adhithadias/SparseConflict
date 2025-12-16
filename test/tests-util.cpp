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

//   vector<std::string> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

//   vector<string> result = {"j", "i", "j"};
//   ASSERT_EQ(scs.size(), (std::size_t) 3);
//   ASSERT_EQ(scs, result);

//   set<vector<string>> allScs = getAllSCS(sequences, fixedSeq);
  set<vector<string>> allScs = findAllSCSWithPrefix(sequences, fixedSeq);
  printSequences(allScs);

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

// New test: ensure multiple shortest common supersequences are discovered
TEST(util, multipleSCS_permutations) {
  // sequences {"i","j"} and {"j","i"} should yield two shortest SCS:
  // {"i","j","i"} and {"j","i","j"}
  vector<vector<string>> sequences = {
    {"i", "j"},
    {"j", "i"}
  };

  // getAllSCS takes a non-const vector by reference and considers permutations
  auto allScs = getAllSCS(sequences, {});

  // Expect exactly two shortest sequences
  ASSERT_EQ((int)allScs.size(), 2);

  vector<string> expected1 = {"i", "j", "i"};
  vector<string> expected2 = {"j", "i", "j"};

  ASSERT_NE(allScs.find(expected1), allScs.end());
  ASSERT_NE(allScs.find(expected2), allScs.end());
}

TEST(util, scs_chars_no_prefix) {
  vector<vector<char>> sequences = {{'A','B','C'}, {'B','X','A'}};

  auto allScs = findAllSCSWithPrefix<char>(sequences, vector<char>{});

  // Expect at least one shortest solution of length 5 and that it contains
  // the expected sequence {'B','X','A','B','C'}.
  bool foundExpected = false;
  for (const auto &scs : allScs) {
    if (scs.size() == 5 && scs == vector<char>{'B','X','A','B','C'}) {
      foundExpected = true;
      break;
    }
  }
  ASSERT_TRUE(foundExpected);
}

TEST(util, scs_chars_with_prefix_BX) {
  vector<vector<char>> sequences = {{'A','B','C'}, {'B','X','A'}};
  vector<char> prefix = {'B','X'};

  auto allScs = findAllSCSWithPrefix<char>(sequences, prefix);

  // With prefix {B,X} we expect at least the completion {B,X,A,B,C}
  vector<char> expected = {'B','X','A','B','C'};
  ASSERT_NE(allScs.find(expected), allScs.end());
}

TEST(util, scs_chars_with_prefix_A) {
  vector<vector<char>> sequences = {{'A','B','C'}, {'B','X','A'}};
  vector<char> prefix = {'A'};

  auto allScs = findAllSCSWithPrefix<char>(sequences, prefix);

  // The prefix {'A'} should allow the completion {A,B,X,A,C}; ensure it's present
  vector<char> expected = {'A','B','X','A','C'};
  ASSERT_NE(allScs.find(expected), allScs.end());
}
