# Code Review: LBNLCPPCommon

**Date:** 2026-03-01
**Scope:** Full library review — headers, tests, build system, CI/CD, documentation
**Commit:** `e04e40f` (Fix Windows min/max macro conflict with std::min)

---

## 1. Project Overview

LBNLCPPCommon is a header-only C++20 utility library from Lawrence Berkeley National Laboratory (LBNL-ETA). It provides:

- **8 header files** in `include/lbnl/`
- **30 test files** with **159 test cases** (~1950 lines of test code)
- **6 documentation pages** in `docs/`
- **Zero external dependencies** (GoogleTest for testing only)
- **83 commits** by 2 contributors

The library targets a real gap: providing C++23-style monadic operations (`and_then`, `or_else`, `map`) for `std::optional` and `std::expected` while staying on C++20. It also bundles algorithm conveniences, map utilities, an enum mapper, and a memoization cache.

A key design principle is that several components are **intentional C++23 polyfills** — they provide C++23 standard library functionality on C++20, and are designed to be easily removed or replaced once the project migrates to C++23. This is documented and deliberate.

---

## 2. Strengths

### Clean header-only design

Every component is a single self-contained header with `#pragma once`. No `.cpp` files, no link-time dependencies, no configuration macros. Integration is a one-liner in CMake. This is exactly right for a utility library of this scope.

### Good modern C++20 usage

The library makes effective use of C++20 features:
- **Concepts** — `AssociativeContainer` (`map_utils.hxx:13`), `Enum` (`enum_index_mapper.hxx:14`), `std::predicate` (`algorithm.hxx:21`), `std::ranges::range` throughout
- **Ranges** — `std::ranges::find_if`, `std::ranges::sort`, `std::views::transform`
- **`constexpr`** — applied broadly and correctly
- **`[[nodiscard]]`** — used consistently on all query functions
- **Structured bindings, deduction guides, `std::decay_t`/`std::remove_cvref_t`**

### Thoughtful API for `ExpectedExt` and `OptionalExt`

The monadic chaining API is well-designed:
- `and_then` handles both wrapped and plain return types automatically (`expected.hxx:116-139`)
- Type stability is maintained through chains — `and_then` with a `T -> U` function correctly produces `ExpectedExt<U, E>`
- The `Unexpected<E>` wrapper disambiguates error construction, mirroring C++23's `std::unexpected`
- Pipe operators (`|` and `||`) provide concise syntax

### Comprehensive test suite

159 tests across 30 files is solid coverage for a library this size. The tests are well-structured:
- Each component has dedicated test files
- Edge cases are covered (empty containers, error paths, type stability)
- Thread safety is tested with 8 concurrent threads (`memoize.unit.cxx:93-127`)
- Type stability has its own dedicated test file (`expected.type_stability.unit.cxx`) with `static_assert` checks

### Excellent documentation

The README is well-organized with badges, folder structure, installation instructions, and quick examples. Each component has a dedicated doc page with full API reference, parameter descriptions, and realistic examples. The `ExpectedExt` docs even include a comparison table with exceptions. This is above average for a utility library.

### Consistent code formatting

A `.clang-format` config is present and consistently applied. Allman-style braces, 4-space indentation, 100-column limit, no tabs. The codebase reads uniformly.

### Deliberate C++23 polyfill strategy

Several components are designed as drop-in polyfills for C++23 standard library features, with a clear migration path:

| Component | C++23 Equivalent | Migration |
|-----------|-------------------|-----------|
| `OptionalExt<T>` with `and_then`, `map`, `or_else` | `std::optional::and_then`, `transform`, `or_else` | Remove `OptionalExt` wrapper, use `std::optional` directly |
| `ExpectedExt<T, E>` | `std::expected<T, E>` | Replace with `std::expected` |
| `Unexpected<E>` | `std::unexpected<E>` | Replace with `std::unexpected` |
| `lbnl::contains()` | `std::ranges::contains` | Drop-in replacement (comment at `algorithm.hxx:34` notes this) |
| `lbnl::to_vector()` | `std::ranges::to<std::vector>()` | Replace call sites |
| Pipe operators (`\|`, `\|\|`) | `std::optional` monadic ops | Use standard monadic chaining |

This is good engineering foresight. The APIs closely mirror the C++23 standard, so migration will be mechanical rather than architectural. The `OptionalExt` wrapper approach is particularly clean — it extends `std::optional` without inheriting from it, so dropping it requires only changing call sites, not data structures.

The remaining components (`algorithm.hxx` utilities, `map_utils`, `EnumIndexMapper`, `LazyEvaluator`) are **not** polyfills — they are custom convenience utilities that will remain useful regardless of C++ standard version.

---

## 3. Design Issues

### 3.1 `ExpectedExt<T, E>` breaks when `T == E`

**Severity: High**

`ExpectedExt` stores its state in `std::variant<T, E>` (`expected.hxx:186`). When `T` and `E` are the same type (e.g., `ExpectedExt<std::string, std::string>`), `std::variant<std::string, std::string>` has two identical alternatives. Calls to `std::holds_alternative<T>`, `std::get<T>`, and `std::get<E>` become ambiguous — they won't compile or will always resolve to the first alternative.

The `Unexpected<E>` wrapper was added to disambiguate *construction* (see `expected.hxx:67`), but it doesn't fix the underlying variant storage problem. `has_value()`, `value()`, and `error()` all use type-based variant access:

```cpp
// expected.hxx:74-101
constexpr bool has_value() const noexcept { return std::holds_alternative<T>(m_data); }
constexpr const T & value() const { return std::get<T>(m_data); }
constexpr const E & error() const { return std::get<E>(m_data); }
```

**Fix:** Replace `std::variant<T, E>` with either:
- A `bool` flag + `std::variant` accessed by index (`std::get<0>` / `std::get<1>`)
- A tagged union with explicit discriminant
- Or simply adopt `std::expected` (C++23) as a polyfill target

The documentation (`docs/expected.md:46-53`) even shows a `T == E` example, suggesting this is an intended use case, which makes the bug more concerning.

### 3.2 LazyEvaluator holds mutex during computation

**Severity: Medium**

`LazyEvaluator::get()` acquires the mutex *before* calling the generator and holds it through the entire computation (`memoize.hxx:26-32`):

```cpp
const Value & get(const Key & key) {
    std::lock_guard lock(m_Mutex);       // locked here
    auto [it, inserted] = m_Cache.try_emplace(key);
    if (inserted) {
        it->second = m_Generator(key);   // generator runs under lock
    }
    return it->second;
}
```

If the generator is slow (file I/O, network, heavy computation — the documented use cases), *all* threads block on *any* key lookup while *any* computation is in progress. For a cache meant to accelerate expensive operations, this is counterproductive.

The test at `memoize.unit.cxx:93-127` validates that only one computation occurs, but it also means 7 of 8 threads are blocked for 50ms waiting for a single key.

**Fix:** Use per-key locking or a `std::shared_mutex` with `std::shared_lock` for cache hits and `std::unique_lock` only for inserts. Alternatively, compute outside the lock and insert the result.

### 3.3 `operator()` vs `get()` return type inconsistency

**Severity: Low**

In `LazyEvaluator` (`memoize.hxx:19-23`):

```cpp
Value operator()(const Key & key)       // returns by VALUE (copy)
const Value & get(const Key & key)      // returns by REFERENCE
```

`operator()` calls `get()` but returns by value, silently copying the cached result. `get()` returns a const reference. Two methods that do the same thing should not have different return semantics — this is surprising and can cause subtle performance issues with large cached values.

### 3.4 `merge()` as a convenience wrapper over `std::ranges::merge`

**Severity: Low**

`lbnl::merge()` (`algorithm.hxx:189-219`) reimplements the merge algorithm manually. Unlike the polyfill components, `std::ranges::merge` is already available in C++20, so this is not a polyfill — it's a convenience wrapper that returns a `std::vector` directly rather than requiring output iterators. That convenience is reasonable, but the implementation doesn't reserve output capacity and uses `<=` instead of `<`, which differs from the standard's strict weak ordering convention. Consider delegating to `std::ranges::merge` internally while keeping the vector-returning API.

### 3.5 `unique()` silently sorts — name is misleading

**Severity: Low**

`lbnl::unique()` (`algorithm.hxx:52-62`) sorts the input before removing duplicates. The name suggests it only removes duplicates (like `std::unique`), but it also reorders elements. The doc comment does mention this, but a name like `sorted_unique` or `deduplicate` would prevent misuse.

### 3.6 `optional_pipe_import.hxx` pollutes the global namespace

**Severity: Medium**

```cpp
// optional_pipe_import.hxx:6-7
using lbnl::operator|;
using lbnl::operator||;
```

These `using` declarations are at file scope in a header file. Any translation unit that includes this header gets `operator|` and `operator||` injected into the global namespace. This can conflict with other libraries' pipe operators (e.g., range-v3, RxCpp, or user-defined operators). For a utility library designed to be consumed by other projects, this is risky.

The header's existence is intentional (opt-in import), but users may not realize the scope of the side effect. A comment warning about this or a namespace-scoped alternative would be prudent.

---

## 4. Build System & CI Issues

### 4.1 `file(GLOB ...)` for test sources

**Severity: Medium**

`CMakeLists.txt:40`:
```cmake
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tst/*.cxx)
```

CMake's `file(GLOB)` captures files at configure time. Adding or removing test files requires a manual reconfigure — the build won't pick up changes automatically. CMake's own documentation recommends against this pattern. Listing files explicitly is more reliable.

### 4.2 `.clang-format` says `Standard: Cpp11` while code is C++20

**Severity: Low**

`.clang-format:86`:
```yaml
Standard: Cpp11
```

The code uses C++20 features (concepts, ranges, `constexpr` lambdas, structured bindings). The format standard should be `Cpp20` or `Latest` to ensure the formatter handles modern syntax correctly (e.g., `requires` clauses, spaceship operator).

### 4.3 Linux CI aarch64 job installs QEMU but never cross-compiles

**Severity: Medium**

The Linux CI job (`build.yml:62-92`) matrices over `[x86_64, aarch64]` and installs QEMU for aarch64 emulation, but then runs the native `cmake` and `g++` without any cross-compilation toolchain. Both matrix entries compile for the host architecture (x86_64), so the aarch64 job is wasted CI time — it just runs the same x86_64 build twice.

To actually cross-compile, you'd need an aarch64 cross-compiler toolchain file or a Docker container with an aarch64 sysroot.

### 4.4 `ctest -C RELEASE` case mismatch

**Severity: Low**

`build.yml:34,59,92`:
```yaml
run: ctest -C RELEASE -V
```

The build type is `Release` (line 7: `BUILD_TYPE: Release`), but CTest is invoked with `-C RELEASE`. On multi-config generators (Visual Studio), this is case-sensitive. While MSVC tends to be forgiving here, it's technically a mismatch and could cause CTest to not find the test binary on some configurations.

### 4.5 Contradictory MSVC runtime library settings

**Severity: Medium**

`CMakeLists.txt:7` sets the runtime to **static** (`/MT`):
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

But `CMakeLists.txt:53` sets the test target to **dynamic** (`/MD`):
```cmake
target_compile_options(LBNLCPPCommonTests PRIVATE /MD$<$<CONFIG:Debug>:d>)
```

Mixing `/MT` and `/MD` in the same build can cause linker errors or runtime crashes. Since `gtest_force_shared_crt` is also set to `ON` (line 34), which forces GoogleTest to use `/MD`, the intent is likely `/MD` everywhere — but the global `CMAKE_MSVC_RUNTIME_LIBRARY` contradicts this.

### 4.6 No CMake install targets or project version

**Severity: Low**

The `project()` call has no `VERSION`:
```cmake
project(LBNLCPPCommon LANGUAGES CXX)
```

There are no `install()` commands, so the library can only be consumed via `add_subdirectory()` or `FetchContent`. Adding install targets and a `LBNLCPPCommonConfig.cmake` would allow `find_package()` integration.

---

## 5. Code-Level Observations

### 5.1 No `noexcept` specifications

None of the functions in `algorithm.hxx`, `map_utils.hxx`, or `optional_utils.hxx` are marked `noexcept`. Functions like `contains()`, `filter()`, and `map_keys()` that only read data could benefit from `noexcept` — it enables compiler optimizations and documents the exception contract. (Note: `has_value()` and `operator bool()` in `OptionalExt` and `ExpectedExt` *are* correctly marked `noexcept`.)

### 5.2 `flatten()` doesn't pre-reserve

`flatten()` (`algorithm.hxx:282-290`) inserts into the result vector in a loop without reserving total capacity upfront. For large nested vectors, this causes multiple reallocations:

```cpp
template<typename T>
constexpr std::vector<T> flatten(const std::vector<std::vector<T>> & nested) {
    std::vector<T> result;
    // No reserve — could sum inner sizes first
    for (const auto & inner : nested) {
        result.insert(result.end(), inner.begin(), inner.end());
    }
    return result;
}
```

### 5.3 `average_optional` uses `static_cast<T>(count)` which truncates for integers

`optional_utils.hxx:39`:
```cpp
return sum / static_cast<T>(count);
```

When `T` is an integer type, this performs integer division. The average of `{1, 2}` returns `1`. The doc comment properly warns about this, which is good — but a `static_assert` or concept requiring floating-point types would prevent silent truncation.

### 5.4 `merge()` doesn't `reserve()` output capacity

`algorithm.hxx:192` — the result vector grows incrementally. Since both input sizes are known, a `reserve(size1 + size2)` would eliminate reallocations.

### 5.5 Windows `min`/`max` macro workaround

`algorithm.hxx:80`:
```cpp
result.reserve((std::min)(std::ranges::size(r1), std::ranges::size(r2)));
```

The parentheses around `std::min` prevent Windows `min`/`max` macro expansion. This is a known workaround, and it's correctly applied. An alternative would be `#define NOMINMAX` before Windows headers, but the parenthesization approach is less invasive for a header-only library.

### 5.6 `map_utils.hxx` missing closing namespace comment

`map_utils.hxx:78` ends with just `}` instead of `} // namespace lbnl`. Minor inconsistency with the other headers which all include the namespace comment.

---

## 6. What Works Well (Summary)

| Aspect | Assessment |
|--------|------------|
| Header-only, zero dependencies | Excellent — easy integration |
| C++20 concepts and ranges | Good use of modern features |
| `[[nodiscard]]` / `constexpr` | Consistently applied |
| Monadic API (`and_then`, `map`, `or_else`) | Well-designed, type-stable |
| Test coverage (159 tests) | Solid, includes edge cases and threading |
| Documentation | Above average — full API docs with examples |
| Thread-safe memoization | Present and tested (see caveats above) |
| Code formatting consistency | Uniform throughout |
| Cross-platform CI | Windows, macOS, Linux |

---

## 7. Prioritized Recommendations

### High Priority

1. **Fix `ExpectedExt<T, E>` variant storage** — Use index-based access (`std::get<0>`/`std::get<1>`) or a bool discriminant so `T == E` works correctly.

2. **Fix MSVC runtime library conflict** — Choose `/MT` or `/MD` and apply it consistently to the project, tests, and GoogleTest.

3. **Fix Linux aarch64 CI** — Either add a proper cross-compilation toolchain or remove the aarch64 matrix entry to avoid false confidence.

### Medium Priority

4. **Reduce lock scope in `LazyEvaluator`** — Compute outside the mutex or use per-key synchronization.

5. **List test sources explicitly in CMake** — Replace `file(GLOB)` with an explicit file list.

6. **Document `optional_pipe_import.hxx` namespace impact** — Add a prominent warning that including this header injects operators into the global namespace.

### Low Priority

7. **Rename `unique()` to `sorted_unique()`** — Make the sorting side-effect obvious in the name.

8. **Add `reserve()` to `flatten()` and `merge()`** — Sum input sizes before inserting.

9. **Fix `ctest -C RELEASE` to `ctest -C Release`** — Match the actual build configuration name.

10. **Update `.clang-format` `Standard` to `Cpp20`** — Align formatter with actual language standard.

11. **Add `noexcept`** to pure query functions like `contains()`, `map_keys()`.

12. **Make `operator()` and `get()` consistent** in `LazyEvaluator` — both should return `const Value &`.

13. **When moving to C++23**, drop `OptionalExt`, `ExpectedExt`, `contains()`, and `to_vector()` in favor of their standard equivalents. The polyfill APIs already mirror the standard closely, so migration should be mechanical. Consider adding a migration guide to `docs/` when the time comes.

---

## 8. Overall Assessment

LBNLCPPCommon is a well-crafted utility library that fills a genuine need for C++20 projects wanting monadic optional/expected operations. The code is clean, well-documented, and well-tested. The design decisions are mostly sound — header-only, zero dependencies, consistent use of modern C++ idioms.

The deliberate C++23 polyfill strategy is a notable strength. `OptionalExt`, `ExpectedExt`, `contains()`, and `to_vector()` closely mirror their C++23 standard counterparts, providing a clear and mechanical migration path when the project moves to C++23. This is forward-thinking design — the library provides value today without creating technical debt for tomorrow.

The main concerns are the `ExpectedExt` variant storage bug (which can cause compilation failures in a documented use case), the mutex contention in `LazyEvaluator` (which undermines its purpose for the slow-computation cases it's designed for), and the CI configuration issues (which mean the aarch64 coverage is illusory).

None of these are showstoppers for the library's current users, but the `ExpectedExt<T, E>` issue in particular should be addressed before the library is promoted for wider adoption.
