#include "test.h"
#include "test_tensors.h"
#include "taco/tensor.h"
#include "taco/index_notation/index_notation.h"
#include "taco/index_notation/index_notation_nodes.h"
#include "codegen/codegen.h"
#include "taco/lower/lower.h"
// #include "taco/util/algo.h"

#include <iostream>

using namespace taco;

TEST(transposefused, dummy) {
    std::cout << "dummy" << std::endl;

    IndexVar i("i"), j("j"), k("k");
    TensorVar a("a", Type(type<double>(), {3, 3}), {Dense,Sparse}), 
        b("b", Type(type<double>(), {3, 3}), {Dense,Sparse}), 
        c("c", Type(type<double>(), {3, 3}), {Dense,Sparse});

    IndexExpr expr = b(i, j);
    ASSERT_TRUE(isa<Access>(expr));
    ASSERT_TRUE(isa<AccessNode>(expr.ptr));
    Access access = to<Access>(expr);

    std::cout << expr << std::endl;

    IndexExpr expr2 = b(i,j) * c(j,i);
    ASSERT_TRUE(isa<Mul>(expr2));
    ASSERT_TRUE(isa<MulNode>(expr2.ptr));
    Mul mul = to<Mul>(expr2);

    std::cout << expr2 << std::endl;

    IndexStmt stmt = a(i, j) = b(i, j) * c(j, i);
    ASSERT_TRUE(isa<Assignment>(stmt));
    ASSERT_TRUE(isa<AssignmentNode>(stmt.ptr));
    Assignment assignment = to<Assignment>(stmt);

    std::cout << stmt << std::endl;

    IndexStmt stmt2 = stmt.concretize();
    // IndexStmt stmt2 = a.getAssignment().concretize();
    // IndexStmt stmt2 = forall(i, forall(j, a(i, j) = b(i, j) * c(i, j)));
    // IndexStmt stmt2 = forall(i, forall(j, forall(i, a(i, j) = b(i, j) * c(j, i))));
    ASSERT_TRUE(isa<Forall>(stmt2));
    ASSERT_TRUE(isa<ForallNode>(stmt2.ptr));
    Forall forallstmt = to<Forall>(stmt2);

    // compile(stmt2);

    std::cout << stmt2 << std::endl;
}


TEST(transposefused, dummy2) {
    int N = 16;

    taco::followMode = taco::FollowMode::PointerTrack;

    Tensor<double> A("A", {N, N}, Format{Dense, Sparse});
    Tensor<double> B("B", {N, N}, Format{Dense, Sparse});
    Tensor<double> C("C", {N, N}, Format{Dense, Sparse});
  
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        B.insert({i, j}, (double) i);
        C.insert({i, j}, (double) j);
      }
    }

    IndexVar i("i"), j("j");

    A(i, j) = B(i, j) * C(j, i);

    IndexStmt stmt = A.getAssignment().concretize(true); // newPath = true/false
    // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

    std::cout << "===============================================" << std::endl;
    std::cout << "\n\n\n\n\n\n\n\n\n" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "here2: " <<  stmt << std::endl;

    A.setAssembleWhileCompute(true);
    A.setNewPath(true);
    std::cout << "compiling statement: " << stmt << std::endl;

    ir::IRPrinter irp = ir::IRPrinter(cout);
      
    cout << stmt << endl;

    std::shared_ptr<ir::CodeGen> codegen = ir::CodeGen::init_default(cout, ir::CodeGen::ImplementationGen);
    ir::Stmt compute = lower(stmt, "compute",  true, true);
    
    irp.print(compute);
    cout << endl;
//     codegen->compile(compute, false);

    A.compile(stmt, true);
//     // A.assemble();
    A.compute();

    std::cout << "done" << std::endl;
}


TEST(transposefused, denseout1) {
  int N = 4;
  Tensor<double> A("A", {N}, Format{Dense});
  Tensor<double> B("B", {N, N}, Format{Dense, Sparse});
  Tensor<double> C("C", {N, N}, Format{Dense, Sparse});

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      B.insert({i, j}, (double) i);
      C.insert({i, j}, (double) j);
    }
  }

  IndexVar i("i"), j("j");

  A(i) = B(i, j) * C(j, i);

  IndexStmt stmt = A.getAssignment().concretize(true);
  // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

  A.setAssembleWhileCompute(true);

  std::cout << "===============================================" << std::endl;
  std::cout << "\n\n\n\n\n\n\n\n\n" << std::endl;
  std::cout << "================================================" << std::endl;

  A.setNewPath(true);
  std::cout << "here: " <<  stmt << std::endl;

  ir::IRPrinter irp = ir::IRPrinter(cout);
  std::shared_ptr<ir::CodeGen> codegen = ir::CodeGen::init_default(cout, ir::CodeGen::ImplementationGen);
  ir::Stmt compute = lower(stmt, "compute",  true, true);
  
  irp.print(compute);
  cout << endl;

  A.compile(stmt, true);

  std::cout << "===============================================" << std::endl;
  std::cout << "\n\n\n\n\n\n\n\n\n" << std::endl;
  std::cout << "================================================" << std::endl;

  // A.assemble();
  // A.compute();
}

TEST(transposefused, denseout2) {
  int N = 4;
  Tensor<double> A("A", {N}, Format{Dense});
  Tensor<double> B("B", {N, N}, Format{Dense, Sparse});
  Tensor<double> C("C", {N, N}, Format{Dense, Sparse});

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      B.insert({i, j}, (double) i);
      C.insert({i, j}, (double) j);
    }
  }

  IndexVar i("i"), j("j");

  A(i) = B(i, j) * C(i,j);

  IndexStmt stmt = A.getAssignment().concretize(false);
  // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

  std::cout << "===============================================" << std::endl;
  std::cout << "\n\n\n\n\n\n\n\n\n" << std::endl;
  std::cout << "================================================" << std::endl;

  A.setAssembleWhileCompute(true);
  // A.setNewPath(true);
  std::cout << "here: " <<  stmt << std::endl;

  A.compile(stmt, true);

  std::cout << "===============================================" << std::endl;
  std::cout << "\n\n\n\n\n\n\n\n\n" << std::endl;
  std::cout << "================================================" << std::endl;

  // A.assemble();
  // A.compute();
}


TEST(transposefused, sspmm) {
  int N = 16;
  Tensor<double> A("A", {N, N}, Format{Dense, Sparse});
  Tensor<double> B("B", {N, N}, Format{Dense, Sparse});
  Tensor<double> C("C", {N, N}, Format{Dense, Sparse});
  Tensor<double> D("D", {N, N}, Format{Dense, Sparse});

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      B.insert({i, j}, (double) i);
      C.insert({i, j}, (double) j);
      D.insert({i, j}, (double) (i + j));
    }
  }

  IndexVar i("i"), j("j"), k("k");

  A(i, j) = B(i, k) * C(k, j) * D(k, j);

  IndexStmt stmt = A.getAssignment().concretize(true);
  // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

  std::cout << "here: " <<  stmt << std::endl;

  A.setNewPath(true);
  A.compile(stmt, false); // assembleWhileCompute = true
  A.assemble();
  A.compute();
}

TEST(transposefused, 3dfuse) {
  int N = 6;

  taco::followMode = taco::FollowMode::Pointer;


  Tensor<double> A("A", {N, N, N}, Format{Dense, Sparse, Sparse});
  Tensor<double> B("B", {N, N, N}, Format{Dense, Sparse, Sparse});
  Tensor<double> C("C", {N, N, N}, Format{Dense, Sparse, Sparse});

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < N; k++) {
        // create random number between 0 and 1
        float val = (float) rand() / RAND_MAX;
        B.insert({i, j, k}, (double) val);
        C.insert({i, k, j}, (double) val);
      }
    }
  }
  B.pack();
  C.pack();

  IndexVar i("i"), j("j"), k("k");

  A(i, j, k) = B(i, j, k) * C(i, k, j);

  IndexStmt stmt = A.getAssignment().concretize(true);
  // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

  std::cout << "here: " <<  stmt << std::endl;

  ir::IRPrinter irp = ir::IRPrinter(cout);
  std::shared_ptr<ir::CodeGen> codegen = ir::CodeGen::init_default(cout, ir::CodeGen::ImplementationGen);
  ir::Stmt compute = lower(stmt, "compute",  true, true);
  
  irp.print(compute);
  cout << endl;

  A.compile(stmt, true);
  A.assemble();
  A.compute();
}

TEST(transposefused, 3dfuse2) {
  int N = 6;
  Tensor<double> A("A", {N, N, N}, Format{Dense, Sparse, Sparse});
  Tensor<double> B("B", {N, N, N}, Format{Dense, Sparse, Sparse});
  Tensor<double> C("C", {N, N, N}, Format{Dense, Sparse, Sparse});

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < N; k++) {
        // create random number between 0 and 1
        float val = (float) rand() / RAND_MAX;
        B.insert({i, j, k}, (double) val);
        C.insert({i, k, j}, (double) val);
      }
    }
  }
  B.pack();
  C.pack();

  IndexVar i("i"), j("j"), k("k");

  A(i, j, k) = B(i, j, k) * C(i, j, k);

  IndexStmt stmt = A.getAssignment().concretize(true);
  // IndexStmt stmt = forall(i, forall(j, forall(i, A(i, j) = B(i, j) * C(j, i))));

  std::cout << "here: " <<  stmt << std::endl;

  A.compile(stmt, true);
  // A.assemble();
  // A.compute();
}

