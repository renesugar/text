// Copyright (C) 2020 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_TEXT_DETAIL_BTREE_HPP
#define BOOST_TEXT_DETAIL_BTREE_HPP

#include <boost/text/detail/utility.hpp>

#include <boost/align/align.hpp>
#include <boost/align/aligned_alloc.hpp>
#include <boost/align/aligned_delete.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <atomic>
#include <vector>


namespace boost { namespace text { namespace detail {

    template<typename T>
    struct node_t;
    template<typename T>
    struct leaf_node_t;
    template<typename T>
    struct interior_node_t;
    template<typename T>
    struct node_ptr;

    template<typename T>
    struct node_ptr
    {
        node_ptr() noexcept : ptr_() {}
        explicit node_ptr(node_t<T> const * node) noexcept : ptr_(node) {}

        explicit operator bool() const noexcept { return ptr_.get(); }

        node_t<T> const * operator->() const noexcept { return ptr_.get(); }

        leaf_node_t<T> const * as_leaf() const noexcept;
        interior_node_t<T> const * as_interior() const noexcept;

        node_t<T> const * get() const noexcept { return ptr_.get(); }

        node_ptr<T> write() const;

        void swap(node_ptr & rhs) noexcept { ptr_.swap(rhs.ptr_); }

        bool operator==(node_ptr const & rhs) const noexcept
        {
            return ptr_ == rhs.ptr_;
        }

        leaf_node_t<T> * as_leaf() noexcept;
        interior_node_t<T> * as_interior() noexcept;

    private:
        intrusive_ptr<node_t<T> const> ptr_;
    };

    template<typename T>
    struct reference
    {
        reference(
            node_ptr<T> const & node,
            std::size_t lo,
            std::size_t hi) noexcept;

        node_ptr<T> vec_;
        std::size_t lo_;
        std::size_t hi_;
    };

    template<typename T>
    void * placement_address(void * buf, std::size_t buf_size) noexcept
    {
        std::size_t const alignment = alignof(T);
        std::size_t const size = sizeof(T);
        return alignment::align(alignment, size, buf, buf_size);
    }

    template<typename T>
    struct node_t
    {
        explicit node_t(bool leaf) noexcept : leaf_(leaf) { refs_ = 0; }
        node_t(node_t const & rhs) noexcept : leaf_(rhs.leaf_) { refs_ = 0; }
        node_t & operator=(node_t const & rhs) = delete;

        mutable std::atomic<int> refs_;
        bool leaf_;
    };

    constexpr unsigned int min_children = 8;
    constexpr unsigned int max_children = 16;

    template<typename T>
    inline std::size_t size(node_t<T> const * node) noexcept;

    using keys_t = container::static_vector<std::size_t, max_children>;

    template<typename T>
    using children_t = container::static_vector<node_ptr<T>, max_children>;

    static_assert(sizeof(std::size_t) * 8 <= 64, "");

    template<typename T>
    struct interior_node_t : node_t<T>
    {
        interior_node_t() noexcept : node_t<T>(false) {}

        void * operator new(std::size_t) = delete;

        alignas(64) keys_t keys_;
        children_t<T> children_;
    };

    template<typename T>
    inline interior_node_t<T> * new_interior_node()
    {
        void * ptr = alignment::aligned_alloc(
            alignof(interior_node_t<T>), sizeof(interior_node_t<T>));
        return ::new (ptr) interior_node_t<T>;
    }

    template<typename T>
    inline interior_node_t<T> *
    new_interior_node(interior_node_t<T> const & other)
    {
        void * ptr = alignment::aligned_alloc(
            alignof(interior_node_t<T>), sizeof(interior_node_t<T>));
        return ::new (ptr) interior_node_t<T>(other);
    }

    template<typename T>
    constexpr std::size_t node_buf_size() noexcept
    {
        return max_(alignof(std::vector<T>), alignof(reference<T>)) +
               max_(sizeof(std::vector<T>), sizeof(reference<T>));
    }

    template<typename T>
    struct leaf_node_t : node_t<T>
    {
        enum class which : char { vec, ref };

        leaf_node_t() noexcept : leaf_node_t(std::vector<T>()) {}

        leaf_node_t(std::vector<T> const & t) :
            node_t<T>(true),
            buf_ptr_(nullptr),
            which_(which::vec)
        {
            auto at =
                detail::placement_address<std::vector<T>>(buf_, sizeof(buf_));
            BOOST_ASSERT(at);
            buf_ptr_ = new (at) std::vector<T>(t);
        }

        leaf_node_t(std::vector<T> && t) noexcept :
            node_t<T>(true),
            buf_ptr_(nullptr),
            which_(which::vec)
        {
            auto at =
                detail::placement_address<std::vector<T>>(buf_, sizeof(buf_));
            BOOST_ASSERT(at);
            buf_ptr_ = new (at) std::vector<T>(std::move(t));
        }

        leaf_node_t(leaf_node_t const & rhs) :
            node_t<T>(true),
            buf_ptr_(rhs.buf_ptr_),
            which_(rhs.which_)
        {
            switch (which_) {
            case which::vec: {
                auto at = detail::placement_address<std::vector<T>>(
                    buf_, sizeof(buf_));
                BOOST_ASSERT(at);
                buf_ptr_ = new (at) std::vector<T>(rhs.as_vec());
                break;
            }
            case which::ref: {
                auto at =
                    detail::placement_address<reference<T>>(buf_, sizeof(buf_));
                BOOST_ASSERT(at);
                buf_ptr_ = new (at) reference<T>(rhs.as_reference());
                break;
            }
            default: BOOST_ASSERT(!"unhandled leaf node case"); break;
            }
        }

        leaf_node_t & operator=(leaf_node_t const &) = delete;
        leaf_node_t(leaf_node_t &&) = delete;
        leaf_node_t & operator=(leaf_node_t &&) = delete;

        ~leaf_node_t() noexcept
        {
            if (!buf_ptr_)
                return;

            switch (which_) {
            case which::vec: as_vec().~vector(); break;
            case which::ref: as_reference().~reference(); break;
            default: BOOST_ASSERT(!"unhandled leaf node case"); break;
            }
        }

        std::size_t size() const noexcept
        {
            switch (which_) {
            case which::vec: return as_vec().size(); break;
            case which::ref:
                return as_reference().hi_ - as_reference().lo_;
                break;
            default: BOOST_ASSERT(!"unhandled leaf node case"); break;
            }
            return -std::size_t(1); // This should never execute.
        }

        std::vector<T> const & as_vec() const noexcept
        {
            BOOST_ASSERT(which_ == which::vec);
            return *static_cast<std::vector<T> *>(buf_ptr_);
        }

        reference<T> const & as_reference() const noexcept
        {
            BOOST_ASSERT(which_ == which::ref);
            return *static_cast<reference<T> *>(buf_ptr_);
        }

        std::vector<T> & as_vec() noexcept
        {
            BOOST_ASSERT(which_ == which::vec);
            return *static_cast<std::vector<T> *>(buf_ptr_);
        }

        reference<T> & as_reference() noexcept
        {
            BOOST_ASSERT(which_ == which::ref);
            return *static_cast<reference<T> *>(buf_ptr_);
        }

        char buf_[detail::node_buf_size<T>()];
        void * buf_ptr_;
        which which_;
    };

    template<typename T>
    inline leaf_node_t<T> const * node_ptr<T>::as_leaf() const noexcept
    {
        BOOST_ASSERT(ptr_);
        BOOST_ASSERT(ptr_->leaf_);
        return static_cast<leaf_node_t<T> const *>(ptr_.get());
    }

    template<typename T>
    inline interior_node_t<T> const * node_ptr<T>::as_interior() const noexcept
    {
        BOOST_ASSERT(ptr_);
        BOOST_ASSERT(!ptr_->leaf_);
        return static_cast<interior_node_t<T> const *>(ptr_.get());
    }

    template<typename T>
    inline node_ptr<T> node_ptr<T>::write() const
    {
        if (ptr_->leaf_)
            return node_ptr<T>(new leaf_node_t<T>(*as_leaf()));
        else
            return node_ptr<T>(detail::new_interior_node(*as_interior()));
    }

    template<typename T>
    inline leaf_node_t<T> * node_ptr<T>::as_leaf() noexcept
    {
        BOOST_ASSERT(ptr_);
        BOOST_ASSERT(ptr_->leaf_);
        return const_cast<leaf_node_t<T> *>(
            static_cast<leaf_node_t<T> const *>(ptr_.get()));
    }

    template<typename T>
    inline interior_node_t<T> * node_ptr<T>::as_interior() noexcept
    {
        BOOST_ASSERT(ptr_);
        BOOST_ASSERT(!ptr_->leaf_);
        return const_cast<interior_node_t<T> *>(
            static_cast<interior_node_t<T> const *>(ptr_.get()));
    }

    // These functions were implemented following the "Reference counting"
    // example from Boost.Atomic.

    template<typename T>
    inline void intrusive_ptr_add_ref(node_t<T> const * node)
    {
        node->refs_.fetch_add(1, std::memory_order_relaxed);
    }

    template<typename T>
    inline void intrusive_ptr_release(node_t<T> const * node)
    {
        if (node->refs_.fetch_sub(1, std::memory_order_release) == 1) {
            std::atomic_thread_fence(std::memory_order_acquire);
            if (node->leaf_)
                delete static_cast<leaf_node_t<T> const *>(node);
            else
                alignment::aligned_delete{}((interior_node_t<T> *)(node));
        }
    }

    template<typename T>
    inline std::size_t size(node_t<T> const * node) noexcept
    {
        if (!node) {
            return 0;
        } else if (node->leaf_) {
            return static_cast<leaf_node_t<T> const *>(node)->size();
        } else {
            auto int_node = static_cast<interior_node_t<T> const *>(node);
            if (int_node->keys_.empty())
                return 0;
            return int_node->keys_.back();
        }
    }

    template<typename T>
    inline children_t<T> const & children(node_ptr<T> const & node) noexcept
    {
        return node.as_interior()->children_;
    }

    template<typename T>
    inline children_t<T> & children(node_ptr<T> & node) noexcept
    {
        return node.as_interior()->children_;
    }

    template<typename T>
    inline keys_t const & keys(node_ptr<T> const & node) noexcept
    {
        return node.as_interior()->keys_;
    }

    template<typename T>
    inline keys_t & keys(node_ptr<T> & node) noexcept
    {
        return node.as_interior()->keys_;
    }

    template<typename T>
    inline std::size_t num_children(node_ptr<T> const & node) noexcept
    {
        return detail::children(node).size();
    }

    template<typename T>
    inline std::size_t num_keys(node_ptr<T> const & node) noexcept
    {
        return detail::keys(node).size();
    }

    template<typename T>
    inline bool full(node_ptr<T> const & node) noexcept
    {
        return detail::num_children(node) == max_children;
    }

    template<typename T>
    inline bool almost_full(node_ptr<T> const & node) noexcept
    {
        return detail::num_children(node) == max_children - 1;
    }

    template<typename T>
    inline bool leaf_children(node_ptr<T> const & node)
    {
        return detail::children(node)[0]->leaf_;
    }

    template<typename T>
    inline std::size_t
    offset(interior_node_t<T> const * node, std::size_t i) noexcept
    {
        BOOST_ASSERT(i <= node->keys_.size());
        if (i == 0)
            return 0;
        return node->keys_[i - 1];
    }

    template<typename T>
    inline std::size_t offset(node_ptr<T> const & node, std::size_t i) noexcept
    {
        return detail::offset(node.as_interior(), i);
    }

    template<typename T>
    inline std::size_t
    find_child(interior_node_t<T> const * node, std::size_t n) noexcept
    {
        std::size_t i = 0;
        auto const sizes = node->keys_.size();
        while (i < sizes - 1 && node->keys_[i] <= n) {
            ++i;
        }
        BOOST_ASSERT(i < sizes);
        return i;
    }

    template<typename T>
    struct found_leaf
    {
        node_ptr<T> const * leaf_;
        std::size_t offset_;
        alignas(
            64) container::static_vector<interior_node_t<T> const *, 24> path_;

        static_assert(sizeof(interior_node_t<T> const *) * 8 <= 64, "");
    };

    template<typename T, typename LeafFunc, typename IntFunc>
    inline void visit_path_to_leaf(
        node_ptr<T> const & node,
        std::size_t n,
        LeafFunc const & leaf_func,
        IntFunc const & int_func) noexcept
    {
        BOOST_ASSERT(node);
        BOOST_ASSERT(n <= detail::size(node.get()));
        if (node->leaf_) {
            leaf_func(node, n);
            return;
        }
        auto const i = detail::find_child(node.as_interior(), n);
        node_ptr<T> const & child = detail::children(node)[i];
        auto const offset_ = detail::offset(node, i);
        int_func(node, n);
        detail::visit_path_to_leaf(child, n - offset_, leaf_func, int_func);
    }

    template<typename T>
    inline void find_leaf(
        node_ptr<T> const & node,
        std::size_t n,
        found_leaf<T> & retval) noexcept
    {
        auto leaf_func = [&retval](node_ptr<T> const & node, std::size_t n) {
            retval.leaf_ = &node;
            retval.offset_ = n;
        };
        auto int_func = [&retval](node_ptr<T> const & node, std::size_t) {
            retval.path_.push_back(node.as_interior());
        };
        detail::visit_path_to_leaf(node, n, leaf_func, int_func);
    }

    template<typename T>
    struct found_element
    {
        found_leaf<T> leaf_;
        T const * element_;
    };

    template<typename T>
    inline void find_element(
        node_ptr<T> const & node,
        std::size_t n,
        found_element<T> & retval) noexcept
    {
        BOOST_ASSERT(node);
        detail::find_leaf(node, n, retval.leaf_);

        leaf_node_t<T> const * leaf = retval.leaf_.leaf_->as_leaf();
        T const * e = nullptr;
        switch (leaf->which_) {
        case leaf_node_t<T>::which::vec:
            e = &leaf->as_vec()[retval.leaf_.offset_];
            break;
        case leaf_node_t<T>::which::ref:
            e = &leaf->as_reference().vec_.as_leaf()->as_vec()
                     [leaf->as_reference().lo_ + retval.leaf_.offset_];
            break;
        default: BOOST_ASSERT(!"unhandled leaf node case"); break;
        }
        retval.element_ = e;
    }

    template<typename T>
    inline reference<T>::reference(
        node_ptr<T> const & vec_node,
        std::size_t lo,
        std::size_t hi) noexcept :
        vec_(vec_node),
        lo_(lo),
        hi_(hi)
    {
        BOOST_ASSERT(vec_node);
        BOOST_ASSERT(vec_node->leaf_);
        BOOST_ASSERT(vec_node.as_leaf()->which_ == leaf_node_t<T>::which::vec);
    }

    template<typename T>
    inline node_ptr<T> make_node(std::vector<T> const & t)
    {
        return node_ptr<T>(new leaf_node_t<T>(t));
    }

    template<typename T>
    inline node_ptr<T> make_node(std::vector<T> && t)
    {
        return node_ptr<T>(new leaf_node_t<T>(std::move(t)));
    }

    template<typename T>
    inline node_ptr<T>
    make_ref(leaf_node_t<T> const * v, std::size_t lo, std::size_t hi)
    {
        BOOST_ASSERT(v->which_ == leaf_node_t<T>::which::vec);
        leaf_node_t<T> * leaf = nullptr;
        node_ptr<T> retval(leaf = new leaf_node_t<T>);
        leaf->which_ = leaf_node_t<T>::which::ref;
        auto at = detail::placement_address<reference<T>>(
            leaf->buf_, sizeof(leaf->buf_));
        BOOST_ASSERT(at);
        leaf->buf_ptr_ = new (at) reference<T>(node_ptr<T>(v), lo, hi);
        return retval;
    }

    template<typename T>
    inline node_ptr<T>
    make_ref(reference<T> const & r, std::size_t lo, std::size_t hi)
    {
        auto const offset = r.lo_;
        return detail::make_ref(r.vec_.as_leaf(), lo + offset, hi + offset);
    }

    template<typename T, typename Fn>
    void foreach_leaf(node_ptr<T> const & root, Fn && f)
    {
        if (!root)
            return;

        std::size_t offset = 0;
        while (true) {
            found_leaf<T> found;
            detail::find_leaf(root, offset, found);
            leaf_node_t<T> const * leaf = found.leaf_->as_leaf();

            if (!f(leaf))
                break;

            if ((offset += detail::size(leaf)) == detail::size(root.get()))
                break;
        }
    }

    template<typename Iter>
    struct reversed_range
    {
        Iter first_;
        Iter last_;

        Iter begin() const noexcept { return first_; }
        Iter end() const noexcept { return last_; }
    };

    template<typename Container>
    reversed_range<typename Container::const_reverse_iterator>
    reverse(Container const & c) noexcept
    {
        return {c.rbegin(), c.rend()};
    }

    template<typename T>
    inline void
    bump_keys(interior_node_t<T> * node, std::size_t from, std::size_t bump)
    {
        for (std::size_t i = from, size = node->keys_.size(); i < size; ++i) {
            node->keys_[i] += bump;
        }
    }

    template<typename T>
    inline void bump_along_path_to_leaf(
        node_ptr<T> const & node,
        std::size_t n,
        std::size_t bump) noexcept
    {
        auto leaf_func = [](node_ptr<T> const &, std::size_t) {};
        auto int_func = [bump](node_ptr<T> const & node, std::size_t n) {
            auto interior =
                const_cast<detail::interior_node_t<T> *>(node.as_interior());
            auto from = detail::find_child(interior, n);
            detail::bump_keys(interior, from, bump);
        };
        detail::visit_path_to_leaf(node, n, leaf_func, int_func);
    }

    template<typename T>
    inline void insert_child(
        interior_node_t<T> * node, std::size_t i, node_ptr<T> && child) noexcept
    {
        auto const child_size = detail::size(child.get());
        node->children_.insert(node->children_.begin() + i, std::move(child));
        node->keys_.insert(node->keys_.begin() + i, detail::offset(node, i));
        detail::bump_keys(node, i, child_size);
    }

    enum erasure_adjustments { adjust_keys, dont_adjust_keys };

    template<typename T>
    inline void erase_child(
        interior_node_t<T> * node,
        std::size_t i,
        erasure_adjustments adj = adjust_keys) noexcept
    {
        auto const child_size = detail::size(node->children_[i].get());
        node->children_.erase(node->children_.begin() + i);
        node->keys_.erase(node->keys_.begin() + i);
        if (adj == adjust_keys)
            detail::bump_keys(node, i, -child_size);
    }

    template<typename T>
    inline node_ptr<T>
    slice_leaf(node_ptr<T> node, std::size_t lo, std::size_t hi)
    {
        BOOST_ASSERT(node);
        BOOST_ASSERT(0 <= lo && lo <= detail::size(node.get()));
        BOOST_ASSERT(0 <= hi && hi <= detail::size(node.get()));
        BOOST_ASSERT(lo < hi);

        switch (node.as_leaf()->which_) {
        case leaf_node_t<T>::which::vec:
            return detail::make_ref(node.as_leaf(), lo, hi);
        case leaf_node_t<T>::which::ref: {
            return detail::make_ref(node.as_leaf()->as_reference(), lo, hi);
        }
        default: BOOST_ASSERT(!"unhandled leaf node case"); break;
        }
        return node_ptr<T>(); // This should never execute.
    }

    template<typename T>
    struct leaf_slices
    {
        node_ptr<T> slice;
        node_ptr<T> other_slice;
    };

    template<typename T>
    inline leaf_slices<T>
    erase_leaf(node_ptr<T> node, std::size_t lo, std::size_t hi)
    {
        BOOST_ASSERT(node);
        BOOST_ASSERT(0 <= lo && lo <= detail::size(node.get()));
        BOOST_ASSERT(0 <= hi && hi <= detail::size(node.get()));
        BOOST_ASSERT(lo < hi);

        auto const leaf_size = detail::size(node.get());

        leaf_slices<T> retval;

        if (lo == 0 && hi == leaf_size)
            return retval;

        if (hi != leaf_size)
            retval.other_slice = slice_leaf(node, hi, leaf_size);
        if (lo != 0)
            retval.slice = slice_leaf(node, 0, lo);

        if (!retval.slice)
            retval.slice.swap(retval.other_slice);

        return retval;
    }

    // Follows CLRS.
    template<typename T>
    inline node_ptr<T> btree_split_child(node_ptr<T> parent, std::size_t i)
    {
        BOOST_ASSERT(0 <= i && i < detail::num_children(parent));
        BOOST_ASSERT(!detail::full(parent));
        BOOST_ASSERT(
            detail::full(detail::children(parent)[i]) ||
            detail::almost_full(detail::children(parent)[i]));

        node_ptr<T> retval;

        interior_node_t<T> * new_node = nullptr;
        node_ptr<T> new_node_ptr(new_node = detail::new_interior_node<T>());

        BOOST_ASSERT(!detail::leaf_children(parent));

        {
            node_ptr<T> child = detail::children(parent)[i];
            std::size_t const elements = min_children - (detail::full(child) ? 0 : 1);
            new_node->children_.resize(elements);
            std::copy(
                detail::children(child).begin() + min_children,
                detail::children(child).end(),
                new_node->children_.begin());
            new_node->keys_.resize(elements);
            auto it = new_node->children_.begin();
            std::size_t sum = 0;
            for (auto & key : new_node->keys_) {
                sum += detail::size(it->get());
                key = sum;
                ++it;
            }
        }

        retval = parent.write();
        detail::children(retval).insert(
            detail::children(retval).begin() + i + 1, new_node_ptr);

        node_ptr<T> & child = detail::children(retval)[i];
        child = child.write();
        detail::children(child).resize(min_children);
        detail::keys(child).resize(min_children);

        detail::keys(retval).insert(
            detail::keys(retval).begin() + i,
            detail::offset(retval, i) + detail::size(child.get()));

        return retval;
    }

    // Analogous to btree_split_child(), for leaf nodes.
    template<typename T>
    inline node_ptr<T>
    btree_split_leaf(node_ptr<T> parent, std::size_t i, std::size_t at)
    {
        BOOST_ASSERT(0 <= i && i < detail::num_children(parent));
        BOOST_ASSERT(0 <= at && at <= detail::size(parent.get()));
        BOOST_ASSERT(!detail::full(parent));

        node_ptr<T> child = detail::children(parent)[i];

        auto const child_size = child.as_leaf()->size();
        auto const offset_at_i = detail::offset(parent, i);
        auto const cut = at - offset_at_i;

        if (cut == 0 || cut == child_size)
            return parent;

        node_ptr<T> retval;

        node_ptr<T> right = slice_leaf(child, cut, child_size);
        node_ptr<T> left = slice_leaf(child, 0, cut);

        retval = parent.write();
        detail::children(retval)[i] = left;
        detail::children(retval).insert(
            detail::children(retval).begin() + i + 1, right);
        detail::keys(retval).insert(
            detail::keys(retval).begin() + i, offset_at_i + cut);

        return retval;
    }

    // Follows CLRS.
    template<typename T>
    inline node_ptr<T> btree_insert_nonfull(
        node_ptr<T> parent, std::size_t at, node_ptr<T> && node)
    {
        BOOST_ASSERT(!parent->leaf_);
        BOOST_ASSERT(0 <= at && at <= detail::size(parent.get()));
        BOOST_ASSERT(node->leaf_);

        std::size_t i = detail::find_child(parent.as_interior(), at);
        if (detail::leaf_children(parent)) {
            // Note that this split may add a node to parent, for a
            // maximum of two added nodes in the leaf code path.
            parent = detail::btree_split_leaf(parent, i, at);
            if (detail::keys(parent)[i] <= at)
                ++i;

            parent = parent.write();
            detail::insert_child(parent.as_interior(), i, std::move(node));
        } else {
            {
                node_ptr<T> child = detail::children(parent)[i];
                bool const child_i_needs_split =
                    detail::full(child) || (detail::leaf_children(child) &&
                                            detail::almost_full(child));
                if (child_i_needs_split) {
                    parent = detail::btree_split_child(parent, i);
                    if (detail::keys(parent)[i] <= at)
                        ++i;
                }
            }
            parent = parent.write();
            auto delta = -detail::size(detail::children(parent)[i].get());
            node_ptr<T> new_child = detail::btree_insert_nonfull(
                detail::children(parent)[i],
                at - detail::offset(parent, i),
                std::move(node));
            delta += detail::size(new_child.get());
            detail::children(parent)[i] = new_child;
            for (std::size_t j = i, size = detail::num_keys(parent); j < size; ++j) {
                detail::keys(parent)[j] += delta;
            }
        }

        return parent;
    }

    // Follows CLRS.
    template<typename T>
    inline node_ptr<T>
    btree_insert(node_ptr<T> root, std::size_t at, node_ptr<T> && node)
    {
        BOOST_ASSERT(0 <= at && at <= detail::size(root.get()));
        BOOST_ASSERT(node->leaf_);

        if (!root) {
            return std::move(node);
        } else if (root->leaf_) {
            interior_node_t<T> * new_root = nullptr;
            node_ptr<T> new_root_ptr(new_root = detail::new_interior_node<T>());
            auto const root_size = detail::size(root.get());
            new_root->children_.push_back(root);
            new_root->keys_.push_back(root_size);
            return detail::btree_insert_nonfull(
                new_root_ptr, at, std::move(node));
        } else if (
            detail::full(root) ||
            (detail::leaf_children(root) && detail::almost_full(root))) {
            interior_node_t<T> * new_root = nullptr;
            node_ptr<T> new_root_ptr(new_root = detail::new_interior_node<T>());
            auto const root_size = detail::size(root.get());
            new_root->children_.push_back(root);
            new_root->keys_.push_back(root_size);
            new_root_ptr = detail::btree_split_child(new_root_ptr, 0);
            return detail::btree_insert_nonfull(
                new_root_ptr, at, std::move(node));
        } else {
            return detail::btree_insert_nonfull(root, at, std::move(node));
        }
    }

    // Recursing top-to-bottom, pull nodes down the tree as necessary to
    // ensure that each node has min_children + 1 nodes in it *before*
    // recursing into it.  This enables the erasure to happen in a single
    // downward pass, with no backtracking.  This function only erases
    // entire segments; the segments must have been split appropriately
    // before this function is ever called.
    template<typename T>
    inline node_ptr<T> btree_erase(
        node_ptr<T> node, std::size_t at, leaf_node_t<T> const * leaf)
    {
        BOOST_ASSERT(node);

        node_ptr<T> retval;

        auto child_index = detail::find_child(node.as_interior(), at);

        if (detail::leaf_children(node)) {
            if (detail::num_children(node) == 2)
                return detail::children(node)[child_index ? 0 : 1];

            BOOST_ASSERT(detail::children(node)[child_index].as_leaf() == leaf);

            retval = node.write();
            detail::erase_child(retval.as_interior(), child_index);
            return retval;
        }

        retval = node.write();

        node_ptr<T> new_child;

        node_ptr<T> & child = detail::children(retval)[child_index];
        // Due to the use of almost_full() in a few places, == does not
        // actually work here.  As unsatisfying as it is, the minimium
        // possible number of children is actually min_children - 1.
        if (detail::num_children(child) <= min_children) {
            BOOST_ASSERT(
                child_index != 0 ||
                child_index != detail::num_children(retval) - 1);

            if (child_index != 0 &&
                min_children + 1 <= detail::num_children(detail::children(
                                        retval)[child_index - 1])) {
                node_ptr<T> & child_left_sib =
                    detail::children(retval)[child_index - 1];

                // Remove last element of left sibling.
                node_ptr<T> moved_node =
                    detail::children(child_left_sib).back();
                auto const moved_node_size = detail::size(moved_node.get());

                child_left_sib = child_left_sib.write();
                detail::erase_child(
                    child_left_sib.as_interior(),
                    detail::num_children(child_left_sib) - 1);

                // Prepend last element onto child; now child has min_children
                // + 1 children, and we can recurse.
                child = child.write();
                detail::insert_child(
                    child.as_interior(), 0, std::move(moved_node));

                std::size_t const offset_ =
                    detail::offset(retval, child_index);
                new_child = detail::btree_erase(
                    child, at - offset_ + moved_node_size, leaf);
            } else if (
                child_index != detail::num_children(retval) - 1 &&
                min_children + 1 <= detail::num_children(detail::children(
                                        retval)[child_index + 1])) {
                node_ptr<T> & child_right_sib =
                    detail::children(retval)[child_index + 1];

                // Remove first element of right sibling.
                node_ptr<T> moved_node =
                    detail::children(child_right_sib).front();

                child_right_sib = child_right_sib.write();
                detail::erase_child(child_right_sib.as_interior(), 0);

                // Append first element onto child; now child has min_children
                // + 1 children, and we can recurse.
                child = child.write();
                detail::insert_child(
                    child.as_interior(),
                    detail::num_children(child),
                    std::move(moved_node));

                std::size_t const offset_ =
                    detail::offset(retval, child_index);
                new_child = detail::btree_erase(child, at - offset_, leaf);
            } else {
                auto const right_index =
                    child_index == 0 ? child_index + 1 : child_index;
                auto const left_index = right_index - 1;

                node_ptr<T> & left = detail::children(retval)[left_index];
                node_ptr<T> & right = detail::children(retval)[right_index];

                left = left.write();
                right = right.write();

                children_t<T> & left_children = detail::children(left);
                children_t<T> & right_children = detail::children(right);

                left_children.insert(
                    left_children.end(),
                    right_children.begin(),
                    right_children.end());

                keys_t & left_keys = detail::keys(left);
                keys_t & right_keys = detail::keys(right);

                auto const old_left_size = left_keys.back();
                std::size_t const old_children = detail::num_keys(left);

                left_keys.insert(
                    left_keys.end(), right_keys.begin(), right_keys.end());
                for (std::size_t i = old_children, size = detail::num_keys(left);
                     i < size;
                     ++i) {
                    left_keys[i] += old_left_size;
                }

                std::size_t const offset_ =
                    detail::offset(retval, left_index);
                new_child = detail::btree_erase(left, at - offset_, leaf);

                // This can only happen if node is the root.
                if (detail::num_children(retval) == 2)
                    return new_child;

                detail::erase_child(
                    retval.as_interior(), right_index, dont_adjust_keys);

                if (right_index <= child_index)
                    --child_index;
            }
        } else {
            std::size_t const offset_ = detail::offset(retval, child_index);
            new_child = detail::btree_erase(child, at - offset_, leaf);
        }

        detail::children(retval)[child_index] = new_child;
        std::size_t prev_size = 0;
        for (std::size_t i = 0, size = detail::num_keys(retval); i < size; ++i) {
            prev_size += detail::size(detail::children(retval)[i].get());
            detail::keys(retval)[i] = prev_size;
        }

        return retval;
    }

    template<typename T>
    inline node_ptr<T>
    btree_erase(node_ptr<T> root, std::size_t lo, std::size_t hi)
    {
        BOOST_ASSERT(root);
        BOOST_ASSERT(0 <= lo && lo <= detail::size(root.get()));
        BOOST_ASSERT(0 <= hi && hi <= detail::size(root.get()));
        BOOST_ASSERT(lo < hi);

        BOOST_ASSERT(root);

        if (lo == 0 && hi == detail::size(root.get())) {
            return node_ptr<T>();
        } else if (root->leaf_) {
            leaf_slices<T> slices;
            slices = detail::erase_leaf(root, lo, hi);
            if (!slices.other_slice) {
                return slices.slice;
            } else {
                interior_node_t<T> * new_root = nullptr;
                node_ptr<T> new_root_ptr(
                    new_root = detail::new_interior_node<T>());
                new_root->keys_.push_back(detail::size(slices.slice.get()));
                new_root->keys_.push_back(
                    new_root->keys_[0] +
                    detail::size(slices.other_slice.get()));
                new_root->children_.push_back(std::move(slices.slice));
                new_root->children_.push_back(std::move(slices.other_slice));
                return new_root_ptr;
            }
        } else {
            node_ptr<T> retval = root;

            auto const final_size = detail::size(retval.get()) - (hi - lo);

            // Right after the hi-segment, insert the suffix of the
            // hi-segment that's not being erased (if there is one).
            detail::found_leaf<T> found_hi;
            detail::find_leaf(retval, hi, found_hi);
            auto const hi_leaf_size = detail::size(found_hi.leaf_->get());
            if (found_hi.offset_ != 0 && found_hi.offset_ != hi_leaf_size) {
                node_ptr<T> suffix =
                    slice_leaf(*found_hi.leaf_, found_hi.offset_, hi_leaf_size);
                auto const suffix_at = hi - found_hi.offset_ + hi_leaf_size;
                retval =
                    detail::btree_insert(retval, suffix_at, std::move(suffix));
                detail::find_leaf(retval, suffix_at, found_hi);
            }

            // Right before the lo-segment, insert the prefix of the
            // lo-segment that's not being erased (if there is one).
            detail::found_leaf<T> found_lo;
            detail::find_leaf(retval, lo, found_lo);
            if (found_lo.offset_ != 0) {
                auto const lo_leaf_size = detail::size(found_lo.leaf_->get());
                node_ptr<T> prefix =
                    slice_leaf(*found_lo.leaf_, 0, found_lo.offset_);
                if (prefix.get() == found_lo.leaf_->get())
                    hi -= lo_leaf_size;
                retval = detail::btree_insert(
                    retval, lo - found_lo.offset_, std::move(prefix));
                detail::find_leaf(retval, lo, found_lo);
            }

            BOOST_ASSERT(found_lo.offset_ == 0);
            BOOST_ASSERT(
                found_hi.offset_ == 0 || found_hi.offset_ == hi_leaf_size);

            leaf_node_t<T> const * leaf_lo = found_lo.leaf_->as_leaf();
            while (true) {
                retval = detail::btree_erase(retval, lo, leaf_lo);
                BOOST_ASSERT(final_size <= detail::size(retval.get()));
                if (detail::size(retval.get()) == final_size)
                    break;
                found_leaf<T> found;
                detail::find_leaf(retval, lo, found);
                leaf_lo = found.leaf_->as_leaf();
            }

            return retval;
        }
    }

#ifdef BOOST_TEXT_TESTING
    template<typename T>
    void dump_tree(
        std::ostream & os,
        node_ptr<T> const & root,
        std::size_t key = -1,
        std::size_t indent = 0);

    template<typename T>
    inline std::size_t check_sizes(node_ptr<T> const & node, std::size_t size)
    {
        if (node->leaf_) {
            auto leaf = node.as_leaf();
            if (leaf->which_ == leaf_node_t<T>::which::vec)
                return leaf->as_vec().size();
            else
                return leaf->as_reference().hi_ - leaf->as_reference().lo_;
        }

        std::size_t children_size = 0;
        std::size_t prev_key = 0;
        std::size_t i = 0;
        for (auto const & child : detail::children(node)) {
            std::size_t key = detail::keys(node)[i++];
            children_size += detail::check_sizes(child, key - prev_key);
            prev_key = key;
        }

        if (children_size != size) {
            (void)0; // set breakpoint here
        }
        BOOST_ASSERT(children_size == size);

        return children_size;
    }
#endif

}}}

#endif
