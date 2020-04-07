/*
All submissions for this problem are available.Given a tree with n nodes. 
Split it into 3 parts such that all the three parts are connected components in original graph and each node is 
exactly present in one of the parts and the following function is minimised: Let s1,s2,s3 be the sizes of the 3 
parts (or connected components) then 
f=|s1−s2|+|s2−s3|+|s1−s3|

Also calculate the number of ways of splitting the tree. Two ways are considered different if different sets of edges is removed.
*/
#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <memory>
#include <list>
#include <algorithm>
#include <stack>
#include <fstream>

using namespace std;

class Node {
    public:
        Node(int i) : info(i) { }
        int info;
        int totalChildren = 0;
        shared_ptr<Node> parent;
        list<shared_ptr<Node>> children;
        friend ostream& operator << (ostream& os, const Node& node) {
            os << "id: " << node.info << " tc: " << node.totalChildren << endl;
            return os;
        }
        friend bool operator ==(const Node& node, const Node& other) {
            return node.info == other.info;
        }
        friend bool operator ==(const shared_ptr<Node>& node, const shared_ptr<Node>& other) {
            return node->info == other->info;
        }
};

class Tree {
    public:
        shared_ptr<Node> root;
        void add(int src, int dest) {
            shared_ptr<Node> parent = make_shared<Node>(src);
            shared_ptr<Node> child = make_shared<Node>(dest);
            
            if (root == nullptr) {
                parent->totalChildren++;
                parent->children.push_back(child);
                root = parent;
                return;
            }
            
            shared_ptr<Node> found = find(root, parent);
            if (found) {
                child->parent = found;
                found->totalChildren++;
                found->children.push_back(child);
            }
        }

        int countChildren() {
            return calc(root);
        }

        void print() {
            print(root);
        }
    private:
        shared_ptr<Node> find(shared_ptr<Node>& root, const shared_ptr<Node>& toFind) {
            if (root != nullptr) {
                for(auto& child : root->children) {
                    if (child == toFind) {
                        return child;
                    }
                    return find(child, toFind);
                }
            }
            return nullptr;
        }

        void print(shared_ptr<Node> root) {
            if (root != nullptr) {
                for(auto& child:root->children) {
                    cout << *child << "," << endl;
                    print(child);
                }
            }
        }

        int calc(shared_ptr<Node> root) {
            if (root != nullptr) {
                for(auto& child : root->children) {
                    root->totalChildren = calc(child) + 1;
                    return root->totalChildren;
                }
            } else {
                root->totalChildren = 0;
                return 0;
            }
        }
};

int a[3];

int max_children(const list<shared_ptr<Node>>& children) {
    int max = 0;
    for (auto& child:children) {
        if (max < child->totalChildren) {
            max = child->totalChildren;
        }
    }
    return max;
}

shared_ptr<Node> max_children_node(const list<shared_ptr<Node>>& children) {
    int max = 0;
    shared_ptr<Node> n;
    for (auto& child:children) {
        if (max < child->totalChildren) {
            max = child->totalChildren;
            n = child;
        }
    }
    return n;
}

void findBreakingNode(shared_ptr<Node> parent, int optimum, int& i) {
    if (parent != nullptr) {
        if (parent->totalChildren <= optimum) {
            int reduceChildren;
            if (parent->children.size() == 1) {
                reduceChildren = parent->totalChildren;
            } else {
                if (parent->totalChildren == optimum) {
                    reduceChildren = parent->totalChildren + 1;
                    parent = parent->parent;
                } else {
                    reduceChildren = max_children(parent->children);
                }
            }
            auto& curr = parent;
            while (curr != nullptr) {
                curr->totalChildren -= reduceChildren;
                curr = curr->parent;
            }
            i = reduceChildren;
        } else {
            auto&& maxNode = max_children_node(parent->children);
            findBreakingNode(maxNode, optimum, i);
        }
    }
}

void findBreakingNode_2(shared_ptr<Node> parent, int optimum, int& i) {
    stack<shared_ptr<Node>> nodeStack;
}

int splitTree(const Tree& tree) {
    int total = tree.root->totalChildren + 1;
    int optimum = total / 3;
    int i = 0;
    auto&& maxNode = max_children_node(tree.root->children);
    findBreakingNode(maxNode, optimum, a[0]);
    total = tree.root->totalChildren + 1;
    optimum = total / 2;
    maxNode = max_children_node(tree.root->children);
    findBreakingNode(maxNode, optimum, a[1]);
    a[2] = tree.root->totalChildren + 1;
}

int main() {
    string nVertices;
    stringstream ss;
    vector<pair<int, int>> edges;
    ifstream fin;

    fin.open("tree-divide.txt");
    getline(fin, nVertices);
    ss << nVertices;
    int v;
    ss >> v;
    for (int i = 0; i < v; ++i) {
        string line;
        stringstream ss;
        getline(fin, line);
        ss << line;
        int t1, t2;
        ss >> t1 >> t2;
        pair<int,int> p {t1, t2};
        edges.push_back(p);
    }

    Tree tree;
    for(auto& edge:edges) {
        tree.add(edge.first, edge.second);
    }
    
    //tree.countChildren();
    tree.print();
}

