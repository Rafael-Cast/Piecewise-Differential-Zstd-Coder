#pragma once

#include <memory>
#include <vector>
#include <random>
#include <functional>

template<typename T>
std::unique_ptr<std::vector<T>> copy_to_vector(T* data, size_t n)
{
    auto res = std::make_unique<std::vector<T>>();
    res->reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res->push_back(data[i]);
    }
    return res;
}

template<typename T>
std::unique_ptr<std::vector<T>> repeat(size_t n, T x)
{
    auto res = std::make_unique<std::vector<T>>();
    res->reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res->push_back(x);
    }
    return res;
}

template<typename T>
std::vector<T> repeat_vect(size_t n, T x)
{
    std::vector<T> res;
    res.reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res.push_back(x);
    }
    return res;
}

template<typename T>
std::unique_ptr<std::vector<T>> nats(size_t n)
{
    auto res = std::make_unique<std::vector<T>>();
    res->reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res->push_back(i);
    }
    return res;
}

template<typename T>
std::vector<T> nats_vect(size_t n)
{
    std::vector<T> res;
    res.reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res.push_back(i);
    }
    return res;
}

std::unique_ptr<std::vector<int16_t>> build_random_vector(int seed)
{
    std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> values_distribution(0, (1 << 16) - 1);
    std::uniform_int_distribution<> size_distibution(1, 1 << 20);
    size_t size = size_distibution(gen);
    auto input = std::make_unique<std::vector<int16_t>>();
    input->reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        int16_t random_value = values_distribution(gen);
        input->push_back(random_value);
    }

    return input;
}

template<typename T>
std::vector<T> trim_and_copy_vector(const std::vector<T> & xs, const size_t n)
{
    std::vector<T> res;
    res.reserve(n);
    for (size_t i = 0; i < n; i++)
    {
        res.push_back(xs[i]);
    }
    return res;
}

std::unique_ptr<std::vector<uint16_t>> build_random_vector_u(int seed)
{
    std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> values_distribution(0, (1 << 16) - 1);
    std::uniform_int_distribution<> size_distibution(1, 1 << 20);
    size_t size = size_distibution(gen);
    auto input = std::make_unique<std::vector<uint16_t>>();
    input->reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        uint16_t random_value = values_distribution(gen);
        input->push_back(random_value);
    }

    return input;
}

std::vector<uint16_t> build_random_vector_u_vect(int seed)
{
    std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> values_distribution(0, (1 << 16) - 1);
    std::uniform_int_distribution<> size_distibution(1, 1 << 20);
    size_t size = size_distibution(gen);
    std::vector<uint16_t> res;
    res.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        uint16_t random_value = values_distribution(gen);
        res.push_back(random_value);
    }

    return res;
}

std::vector<int16_t> build_random_vector_vect(int seed)
{
    std::mt19937 gen(seed); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> values_distribution(0, (1 << 16) - 1);
    std::uniform_int_distribution<> size_distibution(1, 1 << 20);
    size_t size = size_distibution(gen);
    std::vector<int16_t> res;
    res.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        int16_t random_value = values_distribution(gen);
        res.push_back(random_value);
    }

    return res;
}

/*
    T1 and T2 must be copy - constructible as the new vector will own copies of such an object
*/
template<typename T1, typename T2>
std::unique_ptr<std::vector<std::pair<T1,T2>>> product(const std::vector<T1> & xs, const std::vector<T2> & ys)
{
    auto res = std::make_unique<std::vector<std::pair<T1, T2>>>();
    res->reserve(xs.size() * ys.size());
    for (const auto x : xs)
    {
        for (const auto y : ys)
        {
            res->push_back({x, y});
        }
    }
    return res;
};

template<typename T1, typename T2>
std::vector<std::pair<T1,T2>> product_vect(const std::vector<T1> & xs, const std::vector<T2> & ys)
{
    std::vector<std::pair<T1, T2>> res;
    res.reserve(xs.size() * ys.size());
    for (const auto x : xs)
    {
        for (const auto y : ys)
        {
            res.push_back({x, y});
        }
    }
    return res;
}

template<typename T1, typename T2, typename TOut>
std::unique_ptr<std::vector<std::pair<T1, T2>>> productWith(const std::vector<T1> & xs, const std::vector<T2> & ys, std::function<TOut(const T1 &, const T2&)> f)
{
    auto res = std::make_unique<std::vector<TOut>>();
    res->reserve(xs.size() * ys.size());
    for (const auto & x : xs)
    {
        for (const auto & y : ys)
        {
            res->push_back(f(x, y));
        }
    }
    return res;
}

template<typename TIn, typename TOut>
std::unique_ptr<std::vector<TOut>> map(const std::vector<TIn> &xs, std::function<TOut(const TIn&)> f)
{
    auto res = std::make_unique<std::vector<TOut>>();
    res->reserve(xs.size());
    for (const auto &x : xs)
    {
        res->push_back(f(x));
    }
    return res;
}

template<typename TIn, typename TOut>
std::vector<TOut> map_ref(const std::vector<TIn> &xs, std::function<TOut(const TIn&)> f)
{
    std::vector<TOut> res;
    res.reserve(xs.size());
    for (const auto &x : xs)
    {
        res.push_back(f(x));
    }
    return res;
}

template<typename T>
std::vector<T> filter(const std::vector<T> &xs, std::function<bool(const T&)> f)
{
    std::vector<T> res;
    res.reserve(xs.size());
    for (const auto &x : xs)
    {
        if (f(x))
        {
            res.push_back(x);
        }
    }
    res.shrink_to_fit();
    return res;
}

template<typename T>
constexpr T round_up_division(T a, T b)
{
    const auto x = a / b;
    if (a % b != 0)
    {
        return x + 1;
    }
    else
    {
        return x;
    }
}

template<typename T>
T nearest_multiple_of_rounded_down(T x, T div)
{
    return (x / div) * div;
}

#define CONST_MANAGED_HEAP_VAR(varname, lhs) \
    const auto _varname_ptr = lhs;\
    const auto & varname = *_varname_ptr

template<typename T>
std::vector<T> key_buffer_to_unpacked_stl_vector(const uint8_t *data, const size_t number_of_samples)
{
    std::vector<T> res;
    res.reserve(number_of_samples);

    uint8_t keys_buffer;
    uint8_t shift = 8;
    for (size_t i = 0; i < number_of_samples; i++)
    {
        if (shift == 8)
        {
            keys_buffer = *data;
            shift = 0;
            data++;
        }
        res.push_back((keys_buffer >> shift) & 0x3);
        shift += 2;
    }
    return res;
}

template<typename T>
std::vector<T> small_data_buffer_to_unpacked_stl_vector(const uint8_t *data, const size_t number_of_samples)
{
    std::vector<T> res;
    res.reserve(number_of_samples);

    uint8_t keys_buffer;
    uint8_t shift = 8;
    for (size_t i = 0; i < number_of_samples; i++)
    {
        if (shift == 8)
        {
            keys_buffer = *data;
            shift = 0;
            data++;
        }
        res.push_back((keys_buffer >> shift) & 0xF);
        shift += 4;
    }
    return res;
}

template<typename T>
void extend_vector(std::vector<T> &target_vector, const std::vector<T> & data_vector)
{
    target_vector.reserve(target_vector.size() + distance(data_vector.begin(),data_vector.end()));
    target_vector.insert(target_vector.end(),data_vector.begin(),data_vector.end());
}

template<typename T>
std::vector<T> get_copy_of_subrange(const std::vector<T> & xs, const size_t start, const size_t end)
{
    const auto first = xs.begin() + start;
    const auto last = xs.begin() + end;
    std::vector<T> res(first, last);
    return res;
}

#define FAIL_COMPILE_IF(expr) static_assert(!(expr))