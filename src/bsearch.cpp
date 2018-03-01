#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Timing.h"

const int TEST_RUNS = 1000;
const int MAX_VALUE = 1000000;
const int EG_TO_FIND = 75000;

std::pair<int, int> mk_pair(int x) {
  return std::make_pair(x, x);
}

/*
 * - S43 -- Prefer algorithm calls to hand-written loops.
 *
 * The use of the wrapping for loop is to provide the same
 * implementation in all three test functions.
 */
void test_intv(std::vector<std::pair<int, int>> &items) {
  for(int i = 0; i<TEST_RUNS; ++i) {
    const std::pair<int, int> eg = mk_pair(EG_TO_FIND);
    std::binary_search(items.cbegin(), items.cend(), eg);
  }
}

void test_mapv(std::map<int, int> &items) {
  for(int i = 0; i<TEST_RUNS; ++i) {
    items[EG_TO_FIND];
  }
}

void test_u_mapv(std::unordered_map<int, int> &items) {
  for(int i = 0; i<TEST_RUNS; ++i) {
    items[EG_TO_FIND];
  }
}

int main() {
  std::vector<int> intv(MAX_VALUE);
  std::iota(intv.begin(), intv.end(), 0);

  /*
   * S23 - Consider replacing associative containers with sorted vectors.
   */
  std::vector<std::pair<int, int>> zipped_intv;
  zipped_intv.reserve(std::size(intv));

  std::transform(std::cbegin(intv), std::cend(intv), std::back_inserter(zipped_intv)
		 , mk_pair
		 );

  std::map<int,int> mapv(zipped_intv.cbegin(), zipped_intv.cend());
  std::unordered_map<int, int> u_mapv(zipped_intv.cbegin(), zipped_intv.cend());


  auto t_intv = measure<std::chrono::nanoseconds>::execution(test_intv, zipped_intv);
  auto t_mapv = measure<std::chrono::nanoseconds>::execution(test_mapv, mapv);
  auto t_u_mapv = measure<std::chrono::nanoseconds>::execution(test_u_mapv, u_mapv);

  std::cout << "vector<pair<int>>: " << (t_intv/TEST_RUNS)
	    << "\tmap<int, int>: " << (t_mapv/TEST_RUNS)
    	    << "\tunordered_map<int>: " << (t_u_mapv/TEST_RUNS)
	    << "ns" << std::endl;
  
  return 0;
}
