#include <tests/core/test_expr.hpp>

namespace ld::expr {

inline const TestExpr Identifier = {
    "x",                                                              /* text */
    {                                                               /* tokens */
        { ETk::Identifier }
    },
    {
        ld::Identifier{}
    }
};

inline const TestExpr IdentifierExtraSpaces = {
    "x ",                                                             /* text */
    {                                                               /* tokens */
        { ETk::Identifier },
        { ETk::Whitespace }
    },
    {                                                          /* syntax tree */
        ld::Identifier{}
    }
};

inline const TestExpr InvalidIdentifier = {
    "=",                                                              /* text */
    {                                                               /* tokens */
        { ETk::Invalid }
    }
};

inline const TestExpr Variable = {
    "x",                                                              /* text */
    {                                                               /* tokens */
        { ETk::Identifier }
    },
    {
        ld::Variable{}
    }
};

inline const TestExpr VariableExtraSpaces = {
    "x ",                                                             /* text */
    {                                                               /* tokens */
        { ETk::Identifier },
        { ETk::Whitespace }
    },
    {                                                          /* syntax tree */
        ld::Variable{}
    }
};

inline const TestExpr Application = {
    "(x y)",                                                          /* text */
    {                                                               /* tokens */
        { ETk::LeftParenthesis  },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::RightParenthesis }
    },
    {                                                          /* syntax tree */
        ld::Application{}, 0, 5, {
            { ld::Identifier{}, 1, 1 },
            { ld::Identifier{}, 3, 1 }
        }
    }
};

inline const TestExpr ApplicationExtraSpaces = {
    "(x   y ) ",                                                      /* text */
    {                                                               /* tokens */
        { ETk::LeftParenthesis  },
        { ETk::Identifier       },
        { ETk::Whitespace, 3    },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::RightParenthesis },
        { ETk::Whitespace       }
    },
    {                                                          /* syntax tree */
        ld::Application{}, 0, 8, {
            { ld::Identifier{}, 1, 1 },
            { ld::Identifier{}, 5, 1 }
        }
    }
};

inline const TestExpr InvalidApplication = {
    "(x)",                                                            /* text */
    {                                                               /* tokens */
        { ETk::LeftParenthesis  },
        { ETk::Identifier       },
        { ETk::RightParenthesis }
    }
};

inline const TestExpr Abstraction = {
    "\\x.x",                                                          /* text */
    {                                                               /* tokens */
        { ETk::Lambda     },
        { ETk::Identifier },
        { ETk::Dot        },
        { ETk::Identifier }
    },
    {                                                          /* syntax tree */
        ld::Abstraction{}, 0, 4, {
            { ld::Variable{},   1, 1 },
            { ld::Identifier{}, 3, 1 }
        }
    }
};

inline const TestExpr AbstractionExtraSpaces = {
    "\\ x .x ",                                                       /* text */
    {                                                               /* tokens */
        { ETk::Lambda        },
        { ETk::Whitespace    },
        { ETk::Identifier    },
        { ETk::Whitespace    },
        { ETk::Dot           },
        { ETk::Identifier    },
        { ETk::Whitespace    }
    },
    {                                                          /* syntax tree */
        ld::Abstraction{}, 0, 6, {
            { ld::Variable{},   2, 1 },
            { ld::Identifier{}, 5, 1 }
        }
    }
};

inline const TestExpr InvalidAbstraction = {
    "\\x(.x",                                                         /* text */
    {                                                               /* tokens */
        { ETk::Lambda  },
        { ETk::Identifier       },
        { ETk::LeftParenthesis },
        { ETk::Dot },
        { ETk::Identifier },
    }
};

inline const TestExpr One = {
    R"(\f.\x.(f x))",                                                 /* text */
    {                                                               /* tokens */
        { ETk::Lambda           },
        { ETk::Identifier       },
        { ETk::Dot              },
        { ETk::Lambda           },
        { ETk::Identifier       },
        { ETk::Dot              },
        { ETk::LeftParenthesis  },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::RightParenthesis }
    },
    {                                                          /* syntax tree */
        ld::Abstraction{}, 0, 11, {
            { ld::Variable{},    1, 1 },
            { ld::Abstraction{}, 3, 8, {
                { ld::Variable{},    4, 1 },
                { ld::Application{}, 6, 5, {
                    { ld::Identifier{}, 7, 1 },
                    { ld::Identifier{}, 9, 1 }
                }}
            }}
        }
    }
};

inline const TestExpr OneExtraSpaces = {
    R"(\ f  . \ x .  ( f x ) )",                                      /* text */
    {                                                               /* tokens */
        { ETk::Lambda           },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::Whitespace, 2    },
        { ETk::Dot              },
        { ETk::Whitespace       },
        { ETk::Lambda           },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::Dot              },
        { ETk::Whitespace, 2    },
        { ETk::LeftParenthesis  },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::Identifier       },
        { ETk::Whitespace       },
        { ETk::RightParenthesis },
        { ETk::Whitespace       }
    },
    {                                                          /* syntax tree */
        ld::Abstraction{}, 0, 21, {
            { ld::Variable{},    2, 1 },
            { ld::Abstraction{}, 7, 14, {
                { ld::Variable{},     9, 1 },
                { ld::Application{}, 14, 7, {
                    { ld::Identifier{}, 16, 1 },
                    { ld::Identifier{}, 18, 1 }
                }}
            }}
        }
    }
};

}