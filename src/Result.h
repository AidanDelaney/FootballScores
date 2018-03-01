#pragma once // not standard C++, but supported by almost all compilers
// C23 -- Make header files self-sufficient.
// C24 -- Always write internal #include guards. Never write external #include guards.

#include <iomanip>
#include <iostream>

struct GameResult {
  std::string home_team;
  std::string away_team;
  int home_score;
  int away_score;

  /*  GameResult(std::string &home_team, std::string &away_team, int home_score, int away_score)
  : home_team(home_team), away_team(away_team), home_score(home_score), away_score(away_score) {
  } */
  
  ~GameResult() {
#ifdef PRINTY
    std::cout << "Deleting " << home_team << " -v- " << away_team << std::endl;
#endif
  }
};

std::ostream& operator<<(std::ostream &o, const GameResult &gr) {
  // Magic number 12 is the longest team name in the dataset
  o << "home: " << std::setw(12) << gr.home_team << std::setw(0) << "\t" << gr.home_score << "-" << gr.away_score << std::setw(12) << "\taway: " << gr.away_team;
  return o;
}
