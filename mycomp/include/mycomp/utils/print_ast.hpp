#pragma once

#include "../ast_visitor.hpp"

namespace mycomp {

std::unique_ptr<AstVisitor> make_ast_printer();

}
