#pragma once
#include <variant>
#include <string>
#include <memory>

class Callable;

using Value = std::variant<double, bool, std::string, std::nullptr_t, std::shared_ptr<Callable>>;
