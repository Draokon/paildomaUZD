#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cctype>   // For std::isalnum
#include <regex>
#include <sstream>

using std::string;
using std::ifstream;   
using std::ofstream;
using std::map;
using std::set;
using std::vector;
using std::cout;
using std::endl;
using std::getline;
using std::cerr;
using std::regex;
using std::stringstream;


// pašalina skyrybos ženklus nuo žodžio pradžios ir pabaigos
string clean_word(const string& word) {
    size_t start = 0, end = word.size();
    while (start < end && !std::isalnum(word[start])) ++start;      // tikrina ar pirmas simbolis yr raide ar sk.
    while (end > start && !std::isalnum(word[end - 1])) --end;      // same thing, bet tikrina ar paskutinis simbolis yr raide ar sk.
    return word.substr(start, end - start);
}

int main() {
    ifstream in("textas.txt");
    if (!in) {
        cerr << "Nepavyko atidaryti textas.txt\n";
        return 1;
    }

    map<string, int> word_count;         // Žodžių daznumas
    map<string, set<int>> word_lines;    // zodziu eilutes
    regex url_regex(R"((https?:\/\/[^\s,]+)|(www\.[^\s,]+)|([a-zA-Z0-9\-.]+\.[a-zA-Z]{2,}(\/[^\s,]*)?))");   //Standartine israiska URL paieskai
    set<string> urls;

    string line;
    int line_number = 0;
    while (getline(in, line)) {
        ++line_number;

        // URL search
        std::sregex_iterator begin(line.begin(), line.end(), url_regex), end;
        for (auto i = begin; i != end; ++i) {
            string url = i->str();  // randa konkretu URL
            url = clean_word(url);
            urls.insert(url);
        }

        // Žodžių paieška
        stringstream ss(line);
        string word;
        while (ss >> word) {
            word = clean_word(word);
            if (word.empty()) continue;
            // kad palyginimas butu nepriklausiomas nuo raidziu dydzio
            for (auto& c : word) c = std::tolower(c);   //konvertuoja i mazasias raides
            word_count[word]++;
            word_lines[word].insert(line_number);
        }
    }
    in.close();

    // Išvedame žodžių dažnius
    ofstream out1("zodzio_pasikartojimas.txt");
    if (!out1) {
        cerr << "Nepavyko atidaryti zodzio_pasikartojimas.txt\n";
        return 1;
    }
    for (const auto& [word, count] : word_count) {
        if (count > 1)
            out1 << word << ": " << count << "\n";
    }
    out1.close();

    // isvedami eiluciu skaiciai, kuriose pasikartojo zodziai
    ofstream out2("eilutes_kur_pasikartojo.txt");
    if (!out2) {
        cerr << "Nepavyko atidaryti eilutes_kur_pasikartojo.txt\n";
        return 1;
    }
    for (const auto& [word, lines] : word_lines) {
        if (word_count[word] > 1) {
            out2 << word << ": ";
            bool first = true;
            for (int l : lines) {
                if (!first) out2 << ", ";
                out2 << l;
                first = false;
            }
            out2 << "\n";
        }
    }
    out2.close();

    // Išvedame URL sąrašą
    ofstream out3("urls.txt");
    if (!out3) {
        cerr << "Nepavyko atidaryti urls.txt\n";
        return 1;
    }
    for (const auto& url : urls) {
        out3 << url << "\n";
    }
    out3.close();

    cout << "Rezultatai faile: zodzio_pasikartojimas.txt, eilutes_kur_pasikartojo.txt, urls.txt\n";
    return 0;
}

