#include <functional>
#include <iostream>

int add(int x, int y) {
  return x+y;
}

struct IsEqual : std::binary_function<int, int, bool> {
  bool operator()(const int x, const int y) const {
    return x == y;
  }
};

struct IsEq : std::binary_function<const std::string&, const std::string&, bool> {
  bool operator()(const std::string& x, const std::string& y) const {
    return x==y;
  }
};

int main() {
  using namespace std::placeholders;
    
  auto add_5 = std::bind(&add, 5, _1);
  auto equal_5  = std::bind(IsEqual(), 5, _1);

  std::string five("5");
  auto eq_5  = std::bind(IsEq(), five, _1);

  std::cout << add_5(1) << " " << equal_5(6) << " " << eq_5(five) << std::endl;
}
