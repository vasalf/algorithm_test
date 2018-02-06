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

#include <string>
#include <vector>

class avl_tree {
    struct node {
        node *L, *R;
        int h, sz;
        int x;
        node(int val) {
            h = sz = 1;
            L = R = nullptr;
            x = val;
        }
    };

    int node_height(node *v) {
        return v ? v->h : 0;
    }

    int node_sz(node *v) {
        return v ? v->sz : 0;
    }

    void update(node *v) {
        if (v) {
            v->h = std::max(node_height(v->L), node_height(v->R)) + 1;
            v->sz = node_sz(v->L) + node_sz(v->R) + 1;
        }
    }

    node *single_rotate_left(node *p, node *v) {
        p->R = v->L;
        v->L = p;
        update(p);
        update(v);
        return v;
    }

    node *single_rotate_right(node *p, node *v) {
        p->L = v->R;
        v->R = p;
        update(p);
        update(v);
        return v;
    }

    node *single_rotate(node *p, node *v) {
        if (p->L == v)
            return single_rotate_right(p, v);
        else
            return single_rotate_left(p, v);
    }

    node *double_rotate_left(node *g, node *p, node *v) {
        g->R = v->L;
        p->L = v->R;
        v->L = g;
        v->R = p;
        update(g);
        update(p);
        update(v);
        return v;
    }

    node *double_rotate_right(node *g, node *p, node *v) {
        g->L = v->R;
        p->R = v->L;
        v->L = p;
        v->R = g;
        update(g);
        update(p);
        update(v);
        return v;
    }

    node *double_rotate(node *g, node *p, node *v) {
        if (g->L == p)
            return double_rotate_right(g, p, v);
        else
            return double_rotate_left(g, p, v);
    }

    node *insert(node *v, int where, node *a) {
        if (!v)
            return a;
        if (where <= node_sz(v->L)) {
            v->L = insert(v->L, where, a);
            update(v);
            if (node_height(v->L) > node_height(v->R) + 1) {
                if (node_height(v->L->L) > node_height(v->L->R))
                    return single_rotate(v, v->L);
                else
                    return double_rotate(v, v->L, v->L->R);
            }
        } else {
            v->R = insert(v->R, where - node_sz(v->L) - 1, a);
            update(v);
            if (node_height(v->R) > node_height(v->L) + 1) {
                if (node_height(v->R->R) > node_height(v->R->L))
                    return single_rotate(v, v->R);
                else
                    return double_rotate(v, v->R, v->R->L);
            }
        }
        return v;
    }

    node *erase(node *v, int where) {
        if (where < node_sz(v->L)) {
            v->L = erase(v->L, where);
            update(v);
            if (node_height(v->R) > node_height(v->L) + 1) {
                if (node_height(v->R->R) > node_height(v->R->L))
                    return single_rotate(v, v->R);
                else
                    return double_rotate(v, v->R, v->R->L);
            }
            return v;
        }
        if (where == node_sz(v->L)) {
            if (!(v->L)) {
                node *ret = v->R;
                delete v;
                return ret;
            }
            if (!(v->R)) {
                node *ret = v->L;
                delete v;
                return ret;
            }
            node *u = v->R;
            while (u->L)
                u = u->L;
            std::swap(v->x, u->x);
            where = 0;
        } else {
            where -= node_sz(v->L) + 1;
        }
        v->R = erase(v->R, where);
        update(v);
        if (node_height(v->L) > node_height(v->R) + 1) {
            if (node_height(v->L->L) > node_height(v->L->R))
                return single_rotate(v, v->L);
            else
                return double_rotate(v, v->L, v->L->R);
        }
        return v;
    }

    void to_vector(node *v, std::vector<int>& w) {
        if (!v) return;
        to_vector(v->L, w);
        w.push_back(v->x);
        to_vector(v->R, w);
    }

    node *root = nullptr;

public:
    avl_tree() { }

    avl_tree(const avl_tree&) = delete;

    ~avl_tree() = default;

    void insert(int before, int value) {
        root = insert(root, before, new node(value));
    }

    void erase(int where) {
        root = erase(root, where);
    }

    int at(int index) {
        node *v = root;
        while (true) {
            if (index < node_sz(v->L))
                v = v->L;
            else if (index == node_sz(v->L))
                return v->x;
            else {
                index -= node_sz(v->L) + 1;
                v = v->R;
            }
        }
    }

    operator std::vector<int>() {
        std::vector<int> w;
        to_vector(root, w);
        return w;
    }

    static std::string name() { return "avl_tree"; }
};

