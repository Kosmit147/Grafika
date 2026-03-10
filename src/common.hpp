#pragma once

#include <concepts>
#include <cstdint>
#include <utility>
#include <functional>

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;
using isize = std::intmax_t;
using usize = std::uintmax_t;

using f32 = float;
using f64 = double;

template<std::regular_invocable Func> class Defer {
public:
    explicit Defer(Func&& func) : _func{ std::forward<decltype(func)>(func) } {}

    ~Defer() {
        if (!_dismissed) std::invoke(_func);
    }

    Defer(const Defer&) = delete;
    auto operator=(const Defer&) = delete;
    Defer(Defer&&) = delete;
    auto operator=(Defer&&) = delete;

    auto dismiss() -> void { _dismissed = true; }

private:
    Func _func;
    bool _dismissed = false;
};