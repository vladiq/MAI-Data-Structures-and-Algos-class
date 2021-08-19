#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

class TAG {
public:
    void AGPatternSearch(const std::vector<uint32_t>& pattern, 
                    const std::vector<std::pair<uint32_t, std::pair<int, int>>>& text);
private:
    std::unordered_map<uint32_t, int> badCharShift;
    std::unique_ptr<int[]> goodSuffixShift;
    std::vector<int> N;
    bool IsPrefix(const std::vector<uint32_t>& pattern, int position);
    int SuffixLength(const std::vector<uint32_t>& pattern, int position);
    void PreprocessBadChar(const std::vector<uint32_t>& pattern);
    void PreprocessGoodSuffix(const std::vector<uint32_t>& pattern);
    void NFunction(const std::vector<uint32_t>& pattern);
};

void TAG::AGPatternSearch(const std::vector<uint32_t>& pattern,
            const std::vector<std::pair<uint32_t, std::pair<int, int>>>& text) {
    PreprocessBadChar(pattern);
    PreprocessGoodSuffix(pattern);
    NFunction(pattern);

    int j = pattern.size() - 1;
    std::vector<int> m(text.size());

    while (j < text.size()) {
        int i = pattern.size() - 1;
        int h = j;
        while (true) {
            if (m[h] == 0) {
                if (text[h].first == pattern[i] && i == 0) {
                    m[j] = pattern.size();
                    std::cout << text[h].second.first << ", " << text[h].second.second << std::endl;
                    h += std::max(badCharShift[text[h].first], goodSuffixShift[pattern.size() - 1 - i]);
                    j = h;
                    break;
                } else if (text[h].first == pattern[i] && i > 0) {
                    h--;
                    i--;
                    continue;
                } else if (text[h].first != pattern[i]) {
                    m[j] = j - h;
                    h += std::max(badCharShift[text[h].first], goodSuffixShift[pattern.size() - 1 - i]);
                    j = h;
                    break;
                }
            } else if (m[h] < N[i]) {
                i -= m[h];
                h -= m[h];
                continue;
            } else if (m[h] >= N[i] && N[i] == i + 1) {
                m[j] = j - h;
                std::cout << text[j - pattern.size() + 1].second.first << ", " << text[j - pattern.size() + 1].second.second << std::endl;
                j += 1;
                break;
            } else if (m[h] > N[i] && N[i] < i + 1) {
                m[j] = j - h;
                h += std::max(badCharShift[text[j - pattern.size() + 1 + i - N[i]].first], goodSuffixShift[pattern.size() - i + N[i]]);
                j = h;
                break;
            } else if (m[h] == N[i] && N[i] < i + 1) {
                i -= m[h];
                h -= m[h];
                continue;
            }
        }
    }
}

void TAG::NFunction(const std::vector<uint32_t>& pattern) {
    N.resize(pattern.size());
    int size = pattern.size();
    int l = size - 1;
    int r = size - 1;
    int k = size - 2;

    while (k >= 0) {
        if (k < r) {
            for (int i = size - 1, j = k; j != -1; i--, j--) {
                if (pattern[i] == pattern[j]) {
                    N[k]++;
                } else {
                    break;
                }
            }
            if (N[k] > 0) {
                r = k - N[k] + 1;
                l = k;
            }
            k--;
        } else {
            if (N[size - 1 - l + k] < k - r + 1) {
                N[k] = N[size - 1 - l + k];
            } else {
                N[k] = k - r + 1;
                for (int i = size - k + r - 2, j = r - 1; j != -1; i--, j--) {
                    if (pattern[i] == pattern[j]) {
                        N[k]++;
                    } else {
                        break;
                    }
                }
                if (k - N[k] + 1 <= r - 1) {
                    r = k - N[k] + 1;
                    l = k;
                }
            }
            k--;
        }
    }
}

void TAG::PreprocessBadChar(const std::vector<uint32_t>& pattern) {
    int n = pattern.size();
    for(int i = 0; i < n - 1; ++i) {
        badCharShift[pattern[i]] = n - i - 1;
    }
}

bool TAG::IsPrefix(const std::vector<uint32_t>& pattern, int position) {
    int j = 0;
    int n = pattern.size();
    for(int i = position; i < n; ++i) {
        if(pattern[i] != pattern[j]) {
            return false;
        }
        ++j;
    }
    return true;
}

int TAG::SuffixLength(const std::vector<uint32_t>& pattern, int position) {
    int length = 0;
    int j = pattern.size() - 1;
    int i = position;
    while(i >= 0 && pattern[i] == pattern[j]) {
        ++length;
        --i;
        --j;
    }
    return length;
}

void TAG::PreprocessGoodSuffix(const std::vector<uint32_t>& pattern) {
    int patternLength = pattern.size();
    goodSuffixShift = std::unique_ptr<int[]>(new int[patternLength]);
    int lastPrefixOccurance = patternLength;
    for(int i = patternLength - 1; i >= 0; --i) {
        if(IsPrefix(pattern, i + 1)) {
            lastPrefixOccurance = i + 1;
        }
        goodSuffixShift[patternLength - 1 - i] = lastPrefixOccurance + patternLength - i - 1;
    }

    for(int i = 0; i < patternLength - 1; ++i) {
        int suffixLength = SuffixLength(pattern, i);
        goodSuffixShift[suffixLength] = patternLength + suffixLength - i - 1;
    }
}

void GetPattern(std::vector<uint32_t>& pattern) {
    uint32_t tempItem = 0;
    int c;
    bool numRead = false;
    c = getchar();
    if (c == '\n' || c == EOF) {
        return;
    }
    while(true) {
        if(c == ' ') {
            if(numRead) {
                pattern.push_back(tempItem);
                numRead = false;
                tempItem = 0;
            }
        } else {
            if(!numRead) {
                numRead = true;
            }
            tempItem = tempItem * 10 + (c - '0');
        }
        c = getchar();
        if(c == '\n') {
            break;
        }
    }
    if(numRead) {
        pattern.push_back(tempItem);
    }
}

void GetText(std::vector<std::pair<uint32_t, std::pair<int, int>>>& text) {
    int wordCount = 1;
    int lineCount = 1;
    uint32_t tempItem = 0;
    int c;
    bool numRead = false;
    c = getchar();
    if (c == EOF) {
        return;
    }
    while (true) {
        if(c == ' ' || c == '\n') {
            if(numRead) {
                text.push_back({tempItem, {lineCount, wordCount}});
                numRead = false;
                tempItem = 0;
                wordCount++;
            }
            if(c == '\n') {
                lineCount++;
                wordCount = 1;
            }
        } else {
            if(!numRead) {
                numRead = true;
            }
            tempItem = tempItem * 10 + (c - '0');
        }
        c = getchar();
        if(c == EOF) {
            break;
        }
    }
    if(numRead) {
        text.push_back({tempItem, {lineCount, wordCount}});
    }
}

int main() {
    std::vector<uint32_t> pattern;
    GetPattern(pattern);
    if (pattern.size() == 0) {
        return 0;
    }

    std::vector<std::pair<uint32_t, std::pair<int, int>>> text;
    GetText(text);
    if (pattern.size() > text.size() || text.size() == 0) {
        return 0;
    }

    TAG solution;
    solution.AGPatternSearch(pattern, text);
    return 0;
} 
