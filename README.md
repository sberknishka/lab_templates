# Lab Assignment: C++ Templates

## Student Info

| Field     | Value               |
|-----------|---------------------|
| **Name**  | Щащаев Али          |
| **Group** | РПОиПИИ             |

> **Replace the data above with your own!**

---

## Description

Implementation of three template classes: `Array<T>`, `Optional<T>`, and `UniquePtr<T, Deleter>`.

## How to Work with This Repo

### 1. Fork the repository

Click the **Fork** button in the top-right corner of this page.

### 2. Clone your fork

```bash
git clone https://github.com/<your-username>/templates-lab.git
cd templates-lab
```

### 3. Enable GitHub Actions

Go to the **Actions** tab in your fork and click
**"I understand my workflows, go ahead and enable them"**.

### 4. Implement the classes

Since these are template classes, all implementations go in the header files. Each header contains the class declaration with a TODO section at the bottom where you write the method definitions.

**Files to implement:**
- `array.h` -- fixed-size heap-allocated array
- `optional.h` -- optional value wrapper (no heap allocation)
- `unique_ptr.h` -- owning smart pointer with custom deleter

**Rules:**
- You **must not** modify the public interface of the classes.
- You **may** modify the private sections (e.g. member attributes, helper methods).
- You **must not** modify `tests.cpp`, `tracked.h`, or any test files.
- Implementation goes below the TODO comment in each header.

### 5. Build and test locally

```bash
cmake -B build
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

### 6. Push and check CI

```bash
git add .
git commit -m "Implement templates"
git push
```

Go to the **Actions** tab of your fork -- a test run will appear there.
Green checkmark -- all tests passed. Red cross -- there are errors.

### 7. Fill in the README

Enter your name and group in the table above.

---

## Build Details

The project is compiled with strict flags:

```
-Wall -Wextra -Wpedantic -Werror
-fsanitize=address,undefined
```

All warnings are treated as compilation errors. AddressSanitizer and UndefinedBehaviorSanitizer are enabled at runtime. Make sure your code is warning-free and has no memory errors.

## Project Structure

```
array.h                    # Array<T> (implement here)
optional.h                 # Optional<T> (implement here)
unique_ptr.h               # UniquePtr<T, Deleter> (implement here)
tracked.h                  # Test helper (DO NOT modify)
array_tests.cpp            # Array tests (DO NOT modify)
optional_tests.cpp         # Optional tests (DO NOT modify)
unique_ptr_tests.cpp       # UniquePtr tests (DO NOT modify)
CMakeLists.txt             # Build config (DO NOT modify)
.github/workflows/
  test.yml                 # CI configuration
README.md                  # This file
```

## Deadline

**Deadline:** _TBD_

The grade is based on the build and test results at the time of the deadline.
