#include "test.h"
#include "taco/util/algo.h"
#include "taco/index_notation/index_notation.h"

#include <iostream>

using namespace std;
using namespace taco::util;
using namespace taco;

TEST(util, looporder1_1) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<taco::IndexVar>> sequences = {
      {i, k},
      {k, j},
  };
  vector<taco::IndexVar> fixedSeq = {i, j};

  vector<taco::IndexVar> scs = findSCS(sequences, fixedSeq);
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

  vector<IndexVar> scs = findSCS(sequences, fixedSeq);
  // taco::util::printSequence(scs);

  vector<IndexVar> result = {i, j, k, j};
  ASSERT_EQ((int) scs.size(), 4);
  ASSERT_EQ(scs, result);
}

TEST(util, looporder2) {
  taco::IndexVar i("i"), j("j"), k("k");
  vector<vector<std::string>> sequences = {
    {"i", "j"},
    {"j", "i"},
  };
  vector<std::string> fixedSeq = {"i", "j"};

  vector<std::string> scs = findSCS(sequences, fixedSeq);
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
