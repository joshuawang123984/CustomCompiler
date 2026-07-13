# C++ Custom Compiler 

A custom-built compiler project focused on implementing a recursive descent parser and an expression evaluator from scratch. This project contains fundamental stages of compilation: Lexing, Parsing, and Evaluation. Inspired by CraftingInterpreters by Robert Nystrom.

## Project Architecture
This project follows the standard compiler design pipeline:

*   **Lexer (Scanner):** Converts raw source code into a stream of categorized tokens.
*   **Parser:** Constructs an Abstract Syntax Tree (AST) while enforcing operator precedence (PEMDAS).
*   **Evaluator:** Traverses the AST to execute the operations defined in the source.

## Technical Stack
*   **Language:** C++17
*   **Build System:** GNU Make

## Getting Started

### Prerequisites
*   A C++17 compatible compiler (g++ or clang).
*   GNU Make.

### Building the Project
1. Clone the repository.
2. Run `make` to compile the source:
   ```bash
   make