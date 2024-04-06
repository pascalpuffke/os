# Stupid Operating System

_Welcome to hell._

![QEMU screenshot](screenshot.png)

## TODO list

- [ ] Move to a proper build system (cmake)
- [ ] More robust printf
  - [ ] Fix bug: %d _sometimes_ prints 0?
  - [ ] More specifiers
  - [ ] Flags: #, 0, -, +, ' '
  - [ ] Width: %5d, %05d, %.5d, %5.d
  - [ ] Precision: %.5d, %.0d, %.d
  - [ ] Color escape sequences
  - [ ] sprintf, snprintf
- [x] CPUID
- [ ] Interrupts (IDT; controller: PIC, APIC, ?)
- [ ] Paging and everything related to it
- [ ] PS/2 keyboard input
- [ ] Processes
- [ ] Preemptive multitasking
- [ ] VFS
- [ ] Disk drivers (IDE/SCSI, maybe NVMe later)
- [ ] File system (fat32, ext2, iso9660?)
- [ ] Syscalls
- [ ] ELF loader
- [ ] PCI
- [ ] Everything userspace, whatever that may contain (perhaps it would be best to simply use already existing projects)
  - [ ] Libraries, GUI, shell...
  - [ ] coreutils: ls, cat, cp, mv, rm, mkdir, touch, chmod, chown, stat, etc.
- [ ] Networking (that will be fun)
- [ ] SMP (lmao jk... unless?)
- [ ] Linux ABI??? Maybe? One can dream...
- [ ] x86_64
- [ ] A full standard library (highest performance is explicitly not a goal, neither is 1:1 compatibility)
  - [ ] Aim for C++20-like std, with some additions here and there
  - [ ] Add C++23 features if they make sense (like basic_string::contains())

## Documentation

Documentation is automatically generated from the source code using [doxygen](https://www.doxygen.nl/index.html).

**Always use javadoc-style comments** for documentation.

`@brief` statements are unnecessary, as doxygen is configured with `JAVADOC_AUTOBRIEF` enabled:
it will automatically use the first sentence of the comment as the brief description.

**Document all public methods and classes**, unless their purpose and behavior is (very) obvious.
For methods, these exceptions may include property get/set, simple operator overloads (e.g. `operator==`) and ctor/dtors.

**When writing stdlib code**, insert cppreference links as `@see` tags if applicable.

```cpp
/**
* Constructs the string with count copies of character ch.
* @see https://en.cppreference.com/w/cpp/string/basic_string/basic_string (2)
* @param count number of characters to copy
* @param ch character to copy
*/
constexpr BasicString(size_type count, value_type ch)
  : m_allocator(allocator_type())
{
  assign(count, ch);
}
```

## Code style

See [STYLE.md](STYLE.md)
