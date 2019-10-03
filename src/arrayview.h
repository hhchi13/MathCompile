// Compiler from Wolfram Language to C++
// 
// Copyright 2019 Tianhuan Lu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include "types.h"
#include "traits.h"
#include "const.h"

namespace wl
{

template<bool UnitStep>
struct indexer_iter;

template<>
struct indexer_iter<false>
{
    size_t index_;
    ptrdiff_t step_;

    indexer_iter(size_t index, ptrdiff_t step) :
        index_{index}, step_{step}
    {
    }

    auto& operator++()
    {
        index_ += step_;
        return *this;
    }

    bool operator==(const indexer_iter& other) const
    {
        return this->index_ == other.index_;
    }
};

template<>
struct indexer_iter<true>
{
    size_t index_;

    explicit indexer_iter(size_t index) :
        index_{index}
    {
    }

    auto& operator++()
    {
        ++index_;
        return *this;
    }

    bool operator==(const indexer_iter& other) const
    {
        return this->index_ == other.index_;
    }
};

struct all_indexer
{
    size_t size_;

    explicit all_indexer(size_t size) :
        size_{size}
    {
    }
};

struct unit_indexer
{
    size_t begin_;
    size_t size_;

    unit_indexer(size_t begin, size_t size) :
        begin_{begin}, size_{size}
    {
    }

    auto begin() const
    {
        return indexer_iter<true>(begin_);
    }

    auto end() const
    {
        return indexer_iter<true>(begin_ + size_);
    }
};

struct step_indexer
{
    size_t begin_;
    ptrdiff_t step_;
    size_t size_;

    step_indexer(size_t begin, ptrdiff_t step, size_t size) :
        begin_{begin}, step_{step}, size_{size}
    {
    }

    auto begin() const
    {
        return indexer_iter<false>(begin_, step_);
    }

    auto end() const
    {
        return indexer_iter<false>(begin_ + step_ * size_, step_);
    }
};

template<typename IndexType>
size_t convert_index(const IndexType& idx, const size_t& dim)
{
    if constexpr (std::is_unsigned_v<IndexType>)
    {
        if (1u <= idx && idx <= dim)
            return size_t(idx - 1u);
        else
            throw std::logic_error("badindex");
    }
    else
    {
        ptrdiff_t pos_idx = idx >= 0 ?
            idx : idx + ptrdiff_t(dim) + 1;
        if (1 <= pos_idx && pos_idx <= ptrdiff_t(dim))
            return size_t(pos_idx - 1u);
        else
            throw std::logic_error("badindex");
    }
};

struct list_indexer
{
    std::vector<size_t> indices_;

    template<typename Iter>
    list_indexer(Iter idx_begin, Iter idx_end, size_t level_dim)
    {
        using IndexType = remove_cvref_t<decltype(*idx_begin)>;
        indices_.resize(idx_end - idx_begin);
        std::transform(idx_begin, idx_end, indices_.begin(),
            [&](const auto& idx) { return convert_index(idx, level_dim); });
    }
};

template<typename Begin, typename End, typename Step>
struct span
{
    static constexpr auto default_begin = std::is_same_v<Begin, all_type>;
    static constexpr auto default_end = std::is_same_v<End, all_type>;
    static constexpr auto default_step = std::is_same_v<Step, all_type>;

    static_assert(default_begin || is_integral_v<Begin>, "badargtype");
    static_assert(default_end || is_integral_v<End>, "badargtype");
    static_assert(default_step || is_integral_v<Step>, "badargtype");

    Begin begin_;
    End end_;
    Step step_;

    span(Begin begin, End end, Step step) :
        begin_{begin}, end_{end}, step_{step}
    {
    }

    auto to_indexer(size_t dim) const
    {
        if constexpr (default_begin && default_end && default_step)
            return all_indexer(dim);

        // convert WL-style index to C-style index
        ptrdiff_t begin = 1;
        ptrdiff_t end = ptrdiff_t(dim);
        ptrdiff_t step = 1;
        if constexpr (!default_begin)
        {
            if constexpr (std::is_unsigned_v<Begin>)
                begin = ptrdiff_t(this->begin_);
            else if (this->begin_ >= 0)
                begin = ptrdiff_t(this->begin_);
            else // this->begin_ < 0
                begin = ptrdiff_t(this->begin_) + ptrdiff_t(dim + 1u);
            //check out-of-bound
            if (begin < 1 || begin > ptrdiff_t(dim))
                throw std::logic_error("badindex");
        }
        if constexpr (!default_end)
        {
            if constexpr (std::is_unsigned_v<End>)
                end = ptrdiff_t(this->end_);
            else if (this->end_ >= 0)
                end = ptrdiff_t(this->end_);
            else // this->end_ < 0
                end = ptrdiff_t(this->end_) + ptrdiff_t(dim + 1u);
            //check out-of-bound
            if (end < 1 || end > ptrdiff_t(dim))
                throw std::logic_error("badindex");
        }
        if constexpr (!default_step)
        {
            step = step_;
            if (step == 0)
                throw std::logic_error("badstep");
        }

        if constexpr (default_step)
        {
            if constexpr (default_begin || default_end)
            {
                return unit_indexer(
                    size_t(begin - 1),
                    size_t(end - begin + 1));
            }
            else
            {
                return unit_indexer(
                    size_t(begin - 1),
                    end >= begin ? size_t(end - begin + 1) : 0u);
            }
        }
        else if (step > 0)
        {
            return step_indexer(
                size_t(begin - 1),
                end >= begin ? size_t((end - begin) / step) + 1u : 0u,
                step);
        }
        else // step < 0
        {
            return step_indexer(
                size_t(begin),
                end <= begin ? size_t((begin - end) / -step) + 1u : 0u,
                step);
        }
    }
};

template<typename Begin, typename End, typename Step>
auto make_span(const Begin& begin, const End& end, const Step& step)
{
    return span<Begin, End, Step>(begin, end, step);
}

template<typename Begin, typename End>
auto make_span(const Begin& begin, const End& end)
{
    return make_span(begin, end, const_all);
}

template<typename Begin, typename End, typename Step>
auto make_indexer(const span<Begin, End, Step>& s, size_t dim)
{
    return s.to_indexer(dim);
}

template<typename IndexType, size_t Rank>
auto make_indexer(const ndarray<IndexType, Rank>& list, size_t dim)
{
    static_assert(Rank == 1u, "badargtype");
    static_assert(is_integral_v<IndexType>, "badidxtype");
    return list_indexer(list.begin(), list.end(), dim);
}

template<typename IndexType>
auto make_indexer(const IndexType& index, size_t dim)
{
    if constexpr (std::is_same_v<IndexType, all_type>)
        return all_indexer(dim);
    else
        return convert_index(index, dim);
}


template<typename T, size_t ArrayRank, size_t ViewRank, bool Const>
struct simple_view
{
    static constexpr auto is_const = Const;
    static constexpr auto array_rank = ArrayRank;
    static constexpr auto view_rank = ViewRank;
    static constexpr auto rank = ViewRank;

    using value_type = T;
    using array_ref_type = std::conditional_t<
        is_const,
        const ndarray<T, ArrayRank>&,
        ndarray<T, ArrayRank>&>;
    using pointer_type = std::conditional_t<is_const, const T*, T*>;
    using _dims_t = std::array<size_t, view_rank>;

    pointer_type data_;
    const size_t* dims_;

    template<typename... Specs>
    simple_view(array_ref_type base, const Specs&... specs) :
        data_{base.data() + _get_offset(0u, base.dims_ptr(), specs...)},
        dims_{base.dims_ptr() + ArrayRank - ViewRank}
    {
        static_assert(sizeof...(Specs) == ArrayRank, "internal");
    }

    template<typename Spec1, typename... Specs>
    auto _get_offset(const size_t offset, const size_t* dims,
        const Spec1& spec1, const Specs&... specs)
    {
        static_assert(std::is_same_v<Spec1, size_t> ||
            std::is_same_v<Spec1, all_indexer>, "internal");
        if constexpr (sizeof...(Specs) > 0u)
        {
            if constexpr (std::is_same_v<Spec1, size_t>)
            {
                static_assert(sizeof...(Specs) >= ViewRank, "internal");
                return _get_offset(
                    offset * (*dims) + spec1, dims + 1u, specs...);
            }
            else
            {
                static_assert(sizeof...(Specs) < ViewRank, "internal");
                return _get_offset(offset * (*dims), dims + 1u, specs...);
            }
        }
        else
        {
            if constexpr (std::is_same_v<Spec1, size_t>)
            {
                static_assert(sizeof...(Specs) >= ViewRank, "internal");
                return offset * (*dims) + spec1;
            }
            else
            {
                static_assert(sizeof...(Specs) < ViewRank, "internal");
                return offset * (*dims);
            }

        }
    }

    const size_t* dims_ptr() const
    {
        return this->dims_;
    }

    auto begin() const
    {
        return this->data_;
    }

    auto cbegin() const
    {
        return static_cast<const T*>(this->data_);
    }
};


}
