#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <thread>
#include "nlohmann/json.hpp"
#include "hierarchy.h"

using json = nlohmann::json;
using namespace std;

void jsonDecodeProcess(hierarchy &h, json j) {
    h.m_mutex.lock();
    json::iterator it = j.begin();
    string input_fun = it.key();
    string id;
    string name;
    string parent_id;
    string new_parent_id;
    int min_depth = 0;
    int max_depth = INT_MAX;
    vector<string> names;
    vector<string> ids;
    vector<string> root_ids;

    if (input_fun == "add_node") {
        if (nullptr != j[input_fun]["id"])
            id = j[input_fun]["id"];
        if (nullptr != j[input_fun]["name"])
            name = j[input_fun]["name"];
        if (nullptr == j[input_fun]["parent_id"]) {
            return h.add_node(name, id, "");
        } else {
            parent_id = j[input_fun]["parent_id"];
            h.add_node(name, id, parent_id);
        }
    } else if (input_fun == "delete_node") {
        if (nullptr != j[input_fun]["id"])
            id = j[input_fun]["id"];
        h.delete_node(id);
    } else if (input_fun == "move_node") {
        if (nullptr != j[input_fun]["id"])
            id = j[input_fun]["id"];
        if (nullptr != j[input_fun]["new_parent_id"])
            new_parent_id = j[input_fun]["new_parent_id"];
        h.move_node(id, new_parent_id);
    } else if (input_fun == "query") {
        if (j[input_fun]["min_depth"] != nullptr)
            min_depth = j[input_fun]["min_depth"];
        if (j[input_fun]["max_depth"] != nullptr)
            max_depth = j[input_fun]["max_depth"];
        if (j[input_fun]["names"] != nullptr)
            j[input_fun].at("names").get_to(names);
        if (j[input_fun]["ids"] != nullptr)
            j[input_fun].at("ids").get_to(ids);
        if (j[input_fun]["root_ids"] != nullptr)
            j[input_fun].at("root_ids").get_to(root_ids);
        h.query(min_depth, max_depth, names, ids, root_ids);
    } else {
        std::cout << h.fail << std::endl;
    }
    h.m_mutex.unlock();
}

void hierarchy_test_self() {
    hierarchy h;

    auto j0 = json::parse(R"({"add_node":{"id":"1","name":"Root"}})");
    auto j1 = json::parse(R"({"add_node":{"id":"4","name":"Child42","parent_id":"1"}})");
    auto j2 = json::parse(R"({"add_node":{"id":"5","name":"Child42","parent_id":"1"}})");
    auto j3 = json::parse(R"({"add_node":{"id":"6","name":"Child78","parent_id":"200"}})");
    auto j4 = json::parse(R"({"add_node":{"id":"7","name":"Child87","parent_id":"4"}})");
    auto j5 = json::parse(R"({"add_node":{"id":"8","name":"Child12","parent_id":"4"}})");
    auto j6 = json::parse(R"({"add_node":{"id":"9","name":"Child42","parent_id":"4"}})");

    auto j7 = json::parse(R"({"delete_node":{"id":"8"}})");
    auto j8 = json::parse(R"({"delete_node":{"id":"7"}})");
    auto j9 = json::parse(R"({"delete_node":{"id":"4"}})");

    auto j10 = json::parse(R"({"add_node":{"parent_id":"1","id":"2","name":"A"}})");
    auto j11 = json::parse(R"({"add_node":{"parent_id":"1","id":"3","name":"B"}})");
    auto j12 = json::parse(R"({"move_node":{"id":"3","new_parent_id":"4"}})");
    auto j13 = json::parse(R"({"move_node":{"id":"4","new_parent_id":"3"}})");
    auto j14 = json::parse(R"({"move_node":{"id":"9","new_parent_id":"1"}})");
    auto j15 = json::parse(R"({"move_node":{"id":"9","new_parent_id":"7"}})");

    auto j16 = json::parse(R"({"query":{"names":["B"]}})");
    auto j17 = json::parse(R"({"query":{}})");
    auto j18 = json::parse(R"({"query":{"root_ids":["4"],"min_depth":1}})");

    auto j19 = json::parse(R"({"add_node":{"id":"","name":"aaa"}})");


    /* test add_node */
    cout << j0 << endl;
    jsonDecodeProcess(h, j0);
    h.prn_node();
    cout << j0 << endl;
    jsonDecodeProcess(h, j0); // No second root
    h.prn_node();
    cout << j1 << endl;
    jsonDecodeProcess(h, j1);
    h.prn_node();
    cout << j2 << endl;
    jsonDecodeProcess(h, j2); // sibling can not have same name
    h.prn_node();
    cout << j3 << endl;
    jsonDecodeProcess(h, j3); // Add a child node to nonexistent parent.
    h.prn_node();
    cout << j4 << endl;
    jsonDecodeProcess(h, j4);
    h.prn_node();
    cout << j5 << endl;
    jsonDecodeProcess(h, j5);
    h.prn_node();
    cout << j6 << endl;
    jsonDecodeProcess(h, j6);
    h.prn_node();

    /* test delete_node */
    cout << j7 << endl;
    jsonDecodeProcess(h, j7); // delete middle child
    h.prn_node();
    cout << j8 << endl;
    jsonDecodeProcess(h, j8); // delete first child
    h.prn_node();
    cout << j8 << endl;
    jsonDecodeProcess(h, j8); // delete non-exist
    h.prn_node();
    cout << j9 << endl;
    jsonDecodeProcess(h, j9); // delete node who have child
    h.prn_node();

    /* test move_node */
    cout << j10 << endl;
    jsonDecodeProcess(h, j10);
    h.prn_node();
    cout << j11 << endl;
    jsonDecodeProcess(h, j11);
    h.prn_node();
    cout << j12 << endl;
    jsonDecodeProcess(h, j12);
    h.prn_node();
    cout << j13 << endl;
    jsonDecodeProcess(h, j13); // cycle test
    h.prn_node();
    cout << j14 << endl;
    jsonDecodeProcess(h, j14); // same name test
    h.prn_node();
    cout << j15 << endl;
    jsonDecodeProcess(h, j15); // non-exist parent id
    h.prn_node();

    /* test query */
    cout << j16 << endl;
    jsonDecodeProcess(h, j16);
    cout << j17 << endl;
    jsonDecodeProcess(h, j17);
    cout << j18 << endl;
    jsonDecodeProcess(h, j18); // root_ids with min_depth

    cout << j19 << endl; // add node no id
    jsonDecodeProcess(h, j19);
    h.prn_node();

#if 0
    std::cout << "json j0 = " << j0 << std::endl;
    std::cout << j0.dump(4) << std::endl;

    for (auto& [key, value] : j0.items()) {
        std::cout << key << " : " << value << "\n";
    }
    json::iterator it = j0.begin();
    std::cout << it.key() << "\n";
#endif

}

void hierarchy_test() {

    hierarchy h;
    while (cin){
        json j;
        std::cin >> j;
        jsonDecodeProcess(h, j);
    }
}

int main()
{
    thread th_hierarchy(hierarchy_test);
    th_hierarchy.join();

    return 0;
}

