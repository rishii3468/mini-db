#ifndef BTREE_H
#define BTREE_H

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>
#include <cstddef>

class BTreeIndex {
public:
    using Key = std::string;
    using Value = std::vector<std::streampos>;
    using OptionalValue = std::optional<Value>;

    BTreeIndex();
    ~BTreeIndex();

    BTreeIndex(const BTreeIndex&) = delete;
    BTreeIndex& operator=(const BTreeIndex&) = delete;

    void insert(const Key& key, std::streampos position);
    OptionalValue find(const Key& key) const;
    bool contains(const Key& key) const;
    void erase(const Key& key);
    void clear();
    bool empty() const noexcept;
    std::size_t size() const noexcept;

private:
    struct Node;
    Node* root_ = nullptr;
    std::size_t size_ = 0;

    static constexpr std::size_t MAX_KEYS = 5;

    Node* findNode(Node* node, const Key& key) const;
    void insertNonFull(Node* node, const Key& key, std::streampos position);
    void splitChild(Node* parent, std::size_t index);
};

#endif 

