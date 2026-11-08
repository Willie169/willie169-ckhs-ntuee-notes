#pragma once // vector.hpp

#ifndef _VECTOR_GROW
#define _VECTOR_GROW 2
#endif

#include <algorithm>
#include <compare>
#include <cstddef>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

template <class T, class Allocator = std::allocator<T>>
  requires std::is_same_v<T, typename Allocator::value_type>
class vector {
public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  [[no_unique_address]] Allocator alloc;
  T *elems;
  std::size_t sz;
  std::size_t cap;

  constexpr void deallocate() {
    if (elems) {
      std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
      elems = nullptr;
    }
    sz = cap = 0;
  }

  constexpr void destroy_deallocate() {
    if (elems) {
      if constexpr (!std::is_trivially_destructible_v<T>)
        for (std::size_t i = 0; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
      std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
      elems = nullptr;
    }
    cap = 0;
  }

  template <bool UseMove>
  constexpr void transfer_elements(T *new_elems, std::size_t count) {
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memcpy(new_elems, elems, count * sizeof(T));
    } else {
      std::size_t i = 0;
      try {
        for (; i < count; ++i) {
          if constexpr (UseMove) {
            std::allocator_traits<Allocator>::construct(alloc, new_elems + i,
                                                        std::move(elems[i]));
          } else {
            std::allocator_traits<Allocator>::construct(alloc, new_elems + i,
                                                        elems[i]);
          }
        }
      } catch (...) {
        for (std::size_t j = 0; j < i; ++j)
          std::allocator_traits<Allocator>::destroy(alloc, new_elems + j);
        throw;
      }
    }
  }

public:
  constexpr vector() noexcept(noexcept(Allocator()))
      : alloc(Allocator()), elems(nullptr), sz(0), cap(0) {}
  explicit constexpr vector(const Allocator &alloc_) noexcept
      : alloc(alloc_), elems(nullptr), sz(0), cap(0) {}

  explicit vector(std::size_t count, const Allocator &alloc_ = Allocator())
      : alloc(alloc_), sz(count), cap(count) {
    if (count == 0)
      elems = nullptr;
    else {
      elems = std::allocator_traits<Allocator>::allocate(alloc, count);
      if constexpr (!std::is_trivially_copyable_v<T>) {
        std::size_t i = 0;
        try {
          for (; i < count; ++i)
            std::allocator_traits<Allocator>::construct(alloc, elems + i);
        } catch (...) {
          for (std::size_t j = 0; j < i; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
          deallocate();
          throw;
        }
      }
    }
  }

  constexpr vector(std::size_t count, const T &value,
                   const Allocator &alloc_ = Allocator())
      : alloc(alloc_), sz(count), cap(count) {
    if (count == 0)
      elems = nullptr;
    else {
      elems = std::allocator_traits<Allocator>::allocate(alloc, count);
      if constexpr (std::is_trivially_copyable_v<T>)
        std::fill_n(elems, count, value);
      else {
        std::size_t i = 0;
        try {
          for (; i < count; ++i)
            std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                        value);
        } catch (...) {
          for (std::size_t j = 0; j < i; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
          deallocate();
          throw;
        }
      }
    }
  }

  template <std::input_iterator InputIt>
  constexpr vector(InputIt first, InputIt last,
                   const Allocator &alloc_ = Allocator())
      : alloc(alloc_), elems(nullptr), sz(0), cap(0) {
    if constexpr (std::forward_iterator<InputIt>) {
      if (first == last)
        return;
      std::size_t count = static_cast<std::size_t>(std::distance(first, last));
      if (count > 0) {
        elems = std::allocator_traits<Allocator>::allocate(alloc, count);
        sz = cap = count;
        if constexpr (std::is_trivially_copyable_v<T>) {
          if (std::contiguous_iterator<InputIt>)
            std::memmove(elems, &*first, count * sizeof(T));
          else
            std::copy(first, last, elems);
        } else {
          std::size_t i = 0;
          try {
            for (; first != last; ++first, ++i)
              std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                          *first);
          } catch (...) {
            for (std::size_t j = 0; j < i; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, elems + j);
            deallocate();
            throw;
          }
        }
      }
    } else
      for (; first != last; ++first)
        push_back(*first);
  }

  constexpr vector(const vector &other)
      : alloc(std::allocator_traits<
              Allocator>::select_on_container_copy_construction(other.alloc)),
        elems(nullptr), sz(other.sz), cap(other.cap) {
    if (cap > 0) {
      elems = std::allocator_traits<Allocator>::allocate(alloc, cap);
      if constexpr (std::is_trivially_copyable_v<T>)
        std::memmove(elems, other.elems, sz * sizeof(T));
      else {
        std::size_t i = 0;
        try {
          for (; i < sz; ++i)
            std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                        other.elems[i]);
        } catch (...) {
          for (std::size_t j = 0; j < i; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
          std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
          throw;
        }
      }
    }
  }

  constexpr vector(vector &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value)
      : alloc(std::move(other.alloc)), elems(nullptr), sz(0), cap(0) {
    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_move_assignment::value ||
                  std::allocator_traits<Allocator>::is_always_equal::value ||
                  (requires {
                    { alloc == other.alloc } -> std::convertible_to<bool>;
                  } && (alloc == other.alloc))) {
      elems = std::exchange(other.elems, nullptr);
      sz = std::exchange(other.sz, 0);
      cap = std::exchange(other.cap, 0);
    } else {
      if (other.sz > 0) {
        elems = std::allocator_traits<Allocator>::allocate(alloc, other.sz);
        sz = cap = other.sz;
        if constexpr (std::is_trivially_move_constructible_v<T>)
          std::memmove(elems, other.elems, sz * sizeof(T));
        else if (std::is_nothrow_move_constructible_v<T> ||
                 !std::is_copy_constructible_v<T>) {
          std::size_t i = 0;
          try {
            for (; i < sz; ++i)
              std::allocator_traits<Allocator>::construct(
                  alloc, elems + i, std::move(other.elems[i]));
          } catch (...) {
            for (std::size_t j = 0; j < i; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, elems + j);
            std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
            throw;
          }
        } else {
          std::size_t i = 0;
          try {
            for (; i < sz; ++i)
              std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                          other.elems[i]);
          } catch (...) {
            for (std::size_t j = 0; j < i; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, elems + j);
            std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
            throw;
          }
        }
        if constexpr (!std::is_trivially_destructible_v<T> &&
                      !std::is_trivially_move_constructible_v<T>) {
          if (std::is_nothrow_move_constructible_v<T> ||
              !std::is_copy_constructible_v<T>) {
            for (std::size_t j = 0; j < sz; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, other.elems + j);
          }
        }
        other.elems = nullptr;
        other.sz = other.cap = 0;
      }
    }
  }

  constexpr vector(const vector &other, const Allocator &alloc_)
      : alloc(alloc_), elems(nullptr), sz(other.sz), cap(other.cap) {
    if (cap > 0) {
      elems = std::allocator_traits<Allocator>::allocate(alloc, cap);
      if constexpr (std::is_trivially_copyable_v<T>)
        std::memmove(elems, other.elems, sz * sizeof(T));
      else {
        std::size_t i = 0;
        try {
          for (; i < sz; ++i)
            std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                        other.elems[i]);
        } catch (...) {
          for (std::size_t j = 0; j < i; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
          std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
          throw;
        }
      }
    }
  }

  vector(std::initializer_list<T> ilist, const Allocator &alloc_ = Allocator())
      : alloc(alloc_), elems(nullptr), sz(ilist.size()), cap(ilist.size()) {
    if (cap > 0) {
      elems = std::allocator_traits<Allocator>::allocate(alloc, cap);
      if constexpr (std::is_trivially_copyable_v<T>)
        std::memmove(elems, ilist.begin(), sz * sizeof(T));
      else {
        std::size_t i = 0;
        try {
          for (const T *j = ilist.begin(); j != ilist.end(); ++j, ++i)
            std::allocator_traits<Allocator>::construct(alloc, elems + i, *j);
        } catch (...) {
          for (std::size_t k = 0; k < i; ++k)
            std::allocator_traits<Allocator>::destroy(alloc, elems + k);
          std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
          throw;
        }
      }
    }
  }

  constexpr ~vector() {
    if (elems) {
      if constexpr (!std::is_trivially_destructible_v<T>)
        for (std::size_t i = 0; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
      std::allocator_traits<Allocator>::deallocate(alloc, elems, cap);
    }
  }

  constexpr vector &operator=(const vector &other) {
    if (this != &other) {
      if constexpr (std::allocator_traits<Allocator>::
                        propagate_on_container_copy_assignment::value &&
                    (requires {
                      { alloc != other.alloc } -> std::convertible_to<bool>;
                    } && (alloc != other.alloc))) {
        destroy_deallocate();
        alloc = other.alloc;
        if (cap < other.sz) {
          elems = std::allocator_traits<Allocator>::allocate(alloc, other.sz);
          cap = other.sz;
        }
      } else if (other.sz > cap) {
        if (elems)
          destroy_deallocate();
        elems = std::allocator_traits<Allocator>::allocate(alloc, other.sz);
        cap = other.sz;
      } else if constexpr (!std::is_trivially_destructible_v<T>)
        for (std::size_t i = other.sz; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
      if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(elems, other.elems, other.sz * sizeof(T));
      else {
        std::size_t i = 0;
        try {
          for (; i < other.sz; ++i) {
            if (i < sz)
              elems[i] = other.elems[i];
            else
              std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                          other.elems[i]);
          }
        } catch (...) {
          for (std::size_t j = 0; j < i; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
          throw;
        }
      }
      sz = other.sz;
    }
    return *this;
  }

  constexpr vector &operator=(vector &&other) noexcept(
      std::allocator_traits<
          Allocator>::propagate_on_container_move_assignment::value ||
      std::allocator_traits<Allocator>::is_always_equal::value) {
    if (this != &other) {
      if constexpr (std::allocator_traits<Allocator>::
                        propagate_on_container_move_assignment::value ||
                    std::allocator_traits<Allocator>::is_always_equal::value ||
                    (requires {
                      { alloc == other.alloc } -> std::convertible_to<bool>;
                    } && (alloc == other.alloc))) {
        destroy_deallocate();
        alloc = std::move(other.alloc);
        elems = std::exchange(other.elems, nullptr);
        sz = std::exchange(other.sz, 0);
        cap = std::exchange(other.cap, 0);
      } else {
        if (other.sz > cap) {
          if (elems)
            destroy_deallocate();
          elems = std::allocator_traits<Allocator>::allocate(alloc, other.sz);
          cap = other.sz;
        } else if constexpr (!std::is_trivially_destructible_v<T>)
          for (std::size_t i = other.sz; i < sz; ++i)
            std::allocator_traits<Allocator>::destroy(alloc, elems + i);
        if constexpr (std::is_trivially_copyable_v<T>)
          std::memcpy(elems, other.elems, other.sz * sizeof(T));
        else if (std::is_nothrow_move_constructible_v<T> ||
                 !std::is_copy_constructible_v<T>) {
          std::size_t i = 0;
          try {
            for (; i < other.sz; ++i)
              std::allocator_traits<Allocator>::construct(
                  alloc, elems + i, std::move(other.elems[i]));
          } catch (...) {
            for (std::size_t j = 0; j < i; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, elems + j);
            throw;
          }
        } else {
          std::size_t i = 0;
          try {
            for (; i < other.sz; ++i)
              std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                          other.elems[i]);
          } catch (...) {
            for (std::size_t j = 0; j < i; ++j)
              std::allocator_traits<Allocator>::destroy(alloc, elems + j);
            throw;
          }
        }
        sz = other.sz;
      }
    }
    return *this;
  }

  constexpr vector &operator=(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
    return *this;
  }

  constexpr void assign(std::size_t count, const T &value) {
    if (count > cap) {
      destroy_deallocate();
      sz = 0;
      elems = std::allocator_traits<Allocator>::allocate(alloc, count);
      cap = count;
    }
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::fill_n(elems, count, value);
    } else {
      std::size_t min_size = std::min(sz, count);
      for (std::size_t i = 0; i < min_size; ++i)
        elems[i] = value;
      for (std::size_t i = min_size; i < count; ++i)
        std::allocator_traits<Allocator>::construct(alloc, elems + i, value);
      if (sz > count)
        for (std::size_t i = count; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
    }
    sz = count;
  }

  template <std::input_iterator InputIt>
  constexpr void assign(InputIt first, InputIt last) {
    if constexpr (std::forward_iterator<InputIt>) {
      std::size_t count = static_cast<std::size_t>(std::distance(first, last));
      if (count > cap) {
        destroy_deallocate();
        sz = 0;
        elems = std::allocator_traits<Allocator>::allocate(alloc, count);
        cap = count;
      }
      if constexpr (std::is_trivially_copyable_v<T>) {
        if constexpr (std::contiguous_iterator<InputIt>) {
          std::memmove(elems, &*first, count * sizeof(T));
        } else {
          std::copy(first, last, elems);
        }
      } else {
        std::size_t i = 0;
        for (; i < count && first != last; ++i, ++first) {
          if (i < sz)
            elems[i] = *first;
          else
            std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                        *first);
        }
        if (sz > count)
          for (std::size_t j = count; j < sz; ++j)
            std::allocator_traits<Allocator>::destroy(alloc, elems + j);
      }
      sz = count;
    } else {
      clear();
      for (; first != last; ++first)
        push_back(*first);
    }
  }

  constexpr void assign(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
  }

  constexpr allocator_type get_allocator() const noexcept { return alloc; }

  constexpr T &at(std::size_t index) {
    if (index >= sz)
      throw std::out_of_range("vector");
    return elems[index];
  }

  constexpr const T &at(std::size_t index) const {
    if (index >= sz)
      throw std::out_of_range("vector");
    return elems[index];
  }

  constexpr T &operator[](std::size_t index) { return elems[index]; }
  constexpr const T &operator[](std::size_t index) const {
    return elems[index];
  }
  constexpr T &front() { return *elems; }
  constexpr const T &front() const { return *elems; }
  constexpr T &back() { return elems[sz - 1]; }
  constexpr const T &back() const { return elems[sz - 1]; }

  constexpr T *data() noexcept { return elems; }
  constexpr const T *data() const noexcept { return elems; }

  constexpr iterator begin() noexcept { return elems; }
  constexpr const_iterator begin() const noexcept { return elems; }
  constexpr const_iterator cbegin() const noexcept { return elems; }

  constexpr iterator end() noexcept { return elems + sz; }
  constexpr const_iterator end() const noexcept { return elems + sz; }
  constexpr const_iterator cend() const noexcept { return elems + sz; }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(cbegin());
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  constexpr bool empty() const noexcept { return sz == 0; }
  constexpr std::size_t size() const noexcept { return sz; }
  constexpr std::size_t max_size() const noexcept {
    return std::min(
        std::allocator_traits<Allocator>::max_size(alloc),
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()));
  }

  constexpr void reserve(std::size_t new_cap) {
    if (new_cap <= cap)
      return;
    if (new_cap > max_size())
      throw std::length_error("vector");
    T *new_elems = std::allocator_traits<Allocator>::allocate(alloc, new_cap);
    if (elems) {
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(new_elems, elems, sz * sizeof(T));
      } else if (std::is_nothrow_move_constructible_v<T> ||
                 !std::is_copy_constructible_v<T>) {
        transfer_elements<true>(new_elems, sz);
      } else {
        transfer_elements<false>(new_elems, sz);
      }
      destroy_deallocate();
    }
    elems = new_elems;
    cap = new_cap;
  }

  constexpr std::size_t capacity() const noexcept { return cap; }

  constexpr void shrink_to_fit() {
    if (cap != sz) {
      if (sz == 0)
        destroy_deallocate();
      else {
        T *new_elems = std::allocator_traits<Allocator>::allocate(alloc, sz);
        if constexpr (std::is_trivially_copyable_v<T>) {
          std::memcpy(new_elems, elems, sz * sizeof(T));
        } else if (std::is_nothrow_move_constructible_v<T> ||
                   !std::is_copy_constructible_v<T>) {
          transfer_elements<true>(new_elems, sz);
        } else {
          transfer_elements<false>(new_elems, sz);
        }
        destroy_deallocate();
        elems = new_elems;
        cap = sz;
      }
    }
  }

  constexpr void clear() noexcept {
    if constexpr (!std::is_trivially_destructible_v<T>)
      for (std::size_t i = 0; i < sz; ++i)
        std::allocator_traits<Allocator>::destroy(alloc, elems + i);
    sz = 0;
  }

  constexpr iterator insert(const_iterator pos, const T &value) {
    std::size_t index = pos - elems;
    if (index > sz) {
      if (index >= cap)
        reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + 1));
      for (std::size_t i = sz; i < index; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, elems + i);
      }
      sz = index;
    } else if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if (index < sz) {
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memmove(elems + index + 1, elems + index,
                     (sz - index) * sizeof(T));
        elems[index] = value;
      } else {
        std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                    std::move(elems[sz - 1]));
        for (std::size_t i = sz - 1; i > index; --i)
          elems[i] = std::move(elems[i - 1]);
        elems[index] = value;
      }
    } else {
      std::allocator_traits<Allocator>::construct(alloc, elems + index, value);
    }
    ++sz;
    return elems + index;
  }

  constexpr iterator insert(const_iterator pos, T &&value) {
    std::size_t index = pos - elems;
    if (index > sz) {
      if (index >= cap)
        reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + 1));
      for (std::size_t i = sz; i < index; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, elems + i);
      }
      sz = index;
    }
    if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if (index < sz) {
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memmove(elems + index + 1, elems + index,
                     (sz - index) * sizeof(T));
        elems[index] = std::move(value);
      } else {
        std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                    std::move(elems[sz - 1]));
        for (std::size_t i = sz - 1; i > index; --i)
          elems[i] = std::move(elems[i - 1]);
        elems[index] = std::move(value);
      }
    } else {
      std::allocator_traits<Allocator>::construct(alloc, elems + index,
                                                  std::move(value));
    }
    ++sz;
    return elems + index;
  }

  constexpr iterator insert(const_iterator pos, std::size_t count,
                            const T &value) {
    if (count == 0)
      return const_cast<iterator>(pos);
    std::size_t index = pos - elems;
    if (index > sz) {
      if (index >= cap)
        reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + count));
      for (std::size_t i = sz; i < index; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, elems + i);
      }
      sz = index;
    }
    if (sz + count > cap)
      reserve(std::max(sz ? sz * _VECTOR_GROW : 1, sz + count));
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memmove(elems + index + count, elems + index,
                   (sz - index) * sizeof(T));
      std::fill_n(elems + index, count, value);
    } else {
      for (std::size_t i = sz + count - 1; i >= index + count; --i) {
        if (i < sz)
          elems[i] = std::move(elems[i - count]);
        else
          std::allocator_traits<Allocator>::construct(
              alloc, elems + i, std::move(elems[i - count]));
      }
      for (std::size_t i = index; i < index + count; ++i) {
        if (i < sz)
          elems[i] = value;
        else
          std::allocator_traits<Allocator>::construct(alloc, elems + i, value);
      }
    }
    sz += count;
    return elems + index;
  }

  template <std::input_iterator InputIt>
  constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
    std::size_t index = pos - elems;
    if (index > sz) {
      if constexpr (std::forward_iterator<InputIt>) {
        std::size_t count =
            static_cast<std::size_t>(std::distance(first, last));
        if (index >= cap)
          reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + count));
      } else {
        if (index >= cap)
          reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + 1));
      }
      for (std::size_t i = sz; i < index; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, elems + i);
      }
      sz = index;
    }
    if constexpr (std::forward_iterator<InputIt>) {
      std::size_t count = static_cast<std::size_t>(std::distance(first, last));
      if (count == 0)
        return const_cast<iterator>(pos);
      if (sz + count > cap)
        reserve(std::max(sz ? sz * _VECTOR_GROW : 1, sz + count));
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memmove(elems + index + count, elems + index,
                     (sz - index) * sizeof(T));
        if constexpr (std::contiguous_iterator<InputIt>)
          std::memmove(elems + index, &*first, count * sizeof(T));
        else
          std::copy(first, last, elems + index);
      } else {
        for (std::size_t i = sz + count - 1; i >= index + count; --i) {
          if (i < sz)
            elems[i] = std::move(elems[i - count]);
          else
            std::allocator_traits<Allocator>::construct(
                alloc, elems + i, std::move(elems[i - count]));
        }
        std::size_t i = index;
        for (; i < index + count && first != last; ++i, ++first) {
          if (i < sz)
            elems[i] = *first;
          else
            std::allocator_traits<Allocator>::construct(alloc, elems + i,
                                                        *first);
        }
      }
      sz += count;
      return elems + index;
    } else {
      vector<T, Allocator> tmp(first, last, alloc);
      return insert(pos, tmp.begin(), tmp.end());
    }
  }

  constexpr iterator insert(const_iterator pos,
                            std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
  }

  template <class Arg>
  constexpr iterator emplace(const_iterator pos, Arg &&arg) {
    std::size_t index = pos - elems;
    if (index > sz) {
      if (index >= cap)
        reserve(std::max(sz ? sz * _VECTOR_GROW : 1, index + 1));
      for (std::size_t i = sz; i < index; ++i) {
        std::allocator_traits<Allocator>::construct(alloc, elems + i);
      }
      sz = index;
    } else if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if (index == sz) {
      if constexpr (std::is_trivially_move_constructible_v<T>)
        elems[sz] = T(std::forward<Arg>(arg));
      else
        std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                    std::forward<Arg>(arg));
    } else {
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memmove(elems + index + 1, elems + index,
                     (sz - index) * sizeof(T));
        elems[index] = T(std::forward<Arg>(arg));
      } else {
        std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                    std::move(elems[sz - 1]));
        for (std::size_t i = sz - 1; i > index; --i)
          elems[i] = std::move(elems[i - 1]);
        elems[index] = T(std::forward<Arg>(arg));
      }
    }
    ++sz;
    return elems + index;
  }

  constexpr iterator erase(const_iterator pos) {
    std::size_t index = pos - elems;
    if (index >= sz)
      return elems + sz;
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memmove(elems + index, elems + index + 1,
                   (sz - index - 1) * sizeof(T));
    } else {
      for (std::size_t i = index; i < sz - 1; ++i)
        elems[i] = std::move(elems[i + 1]);
      std::allocator_traits<Allocator>::destroy(alloc, elems + sz - 1);
    }
    --sz;
    return elems + index;
  }

  constexpr iterator erase(const_iterator first, const_iterator last) {
    if (first == last)
      return const_cast<iterator>(first);
    std::size_t index = first - elems;
    std::size_t end_index = last - elems;
    if (index >= sz)
      return elems + sz;
    if (end_index > sz)
      end_index = sz;
    std::size_t count = end_index - index;
    if constexpr (std::is_trivially_copyable_v<T>) {
      std::memmove(elems + index, elems + index + count,
                   (sz - index - count) * sizeof(T));
    } else {
      for (std::size_t i = index; i + count < sz; ++i)
        elems[i] = std::move(elems[i + count]);
      for (std::size_t i = sz - count; i < sz; ++i)
        std::allocator_traits<Allocator>::destroy(alloc, elems + i);
    }
    sz -= count;
    return elems + index;
  }

  constexpr void push_back(const T &value) {
    if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if constexpr (std::is_trivially_copyable_v<T>)
      elems[sz++] = value;
    else {
      std::allocator_traits<Allocator>::construct(alloc, elems + sz, value);
      ++sz;
    }
  }

  constexpr void push_back(T &&value) {
    if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if constexpr (std::is_trivially_move_constructible_v<T>)
      elems[sz++] = std::move(value);
    else {
      std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                  std::move(value));
      ++sz;
    }
  }

  template <class Arg> constexpr T &emplace_back(Arg &&arg) {
    if (sz >= cap)
      reserve(sz ? sz * _VECTOR_GROW : 1);
    if constexpr (std::is_trivially_move_constructible_v<T>)
      elems[sz] = T(std::forward<Arg>(arg));
    else
      std::allocator_traits<Allocator>::construct(alloc, elems + sz,
                                                  std::forward<Arg>(arg));
    return elems[sz++];
  }

  constexpr void pop_back() {
    --sz;
    if constexpr (!std::is_trivially_destructible_v<T>)
      std::allocator_traits<Allocator>::destroy(alloc, elems + sz);
  }

  constexpr void resize(std::size_t new_size) {
    if (new_size < sz) {
      if constexpr (!std::is_trivially_destructible_v<T>)
        for (std::size_t i = new_size; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
      sz = new_size;
      return;
    }
    if (new_size > cap)
      reserve(new_size);
    if (new_size > sz) {
      if constexpr (std::is_trivially_default_constructible_v<T>) {
        std::uninitialized_default_construct(elems + sz, elems + new_size);
      } else if constexpr (std::is_default_constructible_v<T>) {
        for (std::size_t i = sz; i < new_size; ++i)
          std::allocator_traits<Allocator>::construct(alloc, elems + i);
      } else {
        throw std::logic_error("vector");
      }
      sz = new_size;
    }
  }

  constexpr void resize(std::size_t new_size, const T &value) {
    if (new_size < sz) {
      if constexpr (!std::is_trivially_destructible_v<T>)
        for (std::size_t i = new_size; i < sz; ++i)
          std::allocator_traits<Allocator>::destroy(alloc, elems + i);
      sz = new_size;
      return;
    }
    if (new_size > cap)
      reserve(new_size);
    if (new_size > sz) {
      if constexpr (std::is_trivially_default_constructible_v<T>) {
        std::uninitialized_fill(elems + sz, elems + new_size, value);
      } else {
        for (std::size_t i = sz; i < new_size; ++i)
          std::allocator_traits<Allocator>::construct(alloc, elems + i, value);
      }
      sz = new_size;
    }
  }

  constexpr void swap(vector &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value) {
    if constexpr (std::allocator_traits<
                      Allocator>::propagate_on_container_swap::value)
      std::swap(alloc, other.alloc);
    std::swap(elems, other.elems);
    std::swap(sz, other.sz);
    std::swap(cap, other.cap);
  }
};

template <class InputIt,
          class Allocator =
              std::allocator<typename std::iterator_traits<InputIt>::value_type>>
vector(InputIt, InputIt, Allocator = Allocator())
    -> vector<typename std::iterator_traits<InputIt>::value_type, Allocator>;

template <class T, class Allocator>
constexpr void
swap(std::vector<T, Allocator> &lhs,
     std::vector<T, Allocator> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

template <class T, class Allocator, class U>
constexpr typename std::vector<T, Allocator>::size_type
erase(std::vector<T, Allocator> &c, const U &value) {
  auto it = std::remove(c.begin(), c.end(), value);
  auto r = c.end() - it;
  c.erase(it, c.end());
  return r;
}

template <class T, class Allocator, class Pred>
constexpr typename std::vector<T, Allocator>::size_type
erase_if(std::vector<T, Allocator> &c, Pred pred) {
  auto it = std::remove_if(c.begin(), c.end(), pred);
  auto r = c.end() - it;
  c.erase(it, c.end());
  return r;
}

template <class T, class Allocator>
constexpr bool operator==(const std::vector<T, Allocator> &lhs,
                          const std::vector<T, Allocator> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Allocator>
constexpr std::strong_ordering
operator<=>(const std::vector<T, Allocator> &lhs,
            const std::vector<T, Allocator> &rhs) {
  return std::lexicographical_compare_three_way(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::compare_three_way());
}
