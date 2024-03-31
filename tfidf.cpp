#include "tfidf.h"
#include <sstream>
#include <iomanip> 

TFIDF::TFIDF(double idf_val) : idf_val(idf_val) {}

std::string TFIDF::vectorize(const std::string sentence) const {
    std::unordered_map<std::string, int> term_freq;
    std::istringstream iss(sentence);
    std::string word;

    while (iss >> word) {
        term_freq[word]++;
    }

    std::ostringstream vector_stream;   
    for (const auto& pair: term_freq) {
        double tfidf = pair.second * idf_val;
        vector_stream << std::setprecision(2) << tfidf << " ";
    }

    return vector_stream.str();
}