# Coding style

Please use `clang-format` with the preconfigured `.clang-format` file before committing.

Use **WebKit** style for C++ code with **west const**.

```cpp
// Do this:
const T& reference = get_reference();
const T* pointer = get_pointer();
// NOT this:
T const& reference = get_reference();
T const* pointer = get_pointer();
```

## Classes, structs, enums

**Naming convention**: PascalCase. Exception: standard library, uses lower_snake_case.

```cpp
enum class MultibootFlag;
```

**Property getter and setters**: Prefix setters with `set_`, but omit `get_` for the getter.

Mark getters as `[[nodiscard]]` by default, unless not doing so makes much more sense.

```cpp
class Foo final {
public:
	// ...
	void set_bar(int bar) { m_bar = bar; }
	[[nodiscard]] int bar() const { return m_bar; }
	
private:
	int m_bar { 0 };
};
```

**For classes**, always use explicit getters and setters. **For structs**, accessing the members directly is recommended.

Prefer **enum class** over bare enums.

## Macros, global constants

**Naming convention**: SCREAMING_SNAKE_CASE.

```cpp
#define MY_CONSTANT 42
static constexpr auto MY_CONSTANT = 42;
```

**Always prefer `[inline] constexpr` functions over preprocessor macros when possible.**

Macros are okay, as long as:
- they are used carefully,
- increase code readability,
- are #undef-d as soon as they are no longer needed.

### Bad macro usage

```cpp
// Classic example of a bad macro that should have been a function:
#define min(a, b) ((a) < (b) ? (a) : (b))
// The macro may behave unexpectedly, as the arguments are evaluated twice.
// Instead, use generic `inline constexpr` functions:
template <typename T>
inline constexpr T min(T a, T b) {
    return a < b ? a : b;
}
```

### Good macro usage

```cpp
// Example of a somewhat good macro in src/kernel/processor/cpuid.cpp:
bool CPUID::has_feature(CPUFeature feature)
{
    get(CPUIDRequest::GET_FEATURES);

#define FEATURE(feat, reg, bit) \
    case CPUFeature::feat:      \
        return (reg & (1 << bit)) != 0;

    switch (feature) {
        FEATURE(FPU, m_edx, 0)
        FEATURE(VME, m_edx, 1)
        FEATURE(DE, m_edx, 2)
        FEATURE(PSE, m_edx, 3)
        // ...
    }
#undef FEATURE
}
// The macro helps readability and turns two lines into one.
// As it won't be needed outside this function, we #undef the macro.

```

**Same goes for constants. Prefer `static constexpr auto CONSTANT = 42;` over `#define CONSTANT 42`.**

snake_case for most other identifiers.

```cpp
int square(int x) {
    return x * x;
}

int MyClass::return_value() {
    return 42;
}
```

## Types

**Use <stdlib/types.h> aliases.**

Exceptions:
- `const char*` for C-style strings (consider using StringView). Prefer `u8` when handling raw bytes.
- `int` for signed, 32-bit integers.

Use `[const] auto` whenever possible.
