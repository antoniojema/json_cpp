#pragma once

#include <variant>
#include <chrono>
#include <format>
#include <array>

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T> concept Floating = std::is_floating_point_v<T>;
template <typename T> concept Integral = std::is_integral_v<T> && !std::is_same_v<T,bool> && !std::is_same_v<T,char*> && !std::is_same_v<T,const char*>;

template <typename I, typename F>
class Number {
public:
    using Integer = I;
    using Float = F;

    constexpr Number() : _value((Integer)0) {}

    template <Integral T> constexpr Number(T val) : _value((Integer)val) {}
    template <Floating T> constexpr Number(T val) : _value((Float)val) {}

    constexpr bool is_integer() const { return this->_value.index() == 0; }
    constexpr bool is_float() const { return this->_value.index() == 1; }

    template<typename T> T& get() { return std::get<T>(this->_value); }
    template<typename T> const T& get() const { return std::get<T>(this->_value); }

private:
    std::variant<Integer, Float> _value;
};
