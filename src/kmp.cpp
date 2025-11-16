#include "../include/kmp.h"
using namespace std;

vector<int> KMP::computeLPS (const string& pattern){
    int n = pattern.length();
    vector<int> lps(n,0);
    
    int len = 0;
    int i=1;
    
    while(i<n){
        if(pattern[i] == pattern[len]){
            len++;
            lps[i] = len;
            i++;
        }
        else{
            if(len != 0) len = lps[len-1];
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

bool KMP::contains(const string& text, const string&pattern){
    if(pattern.empty()) return 1;
    if(text.empty()) return 0;
    
    int n = text.length();
    int m = pattern.length();
    
    int i = 0;
    int j = 0;
    vector<int> lps = computeLPS(pattern);
    
    while(i<n){
        if(pattern[j] == text[i]){
            i++;
            j++;
        }
        
        if(j==m) return 1;
        else if(i<n && pattern[j] != text[i]){
            if(j!=0) j = lps[j-1];
            else i++;
        }
    }
    return 0;
}

vector<int> KMP::findAll (const string& text, const string& pattern){
    vector<int>pos;
    
    int n = text.length();
    int m = pattern.length();
    
    vector<int> lps = computeLPS(pattern);
    
    int i=0; 
    int j=0;
    
    while(i<n){
        if(pattern[j] == text[i]){
            i++;
            j++;
        }
        
        if(j==m){
            pos.push_back(i-j);
            j = lps[j-1];
        }
        else if(i<n && pattern[j] != text[i]){
            if(j!=0) j = lps[j-1];
            else i++;
        }
    }
    return pos;
}
//test


