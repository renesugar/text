#ifndef BOOST_TEXT_UNENCODED_ROPE_VIEW_HPP
#define BOOST_TEXT_UNENCODED_ROPE_VIEW_HPP

#include <boost/text/detail/rope.hpp>


namespace boost { namespace text {

    struct unencoded_rope;

    namespace detail {
        struct const_rope_view_iterator;
        struct const_reverse_rope_view_iterator;
    }

    /** A reference to a substring of an unencoded_rope, string, or
        repeated_string_view.  The substring is assumed to be UTF-8 encoded,
        though it is possible to construct an unencoded_rope_view which is
        not. */
    struct unencoded_rope_view
    {
        using iterator = detail::const_rope_view_iterator;
        using const_iterator = detail::const_rope_view_iterator;
        using reverse_iterator = detail::const_reverse_rope_view_iterator;
        using const_reverse_iterator = detail::const_reverse_rope_view_iterator;

        using size_type = std::ptrdiff_t;

        /** Default ctor.

            \post size() == 0 && begin() == end() */
        unencoded_rope_view() noexcept : ref_(rope_ref()), which_(which::r) {}

        /** Constructs an unencoded_rope_view covering the entire given
            unencoded_rope. The UTF-8 encoding is not checked. */
        unencoded_rope_view(unencoded_rope const & r) noexcept;

        /** Constructs a substring of r, taken from the range of chars at
            offsets [lo, hi).  If either of lo or hi is a negative value x, x
            is taken to be an offset from the end, and so x + size() is used
            instead.  The UTF-8 encoding is checked only at the beginning and
            end of the sequence, to prevent slicing of code points.  To fully
            check the encoding, use checked_encoding().

            These preconditions apply to the values used after size() is added
            to any negative arguments.

            \pre 0 <= lo && lo <= r.size()
            \pre 0 <= hi && lhi <= r.size()
            \pre lo <= hi
            \throw std::invalid_argument if the ends of the string are not
            valid UTF-8.
            \post size() == r.size() && begin() == r.begin() + lo && end() ==
            r.begin() + hi */
        unencoded_rope_view(unencoded_rope const & r, int lo, int hi);

        /** Constructs an unencoded_rope_view covering the entire given
            unencoded_rope.  The UTF-8 encoding is not checked. */
        unencoded_rope_view(string const & r) noexcept;

        /** Constructs a substring of r, taken from the range of chars at
            offsets [lo, hi).  If either of lo or hi is a negative value x, x
            is taken to be an offset from the end, and so x + size() is used
            instead.  The UTF-8 encoding is checked only at the beginning and
            end of the sequence, to prevent slicing of code points.  To fully
            check the encoding, use checked_encoding().

            These preconditions apply to the values used after size() is added
            to any negative arguments.

            \pre 0 <= lo && lo <= r.size()
            \pre 0 <= hi && lhi <= r.size()
            \pre lo <= hi
            \throw std::invalid_argument if the ends of the string are not
            valid UTF-8.
            \post size() == r.size() && begin() == r.begin() + lo && end() ==
            r.begin() + hi */
        unencoded_rope_view(string const & r, int lo, int hi);

        /** Constructs an unencoded_rope_view from a null-terminated C string.
            The UTF-8 encoding is checked only at the beginning and end of the
            string, to prevent slicing of code points.  To fully check the
            encoding, use checked_encoding().

            \pre strlen(c_str) <= max_size()
            \throw std::invalid_argument if the ends of the string are not valid
            UTF-8. \post data() == c_str && size() == strlen(c_str) */
        unencoded_rope_view(char const * c_str) noexcept :
            ref_(string_view(c_str)),
            which_(which::tv)
        {}

        /** Constructs an unencoded_rope_view covering the entire given
            string_view.  The UTF-8 encoding is not checked. */
        unencoded_rope_view(string_view tv) noexcept :
            ref_(tv),
            which_(which::tv)
        {}

        /** Constructs an unencoded_rope_view covering the entire given
            repeated_string_view.  The UTF-8 encoding is not checked. */
        unencoded_rope_view(repeated_string_view rtv) noexcept :
            ref_(repeated_ref(rtv, 0, rtv.size())),
            which_(which::rtv)
        {}

        /** Constructs a substring of rtv, taken from the range of chars at
            offsets [lo, hi).  If either of lo or hi is a negative value x, x
            is taken to be an offset from the end, and so x + size() is used
            instead.  The UTF-8 encoding is checked only at the beginning and
            end of the sequence, to prevent slicing of code points.  To fully
            check the encoding, use checked_encoding().

            These preconditions apply to the values used after size() is added
            to any negative arguments.

            \pre 0 <= lo && lo <= r.size()
            \pre 0 <= hi && lhi <= r.size()
            \pre lo <= hi
            \throw std::invalid_argument if the ends of the string are not
            valid UTF-8.
            \post size() == r.size() && begin() == r.begin() + lo && end() ==
            r.begin() + hi */
        unencoded_rope_view(repeated_string_view rtv, int lo, int hi);

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;

        bool empty() const noexcept;
        size_type size() const noexcept;

        /** Returns the i-th char of *this (not a reference).

            \pre i < size() */
        char operator[](int i) const noexcept;

        /** Returns a substring of *this, taken from the range of chars at
            offsets [lo, hi).  If either of lo or hi is a negative value x, x
            is taken to be an offset from the end, and so x + size() is used
            instead.

            These preconditions apply to the values used after size() is added
            to any negative arguments.

            \pre 0 <= lo && lo <= size()
            \pre 0 <= hi && lhi <= size()
            \pre lo <= hi
            \throw std::invalid_argument if the ends of the string are not
            valid UTF-8. */
        unencoded_rope_view operator()(int lo, int hi) const;

        /** Returns a substring of *this, taken from the first cut chars when
            cut => 0, or the last -cut chars when cut < 0.

            \pre 0 <= cut && cut <= size() || 0 <= -cut && -cut <= size()
            \throw std::invalid_argument if the ends of the string are not
            valid UTF-8. */
        unencoded_rope_view operator()(int cut) const
        {
            int lo = 0;
            int hi = cut;
            if (cut < 0) {
                lo = cut + size();
                hi = size();
            }
            assert(0 <= lo && lo <= size());
            assert(0 <= hi && hi <= size());
            return operator()(lo, hi);
        }

        /** Returns the maximum size a string_view can have. */
        size_type max_size() const noexcept { return PTRDIFF_MAX; }

        /** Visits each segment s of the underlying unencoded_rope and calls
            f(s).  Each segment is a value whose type models a Char_iterator
            iterator-range.  Depending of the operation performed on each
            segment, this may be more efficient than iterating over [begin(),
            end()).

            \pre Fn is an Invocable accepting a single argument whose begin
            and end model Char_iterator. */
        template<typename Fn>
        void foreach_segment(Fn && f) const;

        /** Lexicographical compare.  Returns a value < 0 when *this is
            lexicographically less than rhs, 0 if *this == rhs, and a value >
            0 if *this is lexicographically greater than rhs. */
        int compare(unencoded_rope_view rhs) const noexcept;

        /** Swaps *this with rhs. */
        void swap(unencoded_rope_view & rhs) noexcept
        {
            std::swap(ref_, rhs.ref_);
            std::swap(which_, rhs.which_);
        }

#ifndef BOOST_TEXT_DOXYGEN

    private:
        enum class which { r, tv, rtv };

        struct rope_ref
        {
            rope_ref() : r_(nullptr), lo_(0), hi_(0) {}
            rope_ref(unencoded_rope const * r, int lo, int hi) :
                r_(r),
                lo_(lo),
                hi_(hi)
            {}

            unencoded_rope const * r_;
            int lo_;
            int hi_;
        };

        struct repeated_ref
        {
            repeated_ref() : rtv_(), lo_(0), hi_(0) {}
            repeated_ref(repeated_string_view rtv, int lo, int hi) :
                rtv_(rtv),
                lo_(lo),
                hi_(hi)
            {}

            repeated_string_view rtv_;
            int lo_;
            int hi_;
        };

        union ref
        {
            ref(rope_ref r) : r_(r) {}
            ref(string_view tv) : tv_(tv) {}
            ref(repeated_ref rtv) : rtv_(rtv) {}

            ref(ref const & rhs) { memcpy(this, &rhs, sizeof(*this)); }

            ref & operator=(ref const & rhs)
            {
                memcpy(this, &rhs, sizeof(*this));
                return *this;
            }

            rope_ref r_;
            string_view tv_;
            repeated_ref rtv_;
        };

        unencoded_rope_view(unencoded_rope const * r, int lo, int hi) :
            ref_(rope_ref(r, lo, hi)),
            which_(which::r)
        {}

        ref ref_;
        which which_;

        friend struct unencoded_rope;
#endif
    };

    inline bool
    operator==(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    inline bool
    operator!=(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    inline bool
    operator<(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    inline bool
    operator<=(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    inline bool
    operator>(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    inline bool
    operator>=(unencoded_rope_view lhs, unencoded_rope_view rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /** Creates a new unencoded_rope containing the concatenation of lhs and
     * rhs. */
    inline unencoded_rope
    operator+(unencoded_rope_view lhs, unencoded_rope_view rhs);

}}

#include <boost/text/unencoded_rope.hpp>

namespace boost { namespace text {

    inline unencoded_rope_view repeated_string_view::
    operator()(int lo, int hi) const
    {
        return unencoded_rope_view(*this)(hi, lo);
    }

    inline unencoded_rope_view repeated_string_view::operator()(int cut) const
    {
        return unencoded_rope_view(*this)(cut);
    }

}}

#endif