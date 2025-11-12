#ifndef RANKER_H
#define RANKER_H

#include <string>
#include <vector>
#include <utility>
#include "freq_store.h"
#include "graph.h"

class Ranker {
  private:
      FreqStore* freqStore;
      CooccurrenceGraph* graph;
      std::string lastToken;
  
  public:
      Ranker(FreqStore *fs,CooccurrenceGraph *g);
      void setLastToken(const std::string &token);
      double computeScore(const std::string &token);
      std::vector<std::pair<std::string,double>> rankResults(const std::vector<std::string> &candidates,int k);
};

#endif

