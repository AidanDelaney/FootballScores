#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

/**
 * Demonstrates how to do evil with std::for_each and std::copy, which are
 * supposed to be "Non-modifying sequence operations"
 */
int main () {
  std::vector<int> vs {1, 2, 3, 4, 5};

  std::for_each(vs.begin(), vs.end(), [](int &x) {return ++x;});
  std::copy(vs.cbegin(), vs.cend(), std::ostream_iterator<int>(std::cout, " "));

  std::cout << std::endl;
  
  std::transform(vs.begin(), vs.end(), vs.begin(), [](int &x) {return ++x;});
  std::copy(vs.cbegin(), vs.cend(), std::ostream_iterator<int>(std::cout, " "));

  std::cout << std::endl;

  std::count_if(vs.begin(), vs.end(), [](int &x){++x; return true;});
  std::copy(vs.cbegin(), vs.cend(), std::ostream_iterator<int>(std::cout, " "));
}
