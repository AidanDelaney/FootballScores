#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#define PRINTY

#include "Result.h"

void readCSV(std::vector<GameResult*> &results) {
  std::ifstream infile;
  infile.open("data/en-championship-2016-2017.csv");

  if(!infile.is_open()) {
    std::cerr << "Cannot open data file" << std::endl;
    return;
  }

  while(!infile.eof()) {
    GameResult *result = new GameResult();
    std::string ht, at;
    //    infile >> result->home_team >> result->away_team >> result->home_score >> result->away_score;
    infile >> ht >> at >> result->home_score >> result->away_score;
    result->home_team.assign(ht);
    result->away_team.assign(at);
    results.push_back(result);
  }

  infile.close();
}

/*
 * E39 - Make predicates pure functions.
 */
bool isBrighton(GameResult * gr) {
  const std::string bton("Brighton");
  return (bton == gr->home_team) || (bton == gr->away_team);
}

int main() {
  const int total_games(552); // Known from fixtures list.

  // BREAKS C79 -- Store only values and smart pointers in containers.
  // Which explains why S32 & C82 later don't help us.
  std::vector<GameResult *> results;
  results.reserve(total_games);

  readCSV(results);

  //std::for_each(std::begin(results), std::end(results), [](GameResult *gr) {delete gr;});
  /*
   * S32 -- Follow remove-like algorithms by erase if you really want to remove something.
   * C82 -- Use the accepted idioms to really shrink capacity and really erase elements.
   */
  results.erase(std::remove_if(std::begin(results), std::end(results), isBrighton), std::end(results));

  std::cout<< "Erased all GameResult*" << std::endl;
  

}
