#ifndef STORAGE_H
#define STORAGE_H

#include <algorithm>
#include <string>
#include <stack> 
#include <vector>
#include <utility>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace chrono;

typedef struct tree_s tree_t;
typedef struct node_s node_t;
typedef struct data_s data_t;

#define NO_BD
    
#define MAX_FSTR 22 //  (sizeof(char*)-1)   // размер строк хранимых без аллокации памяти   Должен быть на 1 меньше шага грануляции памяти
/*
struct data_s {
    char str[MAX_FSTR];

    bool init(const string& k) {
        strncpy_s(str, MAX_FSTR, k.data(), k.length());
        str[MAX_FSTR - 1] = 0;
        return true;
    }

    void cleanup() {
        str[0] = 0;
    }
    //    string key;
////    string val;
};
*/
struct node_s {    
    node_t* parent;
    node_t *left;
    node_t *right;
    uint64_t weight; // количество узлов у данного узла

    const char* fstr();
#ifdef NO_BD
    char str[MAX_FSTR];

    bool init(const string& k) {
        strncpy_s(str, MAX_FSTR, k.data(), k.length());
        str[MAX_FSTR - 1] = 0;
        return true;
    }

    void cleanup() {
        str[0] = 0;
    }
#else
    bool init(const string& k);
    void cleanup();
#endif

};

struct tree_s
{
    node_t *root; // указатель на корень дерева
};

class bntree
{    
public:
    bntree();
    ~bntree();
    
    // Вставка данных, ключ - значение
    void insert(const string &key, const string &val = "");
    // Удаление узла по индексу
    void erase(uint64_t index);
    // Удаление узла по ключу
    void erase(const string &key);
    // Взятие узла по индексу
    const char* get(uint64_t index);
    // Взятие узла по ключу
    const char* get(const string &key);
    
    // Поиск узла по ключу, возвращает индекс узла
    uint64_t search(const string &key);
    
    // Кол-во элементов в дереве
    uint64_t size();
    
    // Печать дерева
    void print();
    
private:    
    tree_t *tree;
    
    uint64_t get_child_weight(node_t *node);
    node_t *node_new();
    void node_free(node_t *&e);
    
    bool erase_simple(node_t *search_node);
    
    void clear(node_t *p);
    
    void print(node_t *p, int indent);

    void balance(node_t *p);
    node_t* rotateleft(node_t* p);
    node_t* rotateright(node_t* p);

public:
    // Ближайшая степень 2-ки к числу
    uint64_t cpl2(uint64_t x);
    
    // Быстрый логарифм
    uint64_t ilog2(uint64_t x);
    
    // Вес узла к глубине
    uint64_t weight_to_depth(node_t *p);
};

#endif