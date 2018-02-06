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
#include <queue>
#include <algorithm>
#include <cassert>

class splay_tree {
    struct node {
        node *L, *R, *par;
        int x, sz;
        node(int val) {
            x = val;
            sz = 1;
            L = nullptr;
            R = nullptr;
            par = nullptr;
        }
    };

    int node_sz(node *v) {
        if (v)
            return v->sz;
        else
            return 0;
    }

    void update(node *v) {
        if (v) {
            v->sz = node_sz(v->L) + node_sz(v->R) + 1;
        }
    }

    void rotate_left(node *p, node *v) {
        if (p->par)
            (p->par->L == p ? p->par->L : p->par->R) = v;
        v->par = p->par;
        p->R = v->L;
        if (p->R) p->R->par = p;
        v->L = p;
        p->par = v;
        update(p);
        update(v);
    }

    void rotate_right(node *p, node *v) {
        if (p->par)
            (p->par->L == p ? p->par->L : p->par->R) = v;
        v->par = p->par;
        p->L = v->R;
        if (p->L) p->L->par = p;
        v->R = p;
        p->par = v;
        update(p);
        update(v);
    }

    void single_rotate(node *p, node *v) {
        if (p->L == v)
            rotate_right(p, v);
        else
            rotate_left(p, v);
    }

    void splay(node *v) {
        while (v->par != nullptr) {
            if (v->par->par == nullptr) {
                single_rotate(v->par, v);
            } else if (v->par->L == v && v->par->par->L == v->par
                       || v->par->R == v && v->par->par->R == v->par) {
                single_rotate(v->par->par, v->par);
                single_rotate(v->par, v);
            } else {
                single_rotate(v->par, v);
                single_rotate(v->par, v);
            }
        }
        root = v;
    }

    node *root = nullptr;

    node *find(int before) {
        node *v = root;
        while (true) {
            if (before < node_sz(v->L))
                v = v->L;
            else if (before == node_sz(v->L))
                return v;
            else {
                before -= node_sz(v->L) + 1;
                v = v->R;
            }
        }
    }

public:
    splay_tree() { }

    splay_tree(const splay_tree&) = delete;

    ~splay_tree() {
        if (root) {
            std::queue<node *> dq;
            dq.push(root);
            while (!dq.empty()) {
                node *v = dq.front();
                dq.pop();
                if (v->L) dq.push(v->L);
                if (v->R) dq.push(v->R);
                delete v;
            }
        }
    }

    void insert(int before, int value) {
        node *a = new node(value);
        if (root == nullptr) {
            root = a;
            return;
        }
        node *v = root;
        while (true) {
            if (node_sz(v->L) >= before) {
                if (v->L == nullptr) {
                    v->L = a;
                    a->par = v;
                    break;
                } else
                    v = v->L;
            } else {
                before -= node_sz(v->L) + 1;
                if (v->R == nullptr) {
                    v->R = a;
                    a->par = v;
                    break;
                } else
                    v = v->R;
            }
        }
        splay(a);
    }

    void erase(int where) {
        node *v = find(where);
        if (!(v->L)) {
            if (v->par)
                (v->par->L == v ? v->par->L : v->par->R) = v->R;
            if (v->R)
                v->R->par = v->par;
            if (v == root)
                root = v->R;
            if (v->par)
                splay(v->par);
            delete v;
            return;
        }
        if (!(v->R)) {
            if (v->par)
                (v->par->L == v ? v->par->L : v->par->R) = v->L;
            if (v->L)
                v->L->par = v->par;
            if (v == root)
                root = v->L;
            if (v->par)
                splay(v->par);
            delete v;
            return;
        }
        node *u = v->R;
        while (u->L) u = u->L;
        std::swap(v->x, u->x);
        erase(where + 1);
    }

    int at(int index) {
        node *v = find(index);
        splay(v);
        return v->x;
    }

    operator std::vector<int>() {
        std::vector<int> ret;
        node *v = root;
        int last = 0;
        while (v) {
            if (last == 0) {
                if (v->L) {
                    v = v->L;
                } else if (v->R) {
                    ret.push_back(v->x);
                    v = v->R;
                } else {
                    ret.push_back(v->x);
                    last = (v->par->L == v ? 1 : 2);
                    v = v->par;
                }
            } else if (last == 1) {
                ret.push_back(v->x);
                if (v->R) {
                    last = 0;
                    v = v->R;
                } else if (v->par) {
                    last = (v->par->L == v ? 1 : 2);
                    v = v->par;
                } else {
                    v = v->par;
                }
            } else {
                if (v->par) {
                    last = (v->par->L == v ? 1 : 2);
                    v = v->par;
                } else {
                    v = v->par;
                }
            }
        }
        return ret;
    }

    static std::string name() { return "splay"; }
};