// -*- mode: c++; -*-
/*
 * Copyright (c) 2017-2018 Vasily Alferov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstdlib>
#include <string>
#include <random>

struct c_rnd_eng {
    c_rnd_eng(int seed) {
        std::srand(seed);
    }
    int operator()() {
        return std::rand();
    }
};

template<class T>
std::string rnd_eng_name() {
    return "";
}

template<>
std::string rnd_eng_name<c_rnd_eng> () {
    return "c_rnd";
}

template<>
std::string rnd_eng_name<std::mt19937>() {
    return "mt19937";
}

template<class random_eng>
class olymp_treap {
protected:
    random_eng rnd;
    struct node {
        int x, y;
        int sz;
        node *L, *R;
        node(int _x, random_eng& eng) {
            x = _x;
            y = eng();
            sz = 1;
            L = R = nullptr;
        }
    };
    static int node_sz(node *v) {
        if (!v) return 0;
        return v->sz;
    }
    static void update(node *v) {
        if (v) {
            v->sz = 1 + node_sz(v->L) + node_sz(v->R);
        }
    }
    static void split(node *v, int skip, node*& left, node*& right) {
        if (!v) {
            left = right = nullptr;
            return;
        }
        if (node_sz(v->L) >= skip) {
            split(v->L, skip, left, v->L);
            update(v);
            right = v;
        } else {
            split(v->R, skip - node_sz(v->L) - 1, v->R, right);
            update(v);
            left = v;
        }
    }
    static node *merge(node *left, node *right) {
        if (!left)
            return right;
        if (!right)
            return left;
        if (left->y < right->y) {
            left->R = merge(left->R, right);
            update(left);
            return left;
        } else {
            right->L = merge(left, right->L);
            update(right);
            return right;
        }
    }
    node *get(node *v, int at) {
        if (at < node_sz(v->L)) {
            return get(v->L, at);
        } else if (at == node_sz(v->L)) {
            return v;
        } else {
            return get(v->R, at - node_sz(v->L) - 1);
        }
    }
    void del(node *mem) {
        if (!mem) return;
        del(mem->L);
        del(mem->R);
        delete mem;
    }
    node *root = nullptr;
    olymp_treap(node *_root) : olymp_treap() {
        root = _root;
    }
    void to_vector(node *v, std::vector<int>& w) {
        if (!v) return;
        to_vector(v->L, w);
        w.push_back(v->x);
        to_vector(v->R, w);
    }
public:
    olymp_treap()
        : rnd(179)
    {};
    olymp_treap(const olymp_treap& other) = delete;
    virtual void insert(int before, int value) {
        node *left, *mid, *right;
        split(root, before, left, right);
        mid = new node(value, rnd);
        right = merge(mid, right);
        root = merge(left, right);
    }
    virtual void erase(int which) {
        node *left, *mid, *right;
        split(root, which, left, right);
        split(right, 1, mid, right);
        delete mid;
        root = merge(left, right);
    }
    std::pair<olymp_treap<random_eng>, olymp_treap<random_eng> > split(int left) {
        node *lt, *rt;
        split(root, left, lt, rt);
        return std::make_pair(olymp_treap(lt), olymp_treap(rt));
    };
    static olymp_treap<random_eng>&& merge(const olymp_treap<random_eng>&& lt, const olymp_treap<random_eng>&& rt) {
        node *root = merge(lt.root, rt.root);
        return olymp_treap(root);
    }
    static std::string name() {
        return "olymp_treap<" + rnd_eng_name<random_eng>() + ">";
    }
    virtual ~olymp_treap() {
        del(root);
    }
    int at(int i) {
        return get(root, i)->x;
    }
    operator std::vector<int>() {
        std::vector<int> ret;
        to_vector(root, ret);
        return ret;
    }
};

template<class random_eng>
class opt_treap : public olymp_treap<random_eng> {
protected:
    typedef typename olymp_treap<random_eng>::node node;
    virtual node *add(node *v, int after, node *to_add) {
        if (v == nullptr)
            return to_add;
        else if (to_add->y < v->y) {
            olymp_treap<random_eng>::split(v, after, to_add->L, to_add->R);
            olymp_treap<random_eng>::update(to_add);
            return to_add;
        } else if (olymp_treap<random_eng>::node_sz(v->L) >= after) {
            v->L = add(v->L, after, to_add);
            olymp_treap<random_eng>::update(v);
        } else {
            v->R = add(v->R, after - olymp_treap<random_eng>::node_sz(v->L) - 1, to_add);
            olymp_treap<random_eng>::update(v);
        }
        return v;
    }
    virtual node *remove(node *v, int which) {
        if (which == olymp_treap<random_eng>::node_sz(v->L))
            return olymp_treap<random_eng>::merge(v->L, v->R);
        else if (which < olymp_treap<random_eng>::node_sz(v->L)) {
            v->L = remove(v->L, which);
            olymp_treap<random_eng>::update(v);
        } else {
            v->R = remove(v->R, which - olymp_treap<random_eng>::node_sz(v->L) - 1);
            olymp_treap<random_eng>::update(v);
        }
        return v;
    }
    opt_treap(node *_root) : olymp_treap<random_eng>(_root) { }
public:
    opt_treap() : olymp_treap<random_eng>() { }
    virtual void insert(int before, int value) {
        olymp_treap<random_eng>::root = add(olymp_treap<random_eng>::root, before, new node(value, olymp_treap<random_eng>::rnd));
    }
    virtual void erase(int which) {
        olymp_treap<random_eng>::root = remove(olymp_treap<random_eng>::root, which);
    }
    static std::string name() {
        return "opt_treap<" + rnd_eng_name<random_eng>() + ">";
    }
};

template<class random_eng>
class nr_treap : public opt_treap<random_eng> {
protected:
    typedef typename olymp_treap<random_eng>::node node;
    virtual node *add(node *v, int after, node *to_add) {
        if (v == nullptr)
            return to_add;
        if (to_add->y < v->y) {
            olymp_treap<random_eng>::split(v, after, to_add->L, to_add->R);
            olymp_treap<random_eng>::update(to_add);
            return to_add;
        }
        node *ret = v;
        while (true) {
            v->sz++;
            if (olymp_treap<random_eng>::node_sz(v->L) >= after) {
                if (v->L == nullptr) {
                    v->L = to_add;
                    break;
                } else if (to_add->y < v->L->y) {
                    olymp_treap<random_eng>::split(v->L, after, to_add->L, to_add->R);
                    olymp_treap<random_eng>::update(to_add);
                    v->L = to_add;
                    break;
                } else {
                    v = v->L;
                }
            } else {
                after -= olymp_treap<random_eng>::node_sz(v->L) + 1;
                if (v->R == nullptr) {
                    v->R = to_add;
                    break;
                } else if (to_add->y < v->R->y) {
                    olymp_treap<random_eng>::split(v->R, after, to_add->L, to_add->R);
                    olymp_treap<random_eng>::update(to_add);
                    v->R = to_add;
                    break;
                } else {
                    v = v->R;
                }
            }
        }
        return ret;
    }
    virtual node *remove(node *v, int which) {
        if (which == olymp_treap<random_eng>::node_sz(v->L)) {
            return olymp_treap<random_eng>::merge(v->L, v->R);
        }
        node *ret = v;
        while (true) {
            v->sz--;
            if (which < olymp_treap<random_eng>::node_sz(v->L)) {
                if (which == olymp_treap<random_eng>::node_sz(v->L->L)) {
                    v->L = olymp_treap<random_eng>::merge(v->L->L, v->L->R);
                    break;
                } else {
                    v = v->L;
                }
            } else {
                which -= olymp_treap<random_eng>::node_sz(v->L) + 1;
                if (which == olymp_treap<random_eng>::node_sz(v->R->L)) {
                    v->R = olymp_treap<random_eng>::merge(v->R->L, v->R->R);
                    break;
                } else {
                    v = v->R;
                }
            }
        }
        return ret;
    }
public:
    static std::string name() {
        return "nr_treap<" + rnd_eng_name<random_eng>() + ">";
    }
};