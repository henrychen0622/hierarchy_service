#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <set>
#include "nlohmann/json.hpp"
#include "hierarchy.h"

using json = nlohmann::json;
using namespace std;

/*
 * Description: Add a new node to the tree.
 *
 * Params:
 *    - name      {string}: Node name
 *    - id        {string}: Node ID
 *    - parent_id {string}: ID of the parent node; if ommitted or empty string,
 *                          add this node as the root node (assuming there isn't
 *                          already a root node)
 *
 * Validation:
 *    - Two sibling nodes cannot have the same name.
 *    - No two nodes in the tree can have the same ID.
 *    - There can only be one root node (i.e., a node without a parent).
 *    - Name and ID must be specified and not empty strings.
 *    - If specified, parent node must exist.
 */
void hierarchy::add_node(string name, string id, string parent_id) {
    /* Name and ID must be specified and not empty strings. */
    if (id == "" || name == "") {
        std::cout << fail << std::endl;
        return;
    }

    /* There can only be one root node */
    if (parent_id == "") {
        if (root != nullptr) {
            std::cout << fail << std::endl;
            return;
        } else {
            root = new Node(id, name);
            std::cout << pass << std::endl;
            return;
        }
    }

    /* Inorder Traversal */
    stack<Node *> s;
    Node *cur = root;
    Node *parent = nullptr;
    while (cur || !s.empty()) {
        if (cur) {
            s.push(cur);
            cur = cur->child;
        } else {
            Node *node = s.top();
            s.pop();
            /* unique id, siblings cannot have the same name */
            if ((node->id == id) ||
                 (node->parent_id == parent_id && node->name == name))
            {
                std::cout << fail << std::endl;
                return;
            }
            if (node->id == parent_id)
                parent = node;
            cur = node->next;
        }
    }

    /* parent node must exist */
    if (nullptr == parent) {
        std::cout << fail << std::endl;
        return;
    }

    /* check if it is the first child */
    if (nullptr == parent->child) {
        parent->child = new Node(id, name, parent_id);
        std::cout << pass << std::endl;
        return;
    } else {
        cur = parent->child;
        if (cur->name > name) {
            parent->child = new Node(id, name, parent_id);
            parent->child->next = cur;
            std::cout << pass << std::endl;
            return;
        }
        while (cur->next && cur->next->name <= name) {
            cur = cur->next;
        }
        Node *next = cur->next;
        cur->next = new Node(id, name, parent_id);
        cur->next->next = next;
        std::cout << pass << std::endl;
        return;
    }

    cout << "it should never be printed !" << endl;
    return;
}

/*
 * Description: Delete a node from the tree.
 *
 * Params:
 *    - id {string}: ID of node to delete
 *
 * Validation:
 *    - ID must be specified and not an empty string.
 *    - Node must exist.
 *    - Node must not have children.
 */
void hierarchy::delete_node(string id) {
    /* ID must be specified and not empty strings. */
    if (id == "") {
        std::cout << fail << std::endl;
        return;
    }

    /* Inorder Traversal */
    stack<Node *> s;
    Node *cur = root;
    Node *parent = nullptr;
    while (cur || !s.empty()) {
        if (cur) {
            s.push(cur);
            cur = cur->child;
        } else {
            Node *node = s.top();
            s.pop();
            if (node->next && node->next->id == id) {
                /* Node must not have children. */
                if (nullptr != node->next->child) {
                    std::cout << fail << std::endl;
                    return;
                }
                Node *next = node->next;
                node->next = node->next->next;
                delete next;
                std::cout << pass << std::endl;
                return;
            }
            if (node->id == id) {
                /* Node must not have children. */
                if (nullptr != node->child) {
                    std::cout << fail << std::endl;
                    return;
                }
                parent = s.top();
                parent->child = node->next;
                node->next = nullptr;
                delete node;
                std::cout << pass << std::endl;
                return;
            }
            cur = node->next;
        }
    }

    /* Node must exist. */
    std::cout << fail << std::endl;
}


/*
 * Description: Move a node to a new parent in the tree
 *
 * Params:
 *    - id {string}:            ID of node to move
 *    - new_parent_id {string}: ID of the new parent node
 *
 * Validation:
 *    - ID and new parent ID must be specified and not empty strings.
 *    - Both nodes must exist.
 *    - The name of the node to be moved must not be the same as those of any of
 *      the new parent's other children.
 *    - Move must not create a cycle in the tree.
 */
void hierarchy::move_node(string id, string new_parent_id) {
    /* ID and new parent ID must be specified and not empty strings. */
    if (id == "" || new_parent_id == "" || id == root->id ||
        id == new_parent_id) {
        std::cout << fail << std::endl;
        return;
    }

    stack<Node *> s;
    stack<Node *> ss;
    s.push(root);
    Node *parent_tmp = root;
    Node *parent = nullptr;
    Node *prev = nullptr;
    Node *child = nullptr;
    Node *new_parent = nullptr;
    int depth = 0;

    find_root_id_node(root, &new_parent, new_parent_id, depth);
    if (nullptr == new_parent) {
        std::cout << fail << std::endl;
        return;
    }

    /* Preorder Traversal */
    while (s.size() > 0) {
        Node *node = s.top();
        s.pop();

        /* Move must not create a cycle in the tree. */
        if (!child && node->next && node->next->id == id) {
            prev = node;
            child = node->next;
            if (child->child) {
                ss.push(child->child);
                while (ss.size() > 0) {
                    Node *node_tmp = ss.top();
                    ss.pop();
                    if (node_tmp->id == new_parent_id) {
                        std::cout << fail << std::endl;
                        return;
                    }
                    if (node_tmp->child) ss.push(node_tmp->child);
                    if (node_tmp->next) ss.push(node_tmp->next);
                }
            }
            break;
        }

        /* Move must not create a cycle in the tree. */
        if (!child && node->id == id) {
            parent = parent_tmp;
            child = node;
            if (child->child) {
                ss.push(child->child);
                while (ss.size() > 0) {
                    Node *node_tmp = ss.top();
                    ss.pop();
                    if (node_tmp->id == new_parent_id) {
                        std::cout << fail << std::endl;
                        return;
                    }
                    if (node_tmp->child) ss.push(node_tmp->child);
                    if (node_tmp->next) ss.push(node_tmp->next);
                }
            }
            break;
        }

        if ((node->child) && (!child) || (child && prev)) {
            parent_tmp = node;
            s.push(node->child);
        }
        if (node->next) {
            if (node->next == child) {
                if (node->next->next)
                    s.push(node->next->next);
            } else {
                s.push(node->next);
            }
        }
    }

    /* Both nodes must exist. */
    if (child == nullptr) {
        std::cout << fail << std::endl;
        return;
    }

    /* check if it is the first child */
    if (nullptr == new_parent->child) {
        if (prev) {
            prev->next = prev->next->next;
            child->next = nullptr;
        } else if (parent) {
            parent->child = child->next;
            child->next = nullptr;
        } else {
            cout << "somthing wrong, should not be here" << endl;
            return;
        }

        new_parent->child = child;
        std::cout << pass << std::endl;
        return;
    } else {
        Node *cur = new_parent->child;
        /* check same name */
        while (cur) {
            if (cur->name == child->name) {
                std::cout << fail << std::endl;
                return;
            }
            cur = cur->next;
        }

        /* move from parent */
        if (prev) {
            prev->next = prev->next->next;
            child->next = nullptr;
        } else if (parent) {
            parent->child = child->next;
            child->next = nullptr;
        } else {
            cout << "somthing wrong, should not be here" << endl;
            return;
        }

        /* move to new parent*/
        cur = new_parent->child;
        if (cur->name > child->name) {
            new_parent->child = child;
            child->next = cur;
            child->parent_id = new_parent->id;
            std::cout << pass << std::endl;
            return;
        }      
        while (cur->next && cur->next->name <= child->name) {
            cur = cur->next;
        }
        Node *next = cur->next;
        cur->next = child;
        child->next = next;
        child->parent_id = new_parent->id;
        std::cout << pass << std::endl;
        return;
    }
}

/*
 * Description: Return a list of nodes matching certain criteria.
 * 
 * Params:
 *   - min_depth {integer}:     Minimum distance from root of query. Default: none.
 *   - max_depth {integer}:     Maximum distance from root of query. Default: none.
 *   - names {list of strings}: If specified, only return nodes whose names are in
 *                              the list.
 *   - ids {list of ids}:       If specified, only return nodes whose IDs are in
 *                              the list.
 *   - root_ids {list of ids}:  Search subtrees rooted at specified nodes.
 *                              If not specified, search from the root. If any ID
 *                              in the list doesn't exist in the tree, ignore it.
 */
void hierarchy::query(int min_depth, int max_depth, vector<string>& names,
 vector<string>& ids, vector<string>& root_ids)
{
    json j_arr;
    json j;
    json j_tmp;

    if (!root || (max_depth < min_depth)) {
        j["nodes"] = j_arr;
        std::cout << j.dump(4) << std::endl;
        return;
    }

    m_max_depth = max_depth;
    m_min_depth = min_depth;
    for (int i = 0; i < names.size(); i++)
        m_names_set.insert(names[i]);
    for (int i = 0; i < ids.size(); i++)
        m_ids_set.insert(ids[i]);
    for (int i = 0; i < root_ids.size(); i++) {
        m_root_ids_set.insert(root_ids[i]);
    }

    std::set<string>::iterator itr; 
    if (!m_root_ids_set.empty()) {
        // code here
        for (itr = m_root_ids_set.begin(); itr != m_root_ids_set.end(); itr++) {
            int depth = 0;
            Node *node_be_found = nullptr;
            find_root_id_node(root, &node_be_found, *itr, depth);
            if (node_be_found != nullptr) {
                preOrder(node_be_found->child, depth + 1);
            }
        }
    }
    else {
        preOrder(root, 0);
    }

    j["nodes"] = m_j_arr;
    std::cout << j.dump(4) << std::endl;

out:
    m_max_depth = INT_MAX;
    m_min_depth = 0;
    m_names_set.clear();
    m_ids_set.clear();
    m_root_ids_set.clear();
    m_j_arr.clear();
}

void hierarchy::preOrder(Node *node, int depth) {
    json j_tmp;

	if (node == nullptr)
	    return;

    if (depth < m_min_depth)
        goto traverse;

    if ((!m_names_set.empty() && !m_names_set.count(node->name)) ||
        (!m_ids_set.empty() && !m_ids_set.count(node->id))) {
        goto traverse;
    } else {
        j_tmp = json{{"name", node->name}, {"id", node->id},
             {"parent_id", node->parent_id}};  
        m_j_arr.emplace_back(j_tmp);
    }

traverse:
    if (depth < m_max_depth)
	    preOrder(node->child, depth + 1);
	preOrder(node->next, depth);
}

void hierarchy::find_root_id_node(Node *node, Node **node_be_found,
        string root_id, int& depth) {

	if (node == nullptr)
	    return;

    stack<pair<Node *, int>> s;
    s.push({node, depth});
    while (s.size() > 0) {
        Node *node_tmp = s.top().first;
        depth = s.top().second;
        s.pop();
        if (node_tmp->id == root_id) {
            *node_be_found = node_tmp;
            break;
        }
        if (node_tmp->child)
            s.push({node_tmp->child, depth + 1});
        if (node_tmp->next)
            s.push({node_tmp->next, depth});
    }
}

/*
 * Description: print node inorder for self-test
 */
void hierarchy::prn_node() {
    /* Inorder Traversal */
    stack<Node *> s;
    Node *cur = root;
    while (cur || !s.empty()) {
        if (cur) {
            s.push(cur);
            cur = cur->child;
        } else {
            Node *node = s.top();
            s.pop();
            cout << node->id;
            cur = node->next;
        }
    }
    cout << endl;
    return;
}
