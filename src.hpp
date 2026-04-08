#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <stdexcept>

namespace sjtu {

template <typename T> class list {
protected:
  struct node {
    node *prev;
    node *next;
    bool is_dummy;
    alignas(T) unsigned char data[sizeof(T)];

    node(bool dummy) : prev(this), next(this), is_dummy(dummy) {}
    node(const T &val) : prev(this), next(this), is_dummy(false) {
      new (data) T(val);
    }
    ~node() {
      if (!is_dummy) {
        reinterpret_cast<T*>(data)->~T();
      }
    }
    T* val() { return reinterpret_cast<T*>(data); }
    const T* val() const { return reinterpret_cast<const T*>(data); }
  };

  node *head;
  size_t sz;

  node *insert(node *pos, node *cur) {
    cur->prev = pos->prev;
    cur->next = pos;
    pos->prev->next = cur;
    pos->prev = cur;
    sz++;
    return cur;
  }

  node *erase(node *pos) {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    sz--;
    return pos->next;
  }

public:
  class const_iterator;
  class iterator {
    friend class list;
    friend class const_iterator;
  private:
    node *ptr;
    const list *lst;
  public:
    iterator(node *p = nullptr, const list *l = nullptr) : ptr(p), lst(l) {}

    iterator operator++(int) {
      iterator tmp = *this;
      if (ptr) ptr = ptr->next;
      return tmp;
    }
    iterator &operator++() {
      if (ptr) ptr = ptr->next;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      if (ptr) ptr = ptr->prev;
      return tmp;
    }
    iterator &operator--() {
      if (ptr) ptr = ptr->prev;
      return *this;
    }

    T &operator*() const {
      if (!ptr || ptr->is_dummy) throw std::runtime_error("invalid");
      return *ptr->val();
    }
    T *operator->() const noexcept {
      if (!ptr || ptr->is_dummy) return nullptr;
      return ptr->val();
    }

    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const;
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const;
  };

  class const_iterator {
    friend class list;
    friend class iterator;
  private:
    node *ptr;
    const list *lst;
  public:
    const_iterator(node *p = nullptr, const list *l = nullptr) : ptr(p), lst(l) {}
    const_iterator(const iterator &other) : ptr(other.ptr), lst(other.lst) {}

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      if (ptr) ptr = ptr->next;
      return tmp;
    }
    const_iterator &operator++() {
      if (ptr) ptr = ptr->next;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      if (ptr) ptr = ptr->prev;
      return tmp;
    }
    const_iterator &operator--() {
      if (ptr) ptr = ptr->prev;
      return *this;
    }

    const T &operator*() const {
      if (!ptr || ptr->is_dummy) throw std::runtime_error("invalid");
      return *ptr->val();
    }
    const T *operator->() const noexcept {
      if (!ptr || ptr->is_dummy) return nullptr;
      return ptr->val();
    }

    bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
    bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
  };

  list() {
    head = new node(true);
    sz = 0;
  }
  list(const list &other) {
    head = new node(true);
    sz = 0;
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
  }
  virtual ~list() {
    clear();
    delete head;
  }
  list &operator=(const list &other) {
    if (this == &other) return *this;
    clear();
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
    return *this;
  }

  const T &front() const {
    if (empty()) throw std::runtime_error("empty");
    return *head->next->val();
  }
  const T &back() const {
    if (empty()) throw std::runtime_error("empty");
    return *head->prev->val();
  }
  T &front() {
    if (empty()) throw std::runtime_error("empty");
    return *head->next->val();
  }
  T &back() {
    if (empty()) throw std::runtime_error("empty");
    return *head->prev->val();
  }

  iterator begin() { return iterator(head->next, this); }
  const_iterator cbegin() const { return const_iterator(head->next, this); }
  iterator end() { return iterator(head, this); }
  const_iterator cend() const { return const_iterator(head, this); }

  virtual bool empty() const { return sz == 0; }
  virtual size_t size() const { return sz; }

  virtual void clear() {
    node *cur = head->next;
    while (cur != head) {
      node *next = cur->next;
      delete cur;
      cur = next;
    }
    head->next = head->prev = head;
    sz = 0;
  }

  virtual iterator insert(iterator pos, const T &value) {
    if (pos.lst != this) throw std::runtime_error("invalid");
    node *cur = new node(value);
    insert(pos.ptr, cur);
    return iterator(cur, this);
  }

  virtual iterator erase(iterator pos) {
    if (pos.lst != this || pos.ptr == head || pos.ptr == nullptr) throw std::runtime_error("invalid");
    node *next_node = erase(pos.ptr);
    delete pos.ptr;
    return iterator(next_node, this);
  }

  void push_back(const T &value) {
    insert(end(), value);
  }
  void pop_back() {
    if (empty()) throw std::runtime_error("empty");
    erase(iterator(head->prev, this));
  }
  void push_front(const T &value) {
    insert(begin(), value);
  }
  void pop_front() {
    if (empty()) throw std::runtime_error("empty");
    erase(begin());
  }
};

template <typename T>
bool list<T>::iterator::operator==(const const_iterator &rhs) const {
  return ptr == rhs.ptr;
}

template <typename T>
bool list<T>::iterator::operator!=(const const_iterator &rhs) const {
  return ptr != rhs.ptr;
}

} // namespace sjtu

#endif // SJTU_LIST_HPP