#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
  std::vector<int> vs = {1,2,3,4,5};
  std::vector<int> us = {301, 302, 303};

  // "all iterators and references before the point of insertion are unaffected,
  //  unless the new container size is greater than the previous capacity"
  /*for(auto it = std::lower_bound(vs.begin(), vs.end(), 4)
	; it < vs.end() ; ++it) {
    for(auto ut = us.begin()
	  ; ut < us.end(); ++ut) {
      vs.insert(it, *ut);
    }
  }*/

  vs.insert(std::lower_bound(vs.begin(), vs.end(), four), us.cbegin(), us.cend());
  std::copy(vs.cbegin(), vs.cend(), std::ostream_iterator<int>(std::cout, " "));
}
