/*
All submissions for this problem are available.Given a tree with n nodes. 
Split it into 3 parts such that all the three parts are connected components in original graph and each node is 
exactly present in one of the parts and the following function is minimised: Let s1,s2,s3 be the sizes of the 3 
parts (or connected components) then 
f=|s1−s2|+|s2−s3|+|s1−s3|

Also calculate the number of ways of splitting the tree. Two ways are considered different if different sets of edges is removed.
*/
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <array>
#include <cmath>
#include <set>

using namespace std;

class Node {
    public:
        Node(int i) : info(i) { }
        int info;
        int totalChildren = 0, nextChildren = 0;
        bool disabled = false, visited = false;
        shared_ptr<Node> parent;
        list<shared_ptr<Node>> children;
        friend ostream& operator << (ostream& os, const Node& node) {
            os << "id: " << node.info << " visited: " << node.visited << " tc: " << node.totalChildren << " ic: " << node.nextChildren << " children: " 
            << node.children.size() << " child-ids: ";
            for (auto& child : node.children) {
                os << child->info << ",";
            }
            os << endl;
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
            shared_ptr<Node> parent(new Node(src));
            shared_ptr<Node> child(new Node(dest));
            
            if (root == nullptr) {
                parent->children.push_back(child);
                root = parent;
                child->parent = parent;
                return;
            }
            
            shared_ptr<Node> found = find(root, parent);
            if (found) {
                child->parent = found;
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
                if (root->info == toFind->info) {
                    return root;
                }
                for(auto& child : root->children) {
                    auto ptr = find(child, toFind);
                    if (ptr) {
                        return ptr;
                    }
                }
            }
            return nullptr;
        }
        
        void print(shared_ptr<Node> root) {
            if (root != nullptr) {
                cout << *root << endl;
                for(auto& child:root->children) {
                    print(child);
                }
            }
        }
        

        int calc(shared_ptr<Node> root) {
            if (root != nullptr) {
                for(auto& child : root->children) {
                    root->totalChildren += calc(child);
                    root->nextChildren++;
                }
                return root->totalChildren + 1;
            } else {
                root->totalChildren = 0;
                return 0;
            }
        }
};

void print(shared_ptr<Node> root) {
    if (root != nullptr) {
        cout << *root << endl;
        for(auto& child:root->children) {
            print(child);
        }
    }
}

vector<shared_ptr<Node>> divide(shared_ptr<Node>& rootPtr, int sum, int divider);
vector<array<int, 3>> resp;
//set<array<shared_ptr<Node>, 3>> respNodes;
vector<array<shared_ptr<Node>, 3>> respNodes;

void divideIntoTwo(shared_ptr<Node>& divNode) {
    int val1 = divNode->totalChildren + 1;
    cout << "handle " << *divNode << endl;
    shared_ptr<Node> divNodeParent = divNode->parent;

    auto childList = divNodeParent->children;

    //remove this child node and keep its distance from beginning to add later on at same place.
    auto it = childList.begin();
    int dist = 0;
    for( ; it != childList.end(); ++it) {
        if ((*it)->info == divNode->info) {
            (*it)->disabled = true;
            break;
        }
        dist++;
    }
        
    //update all nodes till parent with updated children.
    auto earstwhileParentPtr(divNodeParent);
    auto iterToParent = divNodeParent;
    decltype(iterToParent) iterToRoot = nullptr;
    divNodeParent->nextChildren--;
    while(iterToParent) {
        iterToParent->totalChildren -= divNode->totalChildren + 1;
        iterToRoot = iterToParent;
        iterToParent = iterToParent->parent;
    }

    //save a copy to detached node.
    shared_ptr<Node> copyToDetachedNode(divNode);

    //optimally divide the tree into 2 nodes now.
    int div = (iterToRoot->totalChildren + 1) / 2;
    auto subdividedNodes = divide(iterToRoot, div, 2);
    int val2, val3;
    cout << "subnodes " << endl;
    for(auto& subdividedNode : subdividedNodes) {
        val2 = subdividedNode->totalChildren + 1;
        val3 = iterToRoot->totalChildren - subdividedNode->totalChildren;
        if (val3 == 0) {
          continue;
        }
        array<shared_ptr<Node>, 3> nodeArr{copyToDetachedNode, subdividedNode, iterToRoot};
        respNodes.push_back(nodeArr);
        array<int, 3> arr{val1, val2, val3};
        resp.push_back(arr);
        cout << *subdividedNode;
        cout << val1 << " " << val2 << " " << val3 << endl;
    }
    
    //re-create the tree by attching detached node to its earstwhile parent.
    iterToParent = earstwhileParentPtr;
    iterToRoot = nullptr;
    
    while(iterToParent) {
        iterToParent->totalChildren += copyToDetachedNode->totalChildren + 1;
        iterToRoot = iterToParent;
        iterToParent = iterToParent->parent;
    }

    it = childList.begin();
    (*it)->disabled = false;

    divNodeParent->nextChildren++;
}

vector<shared_ptr<Node>> divide(shared_ptr<Node>& rootPtr, int sum, int divider) {
    vector<shared_ptr<Node>> optimalNodes{};
    bool hasInnerOptimal = false;
    if (rootPtr && !rootPtr->disabled && !rootPtr->visited) {
        if (rootPtr->totalChildren > sum || (rootPtr->nextChildren == 1 && rootPtr->totalChildren >= sum)) {
            auto it = rootPtr->children.begin();
              for(auto& child : rootPtr->children) {
                if (child->totalChildren > sum) {
                    hasInnerOptimal = true;
                } 
                cout << "sum " << sum << " before child " << *child << endl;
                if (child->visited) {
                    continue;
                }
                auto&& vec = divide(child, sum, divider);
                int size = vec.size() + optimalNodes.size();
                move(vec.begin(), vec.end(), back_inserter(optimalNodes));
            }
            //if all children of the node are smaller than the cutoff then include parent too as an optimal subtree.
            if ((!hasInnerOptimal)) {
                optimalNodes.push_back(rootPtr);
                //this point divide the existing tree into more parts.
                if (divider == 3) {
                    divideIntoTwo(rootPtr);
                    rootPtr->visited = true;
                    cout << "mark true here " << *rootPtr << endl;
                }
            }
            
            return optimalNodes;
        } else {
            //child with cutoff subchildren has reached.
            optimalNodes.push_back(rootPtr);
            //this point divide the existing tree into more parts.
            if (divider == 3) {
                divideIntoTwo(rootPtr);
                rootPtr->visited = true;
                cout << "mark true there " << *rootPtr << endl;
            }
            return optimalNodes;
        }
    }
    return optimalNodes;
}

void divideIntoThree(Tree& tree) {
    int div = (tree.root->totalChildren + 1) / 3;
    auto nodes = divide(tree.root, div, 3);

    cout << "optimal possible subtrees" << endl;
    int id = 0;
    for(auto& p : resp) {
        cout << ++id << endl;
        cout << p[0] << " " << p[1] << " " << p[2] << endl;
    }
    id = 0;
    for(auto& p : respNodes) {
        cout << ++id << endl;
        cout << *p[0] << *p[1] << *p[2] << endl;
    }

    if (resp.size() == 0) {
        return;
    }

    //find smallest of these subtrees.
    auto p = resp[0];
    int min = abs(p[0]-p[1]) + abs(p[1]-p[2]) + abs(p[2]-p[0]);
    int count = 0;
    
    for(int i = 1; i < resp.size(); ++i) {
        auto q = resp[i];
        int curr = abs(q[0]-q[1]) + abs(q[1]-q[2]) + abs(q[2]-q[0]);
        if(curr <= min) {
            min = curr;
        } 
    }
    for(int i = 0; i < resp.size(); ++i) {
        auto q = resp[i];
        int curr = abs(q[0]-q[1]) + abs(q[1]-q[2]) + abs(q[2]-q[0]);
        if(curr == min) {
            count++;
        } 
    }
    cout << "min " << min << " count " << count << endl;
}

int main() {
    int nTests;
    int nEdges;
    int src, dest;
    stringstream ss;
    string list;

    fstream fin("a.txt");
    getline(fin, list);
    ss << list;
    ss >> nTests;
    ss.clear();

    for (int i = 0; i < nTests; i++) {
        getline(fin, list);
        ss << list;
        ss >> nEdges;
        ss.clear();
        vector<pair<int, int>> edges;

        for (int j = 0; j < nEdges; j++) {
            getline(fin, list);
            ss << list;
            ss >> src >> dest;
            ss.clear();

            pair<int,int> p {src, dest};
            edges.push_back(p);
        }

        Tree tree;
        for(auto& edge:edges) {
            tree.add(edge.first, edge.second);
        }
        tree.countChildren();
        tree.print();
        divideIntoThree(tree);
        vector<array<int, 3>>().swap(resp);
        respNodes.clear();
    }
    return 0;
}
