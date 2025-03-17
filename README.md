# LBNLCPPCommon
*A header-only C++ library for reusable utility functions*

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support

## 📌 Overview
**LBNLCPPCommon** is a **header-only** C++ library designed to provide **commonly used functions and utilities** across multiple projects. It eliminates code duplication by offering a centralized set of **generic templates and helper functions**.

- ✅ **Header-only** (no compilation needed)
- ✅ **Modern C++20 features** (concepts, ranges, `std::optional`)
- ✅ **Lightweight and easy to integrate** with CMake
- ✅ **No dependencies** required

## 📁 Folder Structure
\`\`\`
LBNLCPPCommon/
│── include/
│   ├── lbnl/
│   │   ├── algorithm.hxx
│   │   ├── optional.hxx
│── CMakeLists.txt
│── README.md
\`\`\`

## ⚙️ Installation & Usage

### 🔹 Using FetchContent in CMake
The recommended way to use **LBNLCPPCommon** is via **CMake FetchContent**, which automatically downloads and integrates it.

📌 **Add this to your CMakeLists.txt**:
\`\`\`cmake
include(FetchContent)

FetchContent_Declare(
    LBNLCPPCommon
    GIT_REPOSITORY https://github.com/LBNL-ETA/LBNLCPPCommon.git
    GIT_TAG main  # Use latest version or specify a tag
)

FetchContent_MakeAvailable(LBNLCPPCommon)

target_link_libraries(MyProject PRIVATE LBNLCPPCommon)
\`\`\`

### 🔹 Using Git Submodules
If your project is **version-controlled with Git**, you can add `LBNLCPPCommon` as a **submodule**.

\`\`\`sh
git submodule add https://github.com/LBNL-ETA/LBNLCPPCommon.git external/LBNLCPPCommon
git submodule update --init --recursive
\`\`\`

Then, modify **CMakeLists.txt**:
\`\`\`cmake
add_subdirectory(external/LBNLCPPCommon)
target_link_libraries(MyProject PRIVATE LBNLCPPCommon)
\`\`\`

## 📜 Example Usage
### 🔹 Finding an Element in a Container
\`\`\`cpp
#include <lbnl/algorithm.hxx>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    auto found = common_utils::findElement(numbers, [](int x) { return x > 3; });

    if (found) {
        std::cout << "Found: " << *found << '\n';  // Output: Found: 4
    }
}
\`\`\`

## 📌 Provided Utility Functions
| **Function** | **Description** | **Header** |
|-------------|----------------|------------|
| \`findElement(Container, Predicate)\` | Finds an element in a container using \`std::ranges::find_if\`. Returns \`std::optional<T>\`. | \`algorithm.hxx\` |

More utilities will be added in the future! 🚀

## 📖 License
This project is licensed under the Berkeley Lab License – see the [LICENSE](LICENSE) file for details.

## 📬 Contributing
We welcome contributions! Feel free to **open issues, suggest improvements, or submit pull requests**.

## 📧 Contact
For any questions or suggestions, please reach out to the **LBNL-ETA** team.

EOF
