#include "bntree.hpp"




bntree::bntree() {
    this->tree = new tree_t();
}

bntree::~bntree() {
    this->clear(this->tree->root);
    node_free (this->tree->root);
}


#define HeapSize 1100000  // Должен превышать ёмкост данных теста

/*  Быстрая память */

template <typename DATA>
class MemHeap {
    DATA _heap[HeapSize];

    int _end;
    DATA* _freelist;
public:

    inline
        DATA& operator[] (size_t idx) {
        return _heap[idx];
    }
    MemHeap() :_end(0), _freelist(0) {}

    // init() и cleanup() объекта должен сделать вызывающий

    inline
        size_t idx(const DATA* ptr) {
        return (((char*)ptr) - ((char*)&(_heap[0]))) / sizeof(_heap[0]);
    }

    inline
        bool owner(const void* ptr) {
        return(
            ((void*)ptr >= (void*)(&(_heap[0])))
            && ((void*)ptr < (void*)(&(_heap[HeapSize])))
            );
    }

    void Delete(void* d) {
        if (d < (void*)(&(_heap[0]))) return;
        if (d >= (void*)(&(_heap[HeapSize]))) return;
        *(DATA**)(d) = _freelist;
        _freelist = (DATA*)d;
    }
    DATA* New() {
        if (_end < HeapSize - 2) {
            _end += 1;
            return &(_heap[_end]);
        }
        if (_freelist) {
            DATA* d = _freelist;
            _freelist = *(DATA**)(d);
            return d;
        }
        return NULL;
    }
}; //////////////////////////////////////////////////////

MemHeap<node_t> THeap;

node_t* bntree::node_new() {
    //    return new node_t();
    node_t* rez = THeap.New();
//    if (rez) rez->init();
    return rez;
}

void bntree::node_free(node_t* &n) {
    //    delete n;
//    n->cleanup();
    THeap.Delete(n);
    n = 0;
}

#ifndef NO_BD
struct BD {
    char str[MAX_FSTR];

    bool init(const string& k) {
        strncpy_s(str, MAX_FSTR, k.data(), k.length());
        str[MAX_FSTR - 1] = 0;
        return true;
    }

    void cleanup() {
        str[0] = 0;
    }

};

MemHeap<BD>  BDHeap;

inline
bool node_s::init(const string& k) {
    size_t idx = THeap.idx(this);
    if (idx) {
        return BDHeap[idx].init(k);
    }
    return false;
}
#endif

inline 
const char* node_s::fstr() {
#ifndef NO_BD
    size_t idx = THeap.idx(this);
    if (idx) {
        return BDHeap[idx].str;
}
    return 0;
#else
        return str;
#endif
}


void bntree::clear(node_t *p) {
    if (p != NULL) {
        if (p->right) {
            this->clear(p->right);
            this->node_free(p->right);
        }

        if (p->left) {
            this->clear(p->left);
            this->node_free(p->left);
        }
    }
}

uint64_t bntree::search(const string &key) {
    if (!this->tree->root) {
        return -1;
    }

    node_t *search_node = this->tree->root;
    uint64_t index_node = this->get_child_weight(search_node->left);

    for (;;) {
        int cmp = strcmp(key.data(), search_node->fstr());
        if (cmp == 0) {
            return index_node; // найден узел с таким ключем, вернем его индекс
        } else if (cmp>0) {
            search_node = search_node->right;

            if (search_node == NULL)
                return -1;

            index_node += (this->get_child_weight(search_node->left) + 1);
        } else {
            search_node = search_node->left;

            if (search_node == NULL)
                return -1;

            index_node -= (this->get_child_weight(search_node->left) + 1);
        }
    }
}

size_t dpt, maxdpt, countdpt, totaldpt;  //  измеритель глубины

void bntree::insert(const string &key, const string &val) {
    node_t *search_node, *prev_node, **node;
    dpt = 0;
    node = &this->tree->root;
    search_node = this->tree->root;
    prev_node = NULL;
    uint64_t index_node;

    if (!this->tree->root) {
        index_node = 0;
    } else {
        index_node = this->get_child_weight(search_node->left);
    }

    uint64_t index_del = -1;
    for (;;) {
        ++dpt;

        if (search_node == NULL) {
            // Добавялем узел
            search_node = *node = this->node_new();

            search_node->init(key);
//            search_node->data.val = val;
            search_node->weight = 1; // новый узел имеет вес 1

            search_node->left = search_node->right = NULL;
            search_node->parent = prev_node;

            break;
        } else{
            int cmp = strcmp(key.data(), search_node->fstr());
            if (cmp>0) {
              // Идем направо
              prev_node = search_node;
              search_node->weight++; // увеличиваем вес узла
              node = &search_node->right;
              search_node = search_node->right;

              if (search_node)
                  index_node += (this->get_child_weight(search_node->left) + 1);

            } else if (cmp<0) {
            // Идем налево
              prev_node = search_node;
              search_node->weight++; // увеличиваем вес узла
              node = &search_node->left;
              search_node = search_node->left;

              if (search_node)
                index_node -= (this->get_child_weight(search_node->left) + 1);
            }
            else {
                // Если такой ключ уже существует, то обновляем значение.
  //              search_node->data.val = val;

                // Идем назад и отменяем изменение веса у верхних узлов
                for (;;) {

                    if (search_node->parent) {
                        search_node = search_node->parent;
                    }
                    else {
                        return;
                    }

                    search_node->weight--;
                }
            }
        }
    }
    if (maxdpt < dpt) {
        maxdpt = dpt;
        cout << dpt << " max B-tree depth" << endl;
    }
    totaldpt += dpt;
    countdpt += 1;

    this->balance(search_node);

    return;
}

bool bntree::erase_simple(node_t *search_node) {

    node_t *prev_node = search_node->parent;

    if (!search_node->left && !search_node->right) {
        // Удаляемый узел является листом.

        // Обнуляем соответствующую ссылку родителя.
        if (prev_node == NULL) {
            // Удаляемый узел корень.
            this->tree->root = NULL;
        } else if (prev_node->left == search_node) {
            prev_node->left = NULL;
        } else if (prev_node->right == search_node) {
            prev_node->right = NULL;
        }

    } else if (search_node->left && !search_node->right) {
        // Удаляемый узел имеет только левого ребенка.

        // Перекомпануем ссылки родителя на левого внука.
        if (prev_node == NULL) {
            // Удаляемый узел корень.
            this->tree->root = search_node->left;
            this->tree->root->parent = NULL;
        } else if (prev_node->left == search_node) {
            prev_node->left = search_node->left;
            search_node->left->parent = prev_node;
        } else if (prev_node->right == search_node) {
            prev_node->right = search_node->left;
            search_node->left->parent = prev_node;
        }

    } else if (!search_node->left && search_node->right) {
        // Удаляемый узел имеет только правого ребенка.

        // Перекомпануем ссылки родителя на правого внука.
        if (prev_node == NULL) {
            // Удаляемый узел корень.
            this->tree->root = search_node->right;
            this->tree->root->parent = NULL;
        } else if (prev_node->left == search_node) {
            prev_node->left = search_node->right;
            search_node->right->parent = prev_node;
        } else if (prev_node->right == search_node) {
            prev_node->right = search_node->right;
            search_node->right->parent = prev_node;
        }

    } else {
        // Удаляемый узел имеет двух детей. Здесь не обрабатываем.

        return false;
    }

    return true;
}

void bntree::erase(uint64_t index) {

    if (index < 0 || index > this->size() - 1) {
        return;
    }

    node_t *search_node = this->tree->root;
    uint64_t index_node = this->get_child_weight(search_node->left);

    for (;;) {
        if (index == index_node) {
            if (this->erase_simple(search_node)) {
                // pass
            } else if (search_node->left && search_node->right) {
                // Самый сложный случай, удаляемый узел имеет 2-х детей.

                node_t *del_node = search_node;
                uint64_t _index;
                // Маленькая балансировка, если правый вес больше левого то удаляем через право
                // иначе через лево
                if (search_node->right->weight > search_node->left->weight) {
                    _index = index_node + 1;
                } else {
                    _index = index_node - 1;
                }

                // Ищем узел со следующим индексом.
                for (;;) {
                    if (_index == index_node) {
                        // Узел найден
                        break;
                    } else if (_index > index_node) {
                        search_node->weight--;
                        search_node = search_node->right;
                        index_node += (this->get_child_weight(search_node->left) + 1);
                    } else {
                        search_node->weight--;
                        search_node = search_node->left;
                        index_node -= (this->get_child_weight(search_node->right) + 1);
                    }
                }
/*
                del_node->data = search_node->data;
/*/
#ifndef NO_BD
                size_t del_idx = THeap.idx(del_node);
                size_t search_idx = THeap.idx(search_node);
                BDHeap[del_idx] = BDHeap[search_idx];
#else
                strcpy_s( del_node->str, search_node->str);

#endif
//*/
                this->erase_simple(search_node);
            }

            this->node_free(search_node);

            return;
        } else if (index > index_node) {
            search_node->weight--;
            search_node = search_node->right;
            index_node += (this->get_child_weight(search_node->left) + 1);
        } else {
            search_node->weight--;
            search_node = search_node->left;
            index_node -= (this->get_child_weight(search_node->right) + 1);
        }
    }
}

void bntree::erase(const string &key) {

    node_t *search_node = this->tree->root;
    uint64_t index_node = this->get_child_weight(search_node->left);

    for (;;) {
        int cmp = strcmp(key.data(), search_node->fstr());
        if (cmp==0 ) {
            if (this->erase_simple(search_node)) {
                // pass
            } else if (search_node->left && search_node->right) {
                // Самый сложный случай, удаляемый узел имеет 2-х детей.
                
                node_t *del_node = search_node;
                uint64_t _index;
                // Маленькая балансировка, если правый вес больше левого то удаляем через право
                // иначе через лево
                if (search_node->right->weight > search_node->left->weight) {
                    _index = index_node + 1;
                } else {
                    _index = index_node - 1;
                }

                // Ищем узел со следующим индексом.
                for (;;) {
                    if (_index == index_node) {
                        // Узел найден
                        break;
                    } else if (_index > index_node) {
                        search_node->weight--;
                        search_node = search_node->right;
                        index_node += (this->get_child_weight(search_node->left) + 1);
                    } else {
                        search_node->weight--;

                        search_node = search_node->left;
                        index_node -= (this->get_child_weight(search_node->right) + 1);
                    }
                }
/*
                del_node->data = search_node->data;
/*/
#ifndef NO_BD
                size_t del_idx = THeap.idx(del_node);
                size_t search_idx = THeap.idx(search_node);
                BDHeap[del_idx] = BDHeap[search_idx];
#else
                strcpy_s(del_node->str, search_node->str);

#endif
//*/
                this->erase_simple(search_node);
            }

            this->node_free(search_node);

            return;
        } else if (cmp>0) {
            search_node->weight--;
            search_node = search_node->right;
            index_node += (this->get_child_weight(search_node->left) + 1);
        } else {
            search_node->weight--;
            search_node = search_node->left;
            index_node -= (this->get_child_weight(search_node->right) + 1);
        }

    }
}

uint64_t bntree::get_child_weight(node_t *node) {
    if (node) {
        return node->weight;
    }

    return 0;
}

const char* bntree::get(uint64_t index) {
    if (index < 0 || index > this->size() - 1) {
        return NULL;
    }

    node_t *search_node = this->tree->root;
    uint64_t index_node = this->get_child_weight(search_node->left);

    for (;;) {
        if (index == index_node) {
            return search_node->fstr();
        } else if (index > index_node) {
            search_node = search_node->right;
            index_node += (this->get_child_weight(search_node->left) + 1);
        } else {
            search_node = search_node->left;
            index_node -= (this->get_child_weight(search_node->right) + 1);
        }
    }
}

const char* bntree::get(const string &key) {
    node_t *search_node = this->tree->root;
    uint64_t index_node = this->get_child_weight(search_node->left);

    for (;;) {
        int cmp = strcmp(key.data(), search_node->fstr());
        if (cmp==0) {
            return search_node->fstr();
        } else if (cmp>0) {
            search_node = search_node->right;
            index_node += (this->get_child_weight(search_node->left) + 1);
        } else {
            search_node = search_node->left;
            index_node -= (this->get_child_weight(search_node->right) + 1);
        }
    }

    return NULL;
}

uint64_t bntree::size() {
    if (this->tree->root) {
        return this->tree->root->weight;
    }

    return 0;
}

void bntree::print() {
    this->print(this->tree->root, 5);
}

void bntree::print(node_t *p, int indent) {
    if (p != NULL) {
        if (p->right) {
            this->print(p->right, indent + 4);
        }
        if (indent) {
            //std::cout << std::setw(indent) << ' ' << p->weight << ' ';
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right) {
            std::cout << " /\n" << std::setw(indent) << ' ';
        }
        //std::cout << p->data.key << ":" << p->data.val << "\n ";
        std::cout << p->fstr() << ":" << p << ":" << p->parent << "\n ";
        if (p->left) {
            std::cout << std::setw(indent) << ' ' << " \\\n";
            this->print(p->left, indent + 4);
        }
    }
}

inline
node_t *bntree::rotateleft(node_t *p) {
    node_t* child = p->right;
    node_t* parent = p->parent;

    if (parent) {
        if (parent->right == p) {
            parent->right = child;
        }
        else if (parent->left == p) {
            parent->left = child;
        }
    }
    else {
        this->tree->root = child;
    }
    child->parent = parent;
    p->parent = child;

    p->right = child->left;
    if (p->right) {
        p->right->parent = p;
    }

    child->left = p;

    p->weight = 1 + this->get_child_weight(p->left) + this->get_child_weight(p->right);
    child->weight = 1 + this->get_child_weight(child->left) + this->get_child_weight(child->right);

    p = child;
    return p;
}

inline
node_t* bntree::rotateright(node_t* p) {
    node_t* child = p->left;
    node_t* parent = p->parent;

    if (parent) {
        if (parent->right == p) {
            parent->right = child;
        }
        else if (parent->left == p) {
            parent->left = child;
        }
    }
    else {
        this->tree->root = child;
    }
    child->parent = parent;
    p->parent = child;

    p->left = child->right;
    if (p->left) {
        p->left->parent = p;
    }

    child->right = p;

    p->weight = 1 + this->get_child_weight(p->left) + this->get_child_weight(p->right);
    child->weight = 1 + this->get_child_weight(child->left) + this->get_child_weight(child->right);

    p = child;
    return p;
}

inline size_t getsize(node_t* p) {
    if (p) return p->weight;
    return 0;
}

/*
 * Балансировка поворотом
 * Балансировка делается на основе весов левого и правого поддерева
 */
void bntree::balance(node_t *p) {
    if (!p) {
        return;
    }

    node_t *child = NULL;
    node_t *parent = NULL;
    for (;;) {
        int loop = 7;
        do {
            if (p->left) {
                size_t R = getsize(p->right);
                size_t L = p->left->weight;
                if (p->left->left) {
                    if (R > L + L + L + (L >> 1)) {  // большой поворот
                        node_t* q = p->right;
                        if (q) {
                            if (q->left) {
                                size_t S = q->left->weight;
                                size_t D = getsize(q->right);
                                if (S > D + 1) {
                                    p->right = rotateright(p->right);
                                }
                            }
                            p = rotateleft(p);
                            continue;
                        }
                    }

                    size_t A = p->left->left->weight;
                    if (A > R + 1) {
                        p = rotateright(p);  // балансировка по массе, малый поворот
                        continue;
                    }

                    if (!p->left->right && L > R + 1) {
                        p = rotateright(p);  // заставляем ветвиться чтоб не выродилось в список
                        continue;
                    }
                }
            }
            if (p->right) {
                size_t L = getsize(p->left);
                size_t R = p->right->weight;
                if (p->right->right) {
                    if (L > R + R + R + (R >> 1)) {  // большой поворот
                        node_t* q = p->left;
                        if (q) {
                            if (q->right) {
                                size_t S = q->right->weight;
                                size_t D = getsize(q->left);
                                if (S > D + 1) {
                                    p->left = rotateleft(p->left);
                                }
                            }
                            p = rotateright(p);
                            continue;
                        }
                    }

                    size_t C = p->right->right->weight;
                    if (C > L + 1) {
                        p = rotateleft(p);     // балансировка по массе, малый поворот
                        continue;
                    }

                    if (!p->right->left && R > L + 1) {
                        p = rotateleft(p);   // заставляем ветвиться чтоб не выродилось в список
                        continue;
                    }
                }
            }
            break;
        } while (--loop);
/*
        if( L > R + R + R + (R >> 1) ){
            // Правый поворот. 
            // Глубина левого поддерева больше, чем глубина правого
            p = rotateright(p);
            //break;
        } else if( R > L + L + L + (L >>1) ){
                // Левый поворот. 
            // Глубина правого поддерева больше, чем глубина левого
            p = rotateleft(p);
            //break;
        }
*/
        if (p->parent) {
            p = p->parent;
        } else {
            break;
        }
    }

    return;
}

/*
 * Возвращает первое число в степени 2, которое больше или ровно x
 */
uint64_t bntree::cpl2(uint64_t x) {
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    x = x | (x >> 32);

    return x + 1;
}

/*
 * Двоичный логарифм от числа
 */
uint64_t bntree::ilog2(uint64_t d) {
    int result;
    std::frexp(d, &result);
    if (result > 1) {
        return result - 1;
    }
    return 0;
}

/*
 * Вес к глубине
 */
uint64_t bntree::weight_to_depth(node_t *p) {
    if (p == NULL) {
        return 0;
    }

    if (p->weight == 1) {
        return 1;
    } else if (p->weight == 2) {
        return 2;
    }

    return this->ilog2(this->cpl2(p->weight));
}
