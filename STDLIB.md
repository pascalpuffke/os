# Notes on the standard library

The C++ library used in this project deviates from the standard library in major ways.

Types are PascalCase instead of snake_case, and the `std::` prefix is omitted.

## Smart pointers

**UniquePtr<T>**: Single owner pointer, similar to `std::unique_ptr<T>`. The pointer is deleted when it goes out of scope.

Ownership can be transferred by `move`-ing the pointer somewhere else. It cannot be copied.

Header: `<stdlib/memory/unique_ptr.h>`

---

**SharedPtr<T>**: Reference counted pointer, similar to `std::shared_ptr<T>`. The pointer is deleted when the reference count reaches zero. Not yet implemented.

Header: `<stdlib/memory/shared_ptr.h>`

---

Instead of `std::make_{unique,shared}<T>(args...)`, use `{UniquePtr,SharedPtr}<T>::make(args...)`.

## Strings

Note that the current string implementations are horribly broken and a lot of the features are not yet implemented.

The library pretends like there is no language other than English and there are no character encodings other than ASCII.

**String**: A dynamically allocated, growable, owned string. Alias for `BasicString<char>`.

Header: `<stdlib/string.h>`

**StringView**: A non-owning view into another character sequence. Alias for `BasicStringView<char>`.

Cheap to copy, allocation-free, smart alternative to basic `const char*`s.

Header: `<stdlib/string_view.h>`