/**
 * Calculates summary statistics for 2016-2017 season English championshp
 * soccer results.
 * 
 * Much of the code is annotated with EXY, SXY or CXY.  These refer to the
 * best practice guides found in
 *
 * E: Scott Meyers, _Effective Modern C++_, O'Reilly, 2014.
 * S: Scott Meyers, _Effective STL: 50 Specific Ways to Improve Your Use of the Standard Template Library_, Addison-Wesley, 2001.
 * C: Herb Sutter & Andrei Alexandrescu, _C++ Coding Standards: 101 Rules, Guidelines, and Best Practices_, Pearson, 2005.
 *
 * As such, E48 refers to the 48th guideline in _Effective Modern C++_.
 */
// E48 -- Always #include the proper headers.
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "Result.h"
#include "Timing.h"

/*
 * A quick and dirty mechanism for reading our input CSV file.
 * This function does no error handlind and does not follow best practice.
 */
void readCSV(std::vector<GameResult> &results) {
  std::ifstream infile;
  infile.open("data/en-championship-2016-2017.csv");

  if(!infile.is_open()) {
    std::cerr << "Cannot open data file" << std::endl;
    return;
  }

  while(!infile.eof()) {
    GameResult result;
    //    infile >> result->home_team >> result->away_team >> result->home_score >> result->away_score;
    infile >> result.home_team >> result.away_team >> result.home_score >> result.away_score;;
    // C80 -- Prefer push_back to other ways of expanding a sequence.
    results.push_back(result);
  }

  infile.close();
}

/**
 *
 * Follows:
 *   - C15: Use const proactively.
 *   - C25: Take parameters appropriately by value, (smart) pointer, or reference.
 *   - C20: Avoid long functions. Avoid deep nesting.
 *   - S39/C87: Make predicates pure functions.
 */
bool home_win(const GameResult &gr) {
  return gr.home_score > gr.away_score;
}

/**
 * 
 * Follows:
 *   - C15: Use const proactively.
 *   - C25: Take parameters appropriately by value, (smart) pointer, or reference.
 *   - C20: Avoid long functions. Avoid deep nesting.
 *   - S39/C87: Make predicates pure functions.
 *   - C88: Prefer function objects over functions as algorithm and comparer arguments.
 *   - C89: Write function objects correctly.
 */
struct IsHomeWin : std::unary_function<const GameResult&, bool> {
  bool operator()(const GameResult& gr) const noexcept {
    return gr.home_score > gr.away_score;
  }
};

struct IsNamedTeam : std::binary_function<const std::string&, const GameResult&, bool> {
  bool operator()(const std::string& team_name, const GameResult& gr) const noexcept {
    return (team_name == gr.home_team) || (team_name == gr.away_team);
  }
};

/**
 * Use std::algorithm to find out how many games were won by the home team.
 *
 * Follows:
 *   - C15 -- Use const proactively.
 *   - C20 -- Avoid long functions. Avoid deep nesting.
 *   - C25 -- Take parameters appropriately by value, (smart) pointer, or reference.
 *   - E
 *   - S43/C84 -- Prefer algorithm calls to hand-written loops.
 *   - S44 -- Prefer member functions to algorithms with the same names
 *   - S45/C85 -- Distinguish among count, find, binary search, lower_bound, upper_bound, and equal_range.
 *   - S46 -- Consider function objects instead of functions as algorithm parameters.
 *
 * Ignores:
 *   -
 */
int home_games_won_alg(const std::vector<GameResult> &results) {
  int home_won = std::count_if(results.cbegin(), results.cend(), IsHomeWin());
  
  /*
   * The lambda version is slightly slower than passing in the named function.
   *  
    int home_won = std::count_if(std::cbegin(results), std::cend(results), [](const GameResult &gr){
      return gr.home_score > gr.away_score;
      });
   */
  return home_won;
}

/**
 * Use C++11 ranged loop to find out how many games were won by the home team.
 *
 * Follows:
 *   - C15 -- Use const proactively.
 *   - C25 -- Take parameters appropriately by value, (smart) pointer, or reference.
 *   - E
 *   - S
 * Ignores:
 *   -
 */
inline int home_games_won(const std::vector<GameResult> &results) {
  int home_won = 0;
  for(const auto &gr: results) {
    if(gr.home_score > gr.away_score) {
      ++home_won;
    }
  }
  return home_won;
}

/**
 * Uses "traditional" loop to find out how many games were won by the home team.
 *
 * Follows:
 *   - C15 -- Use const proactively.
 *   - C25 -- Take parameters appropriately by value, (smart) pointer, or reference.
 *   - C28 -- Prefer the canonical form of + + and --. Prefer calling the prefix forms.
 *   - E
 *   - S
 * Ignores:
 *   -
 */
int home_games_won_03(const std::vector<GameResult> &results) {
  int home_won = 0;
  for(std::vector<GameResult>::const_iterator it = results.cbegin(); it < results.cend(); ++it) {
    if(it->home_score > it->away_score) {
      ++home_won;
    }
  }
  return home_won;
}

/// In the following test function we don't do anything fancy.
//  * We don't pass in the function to exectute (only the required parameter)
//  * We don't use the result in any way
//  * The driver is a simple loop

const int TEST_RUNS = 100000;

void test_alg(const std::vector<GameResult> &results) {
  for(int i = 0; i<TEST_RUNS; ++i) home_games_won_alg(results);
}

void test_loop(const std::vector<GameResult> &results) {
  for(int i = 0; i<TEST_RUNS; ++i) home_games_won(results);
}

void test_03(const std::vector<GameResult> &results) {
  for(int i = 0; i<TEST_RUNS; ++i) home_games_won_03(results);
}

void print_brighton_stats(std::vector<GameResult> & results) {
  using namespace std::placeholders;

  std::string brighton("Brighton");
  auto brighton_pred = std::bind(IsNamedTeam(), brighton, _1);
  auto home_pred = std::bind(IsHomeWin(), _1);
  auto brighton_home_win_pred = std::bind(std::logical_and<bool>(), brighton_pred, home_pred);

  int brighton_played = std::count_if(results.cbegin(), results.cend(), brighton_pred);
  int brighton_home_win = std::count_if(results.cbegin(), results.cend(), brighton_home_win_pred);

  std::cout << brighton << " played " << brighton_played << std::endl;
  std::cout << brighton_home_win << " home games were won by " << brighton << std::endl;

  /*
   * S32 -- Follow remove-like algorithms by erase if you really want to remove something.
   * C82 -- Use the accepted idioms to really shrink capacity and really erase elements.
   */
  std::cout << results.size() << " ";
  results.erase(std::remove_if(std::begin(results), std::end(results), brighton_pred), std::end(results));
  std::cout << results.size() << " " << std::endl;;
}

int main() {
  const int total_games(552); // Known from fixtures list.

  // C79 -- Store only values and smart pointers in containers.
  // 
  std::vector<GameResult> results;
  results.reserve(total_games);

  readCSV(results);

  int home_games_won = std::count_if(results.cbegin(), results.cend(), IsHomeWin());
  std::cout << "Of " << results.size() << " played " << home_games_won << " were won by home teams" << std::endl;
  std::cout << "We will now calculate that figure " << TEST_RUNS << " times, using 3 mechanisms." << std::endl;

  std::cout << std::endl;
  
  // Run "performance indicators".
  auto t_03 = measure<std::chrono::nanoseconds>::execution(test_03, results);
  auto t_alg = measure<std::chrono::nanoseconds>::execution(test_alg, results);
  auto t_loop = measure<std::chrono::nanoseconds>::execution(test_loop, results);
  
  std::cout << "\tC++03: " << t_03 << "ns" << std::endl;
  std::cout << "\tC++11 loop style: " << t_loop << "ns" << std::endl;
  std::cout << "\tSTD algorithm: " << t_alg << "ns" << std::endl;

  std::cout << std::endl;
  
  print_brighton_stats(results);
  
  return 0;
}
