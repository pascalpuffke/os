#pragma once

// TRY() macro, similar to SerenityOS. Also comparable to Rust's '?' operator.
// Designed to work with the Result<T, E> type.
// Requires the type to implement 'has_value()', 'value()' and 'error() ' methods
// and the function it's called in to return a supported type.
#define TRY(expr) ({                           \
    auto _try_result = (expr);                 \
    if (!_try_result.has_value()) [[unlikely]] \
        return _try_result.error();            \
    _try_result.value();                       \
})
