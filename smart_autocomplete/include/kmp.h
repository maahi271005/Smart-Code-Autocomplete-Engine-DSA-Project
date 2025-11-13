#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

using namespace std;

class KMP {
private:
    static vector<int> computeLPS(const string& pattern);
public:
    static bool contains(const string& text, const string& pattern);
    static vector<int> findAll(const string& text, const string& pattern);
};

#endif
