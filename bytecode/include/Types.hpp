// can also migrate the helper directory in interpreter/include/helper cusomcompielr directory so shares

#pragma once
#include <variant>
#include <string>
#include <memory>

class Callable;
class LoxInstance;

using Value = std::variant<double, bool, std::string, std::nullptr_t, std::shared_ptr<Callable>, std::shared_ptr<LoxInstance>>;