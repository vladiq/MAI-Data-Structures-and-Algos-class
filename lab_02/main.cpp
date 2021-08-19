#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>

using ull = unsigned long long;

const ull T = 5; // Не меньше 2


struct TKV {
    char* Key;
    ull Val;
};


class TNode {
public:
    friend class TBTree;

    TNode(ull facT);
    void Insert(TNode** root ,TKV& elemToInsert); // Вставка элемента в данный корень поддерева
    TKV* Search(char* keyToSearch);               // Поиск ключа в поддереве, задаваемом данным узлом
    void Delete(char* keyToDelete);               // Удаление элемента из поддерева по заданному ключу
    void DeleteTree();                            // Рекурсивное удаление дерева
    bool NSerialize(FILE* f, TNode* node);        // Рекурсивное сохранение узлов в файл
    bool NDeserialize(FILE* f, TNode* node);      // Рекурсивная загрузка узлов из файла
    ~TNode();

private:
    ull t;
    bool isLeaf;
    unsigned elemsNum;
    TKV* elems;
    TNode** children;
    int FindKey(char* keyToSearch);         // Возвращает индекс элемента, ключ которого больше или равен аргументу 
    TKV& FindMax();                         // Возвращает элемент с максимальным ключом
    TKV& FindMin();                         // Возвращает элемент с минимальным ключом
    void InsertNonFull(TKV& elemToInsert);  // Вставка нового элемента в данный узел. При вызове подразумевается, что узел не полный
    void DeleteFromLeaf(int index);         // Удаление index-ного элемента из листа
    void DeleteFromNonLeaf(int index);      // Удаление index-ного элемента из не листового элемента
    void Fill(int index);                   // Функция для заполнения index-ного ребенка, у которого меньше, чем t - 1 ключей
    void BorrowFromPrev(int index);         // Взять ключ у сына index - 1 и вставить его в сына index
    void BorrowFromNext(int index);         // Взять ключ из ребенка index + 1 и положить в ребенка index
    void Split(int i);                      // Разделение ребёнка данного узла
    void Merge(int index);                  // Функция для слияния сыновей с индексами index и index + 1 данного узла
};


class TBTree {
private:
    ull t;
    TNode* root;

public:
    TBTree(ull facT);
    void Insert(TKV& elemToInsert);             // Добавление элемента в дерево
    void RemoveElement(char* keyToDelete);         // Удаление элемента с заданным ключом
    TKV* SearchWord(char* keyToSearch);         // Поиск в дереве по ключу
    bool Serialize(const char* pathToFile);     // Сохранение дерева в файл
    bool Deserialize(const char* pathToFile);   // Загрузка дерева из файла
    ~TBTree();
};


TBTree::TBTree(ull facT) {
    t = facT;
    root = new TNode(t);
}


// Добавление элемента в дерево
void TBTree::Insert(TKV& elemToInsert) {
    root->Insert(&root, elemToInsert);
}


// Поиск в дереве по ключу
TKV* TBTree::SearchWord(char* keyToSearch) {
    if (root != nullptr) {
        // Ищем ключ в поддереве корня (т.е во всём дереве)
        return root->Search(keyToSearch);
    } else {
        return nullptr;
    }
}


// Удаление элемента с заданным ключом
void TBTree::RemoveElement(char* keyToDelete) {
    root->Delete(keyToDelete);

    // Если у корня 0 ключей, то делаем его первого сына новым корнем. Если этого сына нет, то корень nullptr
    if (root->elemsNum == 0) {
        TNode* temp = root;
        if (root->isLeaf == false) {
            root = root->children[0];
            temp->children[0] = nullptr;
            // Удаляем старый корень
            delete temp;
        }
    }
}


// Сохранение дерева в файл
bool TBTree::Serialize(const char* pathToFile) {
    FILE* f = fopen(pathToFile, "wb");

    if (f == nullptr) {
        return false;
    }

    if (fwrite(&t, sizeof(t), 1, f) != 1) {
        return false;
    }

    bool ans = root->NSerialize(f, root);

    fclose(f);

    return ans;
}


// Загрузка дерева из файла
bool TBTree::Deserialize(const char* pathToFile) {
    FILE* f = fopen(pathToFile, "rb");

    if (f == nullptr) {
        return false;
    }

    if (fread(&t, sizeof(t), 1, f) != 1) {
        return false;
    }

    TNode* rootNew = new TNode(t);
    bool ans = root->NDeserialize(f, rootNew);

    fclose(f);

    if (ans) {
        root->DeleteTree();
        root = rootNew;

        return true;
    } else {
        rootNew->DeleteTree();

        return false;
    }
}


TBTree::~TBTree() { 
    root->DeleteTree();
} 


void Swap(char* &x, char* &y) {
    char* temp = x;
    x = y;
    y = temp;
}


TNode::TNode(ull facT) {
    elemsNum = 0;
    isLeaf = true;
    t = facT;
    // Выделяем память под максимально возможное количество ключей и указателей на детей
    elems = (TKV*)malloc(sizeof(TKV) * (2 * facT - 1));
    children = (TNode**)malloc(sizeof(TNode*) * 2 * facT);
    for (int i = 0; i < 2 * facT; ++i) {
        children[i] = nullptr;
        if (i < 2 * facT - 1) {
            elems[i].Key = nullptr;
        }
    }
}


// Поиск ключа в поддереве, задаваемом данным узлом
TKV* TNode::Search(char* keyToSearch) {
    int i = 0;

    // Ищем первый ключ-строку, большую или равную исходной строке
    while (i < elemsNum && strcmp(keyToSearch, elems[i].Key) > 0) {
        ++i;
    }

    // Строки совпадают, поиск окончен
    if (i < elemsNum && strcmp(keyToSearch, elems[i].Key) == 0){
        return &elems[i];

    // Ключ не найден и мы в листе
    } else if (isLeaf == true) {
        return nullptr;

    // Ключ не найден, мы не в листе, значит переходим к ребенку
    } else {
        return children[i]->Search(keyToSearch);
    }
}


// Функция для разделения ребёнка "у" данного узла
// Ребенок должен быть полным 
void TNode::Split(int i) {
    // Создаем новый узел, который будет содержать (t-1) ключей у
    TNode* z = new TNode(t);
    TNode* y = children[i];
    z->isLeaf = y->isLeaf;
    z->elemsNum = t - 1;
    
    // Копируем последние t-1 ключей из y в z
    for (int j = 0; j < t - 1; ++j) {
        Swap(z->elems[j].Key, y->elems[j + t].Key);
        z->elems[j].Val = y->elems[j + t].Val;
    }
    
    // Копируем последние t детей из y в z
    if (y->isLeaf == false) {
        for (int j = 0; j < t; ++j) {
            z->children[j] = y->children[j + t];
        }
    }

    y->elemsNum = t - 1;

    // Так как у этого узла будет новый сын, то выделяем место для нового сына
    for (int j = elemsNum; j >= i + 1; --j) {
        children[j + 1] = children[j];
    }

    // Подключаем нового сына к этому узлу
    children[i + 1] = z;
    
    // Ключ из y переместится в этот узел. Ищем новый ключ и перемещаем на 1 все ключи, которые больше, чем данный
    for (int j = elemsNum - 1; j >= i; --j) {
        Swap(elems[j + 1].Key, elems[j].Key);
        elems[j + 1].Val = elems[j].Val;
    }

    // Копируем в этот узел ключ из середины у 
    Swap(elems[i].Key, y->elems[t - 1].Key);
    elems[i].Val = y->elems[t - 1].Val;
    ++elemsNum;
}


// Функция для слияния сыновей с индексами index и index + 1 данного узла, причем второй удаляется
void TNode::Merge(int index) {
    int i;
    TNode* child = children[index];
    TNode* brother = children[index + 1];
    
    // Забираем ключ у текущего узла и вставляем его на (t - 1) позицию
    char *temp = child->elems[t - 1].Key;
    child->elems[t - 1].Key = elems[index].Key;
    elems[index].Key = temp;
    child->elems[t - 1].Val = elems[index].Val;
    
    // Копируем ключи сына index + 1 в сына index в конец
    for (i = 0; i < brother->elemsNum; ++i) {
        Swap(child->elems[i + t].Key, brother->elems[i].Key);
        child->elems[i + t].Val = brother->elems[i].Val;
    }
    
     // Теперь копируем ключи на детей
    if (child->isLeaf == false) {
        for(i = 0; i <= brother->elemsNum; ++i) {
            child->children[i + t] = brother->children[i];
        }
    }

    
    // Двигаем все ключи после index в текущем узле на позицию назад, чтобы заполнить пробел после перемещения ключей
    for (i = index + 1; i < elemsNum; ++i) {
        Swap(elems[i - 1].Key, elems[i].Key);
        elems[i - 1].Val = elems[i].Val;
    }
    
    // Двигаем указатели на тедей после index + 1 на одну позицию назад
    for (i = index + 2; i <= elemsNum; ++i) {
        children[i - 1] = children[i];
    }
    
    // Обновляем количество ключей для ребенка и текущего узла
    children[elemsNum] = nullptr;
    child->elemsNum += brother->elemsNum + 1; 
    --elemsNum;
    delete brother;
}


// Вставка элемента в данный корень поддерева
void TNode::Insert(TNode** root, TKV& elemToInsert) {
    // Если корень полный, то высота дерева увеличивается
    if (elemsNum == 2 * t - 1) {
        TNode* s = new TNode(t);
        *root = s;
        s->isLeaf = false;
        s->elemsNum = 0;
        // Делаем старый корень ребенком нового корня
        s->children[0] = this;
        // Разделяем старый корень и перемещаем 1 ключ в новый корень 
        s->Split(0);
        s->InsertNonFull(elemToInsert);
    // Если корень не полный
    } else {
        (*root)->InsertNonFull(elemToInsert);
    }
}


// Вставка нового элемента в данный узел. При вызове подразумевается, что узел не полный
void TNode::InsertNonFull(TKV& elemToInsert) {
    int i = elemsNum - 1;
    
    if (isLeaf) {
        // Находим место для вставки нового ключа и перемещаем все большие ключи на шаг вперед
        while(i >= 0 && strcmp(elemToInsert.Key, elems[i].Key) < 0) {
            Swap(elems[i + 1].Key, elems[i].Key);
            elems[i + 1].Val = elems[i].Val;
            --i;
        }

        // Вставляем новый элемент 
        elems[i + 1].Key = (char *)malloc(sizeof(char) * strlen(elemToInsert.Key) + 1);
        strcpy(elems[i + 1].Key, elemToInsert.Key);
        elems[i + 1].Val = elemToInsert.Val;
        ++elemsNum;
        std::cout << "OK" << std::endl;
    } else {
        // Находим ребенка, у которого будет новый ключ 
        while(i >= 0 && 0 > strcmp(elemToInsert.Key, elems[i].Key)) {
            --i;
        }
        ++i;
        // Проверяем, не полный ли ребенок
        if(children[i]->elemsNum == 2 * t - 1) {
            Split(i);
            // После разделения ключ в середине сына i перемещается вверх, а сам сын делится на 2.
            // Потом проверяется, в кого из них кладётся новый ключ 
            if (strcmp(elemToInsert.Key, elems[i].Key) > 0) {
                ++i;
            }
        }
        children[i]->InsertNonFull(elemToInsert);
    }
}


// Возвращает индекс элемента, ключ которого больше или равен аргументу
int TNode::FindKey(char* keyToSearch) {
    unsigned first = 0;
    unsigned last = elemsNum;
    while (first < last) {
        unsigned mid = first + (last - first) / 2;
        if (strcmp(keyToSearch, elems[mid].Key) <= 0) {
            last = mid;
        } else {
            first = mid + 1;
        }
    }
    return last;
}


// Удаление index-ного элемента из листа
void TNode::DeleteFromLeaf(int index) {
    // Сдвигаем ключи после index-ной позиции на 1 позицию назад
    for (int i = index + 1; i < elemsNum; ++i) {
        Swap(elems[i-1].Key, elems[i].Key);
        elems[i-1].Val = elems[i].Val;
    }
    --elemsNum;
    free(elems[elemsNum].Key);
    elems[elemsNum].Key = nullptr;
    std::cout << "OK" << std::endl;
}


// Возвращает элемент с максимальным ключом
TKV& TNode::FindMax() {
    TNode* temp = this;
    while (temp->isLeaf == false) {
        temp = temp->children[temp->elemsNum];
    }
    return temp->elems[temp->elemsNum - 1];
}


// Возвращает элемент с минимальным ключом
TKV& TNode::FindMin() {
    TNode* temp = this;
    while (temp->isLeaf == false) {
        temp = temp->children[0];
    }
    return temp->elems[0];
}


// Функция для заполнения ребенка index, у которого меньше, чем t - 1 ключей
void TNode::Fill(int index) {
    // Если у предыдущего сына больше, чем t - 1 ключей, то берем ключ у него
    if (index != 0 && children[index - 1]->elemsNum >= t) {
        BorrowFromPrev(index);

    // То же самое со следующим сыном
    } else if (index != elemsNum && children[index + 1 ]->elemsNum >= t) {
        BorrowFromNext(index);

    // Иначе - слияние index ребенка с братом. Если эо последний ребенок, то слияем его с предыдущим братом
    } else {
        if (index != elemsNum) {
            Merge(index);
        } else {
            Merge(index - 1);
        }
    }
}


// Взять ключ у сына index - 1 и вставить его в сына index
// Последний ключ из сына index - 1 переходит к родителю и ключ index - 1 родителя становится первым ключом index-ного сына
void TNode::BorrowFromPrev(int index) {
    TNode *child = children[index];
    TNode *brother = children[index - 1];

    // Перемешаем все ключи index-ного ребенка на 1 позицию вперёд
    for (int i = child->elemsNum - 1; i >= 0; --i) {
        Swap(child->elems[i + 1].Key, child->elems[i].Key);
        child->elems[i + 1].Val = child->elems[i].Val;
    }

    // Если index-ный сын не лист, то перемещаем на 1 все указатели его сына
    if (child->isLeaf == false) {
        for(int i = child->elemsNum; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    // Присваиваем первому ключу ребенка ключ index - 1 из текущего узла
    Swap(child->elems[0].Key, elems[index - 1].Key);
    child->elems[0].Val = elems[index - 1].Val;

    // Делаем последнего сына брата первым сыном index-ного ребенка
    if (isLeaf == false) {
        child->children[0] = brother->children[brother->elemsNum];
    }

    // Перемещаем ключ от брата к родителю
    Swap(elems[index - 1].Key, brother->elems[brother->elemsNum - 1].Key);
    elems[index - 1].Val = brother->elems[brother->elemsNum - 1].Val;

    ++child->elemsNum;
    --brother->elemsNum;

    return;
}


// Взять ключ из ребенка index + 1 и положить в ребенка index
void TNode::BorrowFromNext(int index) {
    TNode* child = children[index];
    TNode* brother = children[index + 1];

    // index-ный ключ вставляется как последний ключ index-ного сына
    Swap(child->elems[(child->elemsNum)].Key, elems[index].Key);
    child->elems[(child->elemsNum)].Val = elems[index].Val;

    // Первый сын брата становится последним ребенком index-ного сына
    if (child->isLeaf == false) {
        child->children[(child->elemsNum) + 1] = brother->children[0];
    }

    // Первый ключ брата вставляется в массив ключей на index-ное место
    Swap(elems[index].Key, brother->elems[0].Key);
    elems[index].Val = brother->elems[0].Val;
 
    // Перемещаем все ключи брата на позицию назад
    for (int i = 1; i < brother->elemsNum; ++i) {
        Swap(brother->elems[i-1].Key, brother->elems[i].Key);
        brother->elems[i-1].Val = brother->elems[i].Val;
    }

    // Перемещаем указатели на детей брата на позицию назад 
    if (brother->isLeaf == false) {
        for(int i = 1; i <= brother->elemsNum; ++i)
            brother->children[i - 1] = brother->children[i];
    }

    child->elemsNum += 1;
    brother->elemsNum -= 1;

    return;
}


// Удаление index-ного ключа не листового узла
void TNode::DeleteFromNonLeaf(int index) {
    char array[257];
    strcpy(array, elems[index].Key);

    if (children[index]->elemsNum >= t) {
        // Удаляем элемент с максимальным ключом
        TKV &temp = children[index]->FindMax();
        free(elems[index].Key);
        elems[index].Key = (char*)malloc(sizeof(char) * strlen(temp.Key) + 1);
        strcpy(elems[index].Key, temp.Key);
        elems[index].Val = temp.Val;
        children[index]->Delete(temp.Key);

    } else if (children[index + 1]->elemsNum >= t) {
        // Удаляем элемент с минимальным ключом
        TKV& temp = children[index + 1]->FindMin();
        free(elems[index].Key);
        elems[index].Key = (char*)malloc(sizeof(char) * strlen(temp.Key) + 1);
        strcpy(elems[index].Key, temp.Key);
        elems[index].Val = temp.Val;
        children[index + 1]->Delete(temp.Key);

    // Если у сыновей index и index + 1 меньше, чем t ключей, то делаем слияние в index
    } else {
        Merge(index);
        children[index]->Delete(array);
    }
}


// Удаление элемента из поддерева по заданному ключу
void TNode::Delete(char* keyToDelete) {
    int index = FindKey(keyToDelete);
    // Если удаляемый ключ содержится в этом узле
    if (index < elemsNum && strcmp(keyToDelete, elems[index].Key) == 0) {
        if (isLeaf) {
            DeleteFromLeaf(index);
        } else {
            DeleteFromNonLeaf(index);
        } 

    // Если мы добрались до листа до сих пор не нашли совпадения
    } else {
        if (isLeaf) {
            std::cout << "NoSuchWord" << std::endl;
            return;
        }

        // flag показывает, есть ли ключ в поддереве последнего ребенка данного узла
        bool flag = (index == elemsNum) ? true : false;

        // Если сын, у которого должен быть искомый ключ, имеет меньше, чем t ключей, то заполняем его
        if (children[index]->elemsNum < t) {
            Fill(index);
        }

        // Если у последнего ребенка было слияние, то оно происходило с предыдущим сыном, тогда:
        if (flag == true && index > elemsNum) {
            children[index - 1]->Delete(keyToDelete);
        } else {
            children[index]->Delete(keyToDelete);
        }
    }
    return;
}


// Сохранение дерева в файл
bool TNode::NSerialize(FILE* f, TNode* node) {
    if (fwrite(&node->elemsNum, sizeof(node->elemsNum), 1, f) != 1) {
        return false;
    }

    if (fwrite(&node->isLeaf, sizeof(node->isLeaf), 1, f) != 1) {
        return false;
    }

    for (unsigned i = 0; i < node->elemsNum; ++i) {
        const TKV* data = &node->elems[i];
        const size_t keyLen = strlen(data->Key);
        const char* keyToSave = data->Key;

        if (fwrite(&keyLen, sizeof(keyLen), 1, f) != 1) {
            return false;
        }
        if (fwrite(keyToSave, sizeof(char), keyLen, f) != keyLen) {
            return false;
        }
        if (fwrite(&data->Val, sizeof(data->Val), 1, f) != 1) {
            return false;
        }
    }

    if (node->isLeaf == false) {
        for (unsigned i = 0; i < node->elemsNum + 1; ++i) {
            if (NSerialize(f, node->children[i]) == false) {
                return false;
            }
        }
    }

    return true;
}


// Загрузка дерева из файла
bool TNode::NDeserialize(FILE* f, TNode* node) {
    char copyKey[257];

    if (fread(&node->elemsNum, sizeof(node->elemsNum), 1, f) != 1) {
        return false;
    }
    if (fread(&node->isLeaf, sizeof(node->isLeaf), 1, f) != 1) {
        return false;
    }

    for (unsigned i = 0; i < node->elemsNum; ++i) {
        TKV* data = &node->elems[i];
        size_t keyLen = 0;

        if (fread(&keyLen, sizeof(keyLen), 1, f) != 1) {
            return false;
        }
        if (fread(copyKey, sizeof(char), keyLen, f) != keyLen) {
            return false;
        }
        if (fread(&data->Val, sizeof(data->Val), 1, f) != 1) {
            return false;
        }

        copyKey[keyLen] = '\0';
        data->Key = (char *)malloc(sizeof(char) * keyLen + 1);
        strcpy(data->Key, copyKey);
    }

    if (node->isLeaf == false) {
        for (unsigned i = 0; i < node->elemsNum + 1; ++i) {
            node->children[i] = new TNode(t);

            if (NDeserialize(f, node->children[i]) == false) {
                return false;
            }
        }
    }

    return true;
}


TNode::~TNode() {
    for (unsigned i = 0; i < elemsNum; ++i) {
        free(elems[i].Key);
    }
    free(elems);
    free(children);
}


// Рекурсивное удаление дерева
void TNode::DeleteTree() {
    TNode *tmp = this;
    if (tmp == nullptr) {
        return;
    }
    for (unsigned i = 0; i < tmp->elemsNum + 1; ++i) {
            tmp->children[i]->DeleteTree();
    }
    delete(tmp);
}


// Приведение символа в нижний регистр
char CharToLower(char c) {
    return isupper(c) ? c - 'A' + 'a' : c; 
}


// Парсинг входной строки
void Parsing(char* action, char* inputStr, ull* value) {
    char ch;
    unsigned i = 0;

    ch = getchar();

    if (ch == EOF) {
        *action = 'Q';
        return;
    }

    if (ch == '+' || ch == '-') {
        *action = ch;
        getchar();
        while (true) {
            ch = CharToLower(getchar());
            if (isalpha(ch) == 0) {
                break;
            } else {
                inputStr[i++] = ch;
            }
        }
        inputStr[i] = '\0';

        if (*action == '+') {
            *value = 0;

            while ((ch = getchar()) != '\n') {
                *value = (*value) * 10 + ch - '0';
            }
        }

    } else if (ch == '!') {
        getchar();
        while ((ch = getchar()) != ' ') {
            inputStr[i++] = ch;
        }
        inputStr[i] = '\0';
        i = 0;
        *action = inputStr[0];
        while((ch = getchar()) != '\n') {
            inputStr[i++] = ch;
        }
        inputStr[i] = '\0';

    } else {
        *action = 'B';
        inputStr[0] = CharToLower(ch);
        i++;
        while((ch = getchar()) != '\n') {
            inputStr[i++] = CharToLower(ch);
        }
        inputStr[i] = '\0';
    }
}

int main(void) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TBTree tree(T);
    TKV elemToInsert;
    char inputStr[257];
    char action;
    ull value;

    while (true) {
        Parsing(&action, inputStr, &value);
        
        if (action == 'Q') {
            break;
        }

        switch (action) {
            case '+': 
                elemToInsert.Key = (char*)malloc(sizeof(char) * strlen(inputStr) + 1);
                strcpy(elemToInsert.Key, inputStr);
                elemToInsert.Val = value;

                if (tree.SearchWord(inputStr) == nullptr) {
                    tree.Insert(elemToInsert);
                } else {
                    std::cout << "Exist" << std::endl;
                }
                free(elemToInsert.Key);
                break;

            case '-':
                    tree.RemoveElement(inputStr);
                break;

            case 'S': {
                std::ofstream fileToSave(inputStr);
                if (!fileToSave.is_open()) {
                    std::cout << "ERROR: Invalid file to save to" << std::endl;
                } else {
                    std::cout << ((tree.Serialize(inputStr) ? "OK" : "ERROR: Cannot create a file")) << std::endl;
                }
                break;
            }

            case 'L': {
                std::ifstream fileToLoad(inputStr);
                if (!fileToLoad.is_open()) {
                    std::cout << "ERROR: Invalid file to read from" << std::endl;
                } else {
                    std::cout << ((tree.Deserialize(inputStr) ? "OK" : "ERROR: Cannot load the file")) << std::endl;
                }
                break;
            }

            case 'B':
                if (tree.SearchWord(inputStr) != nullptr) {
                    std::cout << "OK: " << tree.SearchWord(inputStr)->Val << std::endl;
                } else {
                    std::cout << "NoSuchWord" << std::endl;
                }
                break;
        }
    }
}
