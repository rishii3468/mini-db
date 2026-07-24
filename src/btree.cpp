#include "btree.h"
#include <algorithm>

struct BTreeIndex::Node {
    bool isLeaf = true;
    std::vector<Key> keys;
    std::vector<Value> values;
    std::vector<Node*> children;

    explicit Node(bool leaf) : isLeaf(leaf) {}

    ~Node() {
        for (Node* child : children) {
            delete child;
        }
    }
};

BTreeIndex::BTreeIndex() : root_(new Node(true)), size_(0) {}

BTreeIndex::~BTreeIndex() {
    delete root_;
}

void BTreeIndex::insert(const Key& key, std::streampos position) {
    if (!root_) {
        root_ = new Node(true);
    }

    if (root_->keys.size() == MAX_KEYS) {
        Node* newRoot = new Node(false);
        newRoot->children.push_back(root_);
        splitChild(newRoot, 0);
        root_ = newRoot;
    }

    insertNonFull(root_, key, position);
}

BTreeIndex::OptionalValue BTreeIndex::find(const Key& key) const {
    Node* target = findNode(root_, key);
    if (!target) {
        return std::nullopt;
    }

    std::size_t i = 0;
    while (i < target->keys.size() && target->keys[i] < key) {
        ++i;
    }

    if (i < target->keys.size() && target->keys[i] == key) {
        return target->values[i];
    }

    return std::nullopt;
}

bool BTreeIndex::contains(const Key& key) const {
    Node* target = findNode(root_, key);
    return target != nullptr;
}

void BTreeIndex::erase(const Key& key) {
    Node* target = findNode(root_, key);
    if (!target) {
        return;
    }

    auto it = std::find(target->keys.begin(), target->keys.end(), key);
    if (it != target->keys.end()) {
        std::size_t index = std::distance(target->keys.begin(), it);
        target->keys.erase(target->keys.begin() + index);
        target->values.erase(target->values.begin() + index);
        --size_;
    }
}

void BTreeIndex::clear() {
    delete root_;
    size_ = 0;
    root_ = new Node(true);
}

bool BTreeIndex::empty() const noexcept {
    return size_ == 0;
}

std::size_t BTreeIndex::size() const noexcept {
    return size_;
}

BTreeIndex::Node* BTreeIndex::findNode(Node* node, const Key& key) const {
    std::size_t i = 0;
    while (i < node->keys.size() && node->keys[i] < key) {
        ++i;
    }
    if (i < node->keys.size() && node->keys[i] == key) {
        return node;
    }
    if (node->isLeaf) {
        return nullptr;
    }
    return findNode(node->children[i], key);
}

void BTreeIndex::insertNonFull(Node* node, const Key& key, std::streampos position) {
    auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
    std::size_t index = std::distance(node->keys.begin(), it);

    if (index < node->keys.size() && node->keys[index] == key) {
        node->values[index].push_back(position);
        return;
    }

    if (node->isLeaf) {
        node->keys.insert(it, key);
        node->values.insert(node->values.begin() + index, Value{position});
        ++size_;
    } else {
        if (node->children[index]->keys.size() == MAX_KEYS) {
            splitChild(node, index);
            if (node->keys[index] < key) {
                ++index;
            }
        }
        insertNonFull(node->children[index], key, position);
    }
}

void BTreeIndex::splitChild(Node* parent, std::size_t index) {
    Node* child = parent->children[index];
    const std::size_t midIndex = child->keys.size() / 2;

    Key medianKey = child->keys[midIndex];
    Value medianValue = child->values[midIndex];

    Node* sibling = new Node(child->isLeaf);

    sibling->keys.assign(child->keys.begin() + midIndex + 1, child->keys.end());
    sibling->values.assign(child->values.begin() + midIndex + 1, child->values.end());

    if (!child->isLeaf) {
        sibling->children.assign(child->children.begin() + midIndex + 1, child->children.end());
        child->children.resize(midIndex + 1);
    }

    child->keys.resize(midIndex);
    child->values.resize(midIndex);

    parent->keys.insert(parent->keys.begin() + index, medianKey);
    parent->values.insert(parent->values.begin() + index, medianValue);
    parent->children.insert(parent->children.begin() + index + 1, sibling);
}