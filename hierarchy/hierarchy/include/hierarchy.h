#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <set>
#include <mutex>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

struct Node {
    string id;
    string name;
    string parent_id;
    Node *next; // sibling
    Node *child;
    Node() :
        id(""), name(""), parent_id(""), next(nullptr), child(nullptr) {}
    Node(const string& x, const string& y) :
        id(x), name(y), next(nullptr), child(nullptr)  {}
    Node(const string& x, const string& y, const string& z) :
        id(x), name(y), parent_id(z), next(nullptr), child(nullptr)  {}
    Node(const string& x, const string& y, const string& z, Node *next) :
        id(x), name(y), parent_id(z), next(next), child(nullptr)  {}
};

class hierarchy
{
public:
    json pass = json::parse(R"({"ok": true})");
    json fail = json::parse(R"({"ok": false})");
    Node *root = nullptr;
    mutex m_mutex;

    hierarchy() {}

    void add_node(string, string, string);
    void delete_node(string);
    void move_node(string, string);
    void query(int, int, vector<string>&, vector<string>&, vector<string>&);
    void preOrder(Node*, int);
    void find_root_id_node(Node *, Node **, string, int&);
    void prn_node();

private:
    std::set<string> m_names_set;
    std::set<string> m_ids_set;
    std::set<string> m_root_ids_set;
    int m_max_depth;
    int m_min_depth;
    json m_j_arr;
};

#endif
