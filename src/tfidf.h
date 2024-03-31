#ifndef TFIDF_H
#define TFIDF_H

#include <string>
#include <vector>
#include <unordered_map>

class TFIDF {
public:
    explicit TFIDF(double idf_val = 1.0);
    std::string vectorize(const std::string sentence) const;

private:
    double idf_val;
};

#endif // TFIDF_H