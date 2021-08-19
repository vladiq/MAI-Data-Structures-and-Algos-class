
#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <vector>

const char SENTINEL = '$';

class TSuffixArray;

class TNode {
public:
    std::map<char,TNode*> Children;
    TNode* SufLink;
    std::pair<std::string ::iterator, std::string::iterator> Coords;
    long long int SufIndex;

    TNode(std::string::iterator it1,std::string::iterator it2);
    ~TNode() = default;
};

class TSuffixTree {
public:
    TSuffixTree(std::string input);
    ~TSuffixTree();
    friend TSuffixArray;

private:
    std::string Text;
    TNode* Root;
    long long int Remainder, ActiveLength;
    TNode* NodeForSufLink, *ActiveNode;
    std::string::iterator ActiveEdge;

    long long EdgeLength(TNode* node);
    void AddSufLink(TNode* node);
    void Destroy(TNode* node);
    void TreeExtend(std::string::iterator add);
    bool WalkDown(TNode *node);
    void DFS(TNode *node, std::vector<int> &vector, int d);
};

class TSuffixArray {
public:
    std::vector<int> Find(std::string& pattern);
    TSuffixArray(TSuffixTree& Tree);
    ~TSuffixArray() = default;

private:
    std::vector<int> Array;
    std::string Text;
    int Compare(int index, const std::string& pattern);
};

TNode::TNode(std::string::iterator it1, std::string::iterator it2) {
    Coords.first = it1;
    Coords.second = it2;
    this->SufLink = nullptr;
    this->SufIndex = -1;
}

void TSuffixTree::Destroy(TNode *node) {
    for (auto it = node->Children.begin(); it != node->Children.end(); ++it) {
        Destroy(it->second);
    }
    delete node;
}

TSuffixTree::~TSuffixTree() {
    Destroy(Root);
}

TSuffixTree::TSuffixTree(std::string input) {
    Text = input;
    Remainder = 0;
    ActiveLength = 0;
    Root = new TNode(Text.end(),Text.end());
    ActiveNode = NodeForSufLink = Root->SufLink = Root;
    for (auto it = Text.begin(); it != Text.end(); ++it) {
        TreeExtend(it);
    }
}


long long TSuffixTree::EdgeLength(TNode* node) {
    return node->Coords.second - node->Coords.first;
}

bool TSuffixTree::WalkDown(TNode *node) {
    if (ActiveLength >= EdgeLength(node)) {
        ActiveEdge += EdgeLength(node);
        ActiveNode = node;
        ActiveLength -= EdgeLength(node);
        return true;
    } else {
        return false;
    }
}

void TSuffixTree::AddSufLink(TNode *node) {
    if (NodeForSufLink != Root) {
        NodeForSufLink->SufLink = node;
        NodeForSufLink = node;
    } else {
        NodeForSufLink = node;
    }
}


void TSuffixTree::TreeExtend(std::string::iterator newSymbol) {
    ++Remainder;
    NodeForSufLink = Root;
    TNode* nextNode;
    while (Remainder > 0) {
        if (ActiveLength == 0) {
            ActiveEdge = newSymbol;
        }
        auto iterator = ActiveNode->Children.find(*ActiveEdge);
        
        if (iterator != ActiveNode->Children.end()) {
        	nextNode = iterator->second;
        } else {
        	nextNode = nullptr;
        }
        if (nextNode == nullptr) {
            TNode* newLeaf = new TNode(newSymbol,Text.end());
            ActiveNode->Children[*ActiveEdge] = newLeaf;
            AddSufLink(ActiveNode);
        } else {
            if (WalkDown(nextNode)) {
                continue;
            }
            if (*(nextNode->Coords.first + ActiveLength) == *newSymbol) {
                AddSufLink(ActiveNode);
                ++ActiveLength;
                break;
            }
            TNode *newInnerNode = new TNode(nextNode->Coords.first,nextNode->Coords.first + ActiveLength);
            TNode* newLeaf = new TNode(newSymbol,Text.end());
            ActiveNode->Children[*ActiveEdge] = newInnerNode;
            newInnerNode->Children[*newSymbol] = newLeaf;
            nextNode->Coords.first += ActiveLength;
            newInnerNode->Children[*nextNode->Coords.first] = nextNode;
            AddSufLink(newInnerNode);
        }
        --Remainder;
        if (ActiveNode == Root && ActiveLength > 0) {
            ActiveEdge = newSymbol - Remainder + 1;
            --ActiveLength;
        } else {
            if (ActiveNode->SufLink != nullptr) {
                ActiveNode = ActiveNode->SufLink;
            } else {
                ActiveNode = Root;
            }
        }
    }
}

void TSuffixTree::DFS(TNode *node, std::vector<int> &vector, int d) {
    if (node->Children.empty()) {
        vector.push_back(Text.size() - d);
        return;
    }
    for (auto it = node->Children.begin(); it != node->Children.end(); ++it) {
        if (it->first == '$' && node == Root) {
            continue;
        }
        int tmpd = d;
        tmpd += it->second->Coords.second - it->second->Coords.first;
        DFS(it->second,vector,tmpd);
    }
}


TSuffixArray::TSuffixArray(TSuffixTree &Tree) {
    Tree.DFS(Tree.Root,this->Array,0);
    this->Text = std::move(Tree.Text);
}


std::vector<int> TSuffixArray::Find(std::string &pattern) {
    std::vector<int> result;
    int left = 0, right = Array.size() - 1, m;
    while (left <= right) {
        m = (left + right) / 2;
        int compare_result = Compare(Array[m],pattern);
        if (compare_result == 1) {
            left = m + 1;
        } else if (compare_result == -1) {
            right = m -1;
        }
        if (compare_result == 0) {
            int left_bound = m;
            while ( left_bound > 0 && Compare(Array[left_bound - 1],pattern) == 0) {
                --left_bound;
            }
            int right_bound = m;
            while ( right_bound < Array.size() - 1 && Compare(Array[right_bound + 1],pattern) == 0 ) {
                ++right_bound;
            }
            if (right_bound == left_bound) {
                result.push_back(Array[left_bound]);
            } else {
                for (int i = left_bound; i <= right_bound; ++ i) {
                    result.push_back(Array[i]);
                }
            }

            break;
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

int TSuffixArray::Compare(int index, const std::string &pattern) {
	int min_size;
	if (Text.size() - index < pattern.size()) {
		min_size = Text.size();
	} else {
		min_size = pattern.size();
	}
    for (int i = 0; i < min_size; ++i) {
        if (Text[index] != pattern[i]) {
            if (Text[index] < pattern[i]) {
                return 1;
            } else {
                return -1;
            }
        }
        ++index;
    }
    if (pattern.size() <= min_size) {
        return 0;
    }
    return 1;
}

int main() {
    std::string text, pattern;
    std::cin >> text;
    TSuffixTree tree(text + SENTINEL);
    TSuffixArray array(tree);
    for (int j = 1; std::cin >> text; ++j) {
        std::vector<int> result = array.Find(text);
        if (!result.empty()) {
            std::cout << j << ": ";
            for (int i = 0; i < result.size(); ++i) {
                std::cout << result[i] + 1;
                if (i < result.size() -  1) {
                    std::cout << ", ";
                }
            }
            std::cout << '\n';
        }
    }

    return 0;
}