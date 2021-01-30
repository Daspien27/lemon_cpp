#pragma once

#define _CRT_SECURE_NO_DEPRECATE

enum class Boolean
{
    LEMON_FALSE = 0,
    LEMON_TRUE = 1
};

namespace Rule
{
struct rule;
}
using namespace Rule;

struct lemon;

namespace Action
{
struct action;

static action* Action_new(void);
static action* Action_sort(action*);
}
using namespace Action;

namespace Config
{
struct config;
}
using namespace Config;

namespace Configlist
{
void Configlist_init(void);
config* Configlist_add(Rule::rule*, int);
config* Configlist_addbasis(Rule::rule*, int);
void Configlist_closure(lemon&);
void Configlist_sort(void);
void Configlist_sortbasis(void);
config* Configlist_return(void);
config* Configlist_basis(void);
void Configlist_eat(config*);
void Configlist_reset(void);
}
using namespace Configlist;

namespace Plink
{
struct plink;

plink* Plink_new();
void Plink_add(plink**, config*);
void Plink_copy(plink**, plink*);
void Plink_delete(plink*);
}
using namespace Plink;

namespace Symbol
{
/* Symbols (terminals and nonterminals) of the grammar are stored
** in the following: */
enum class symbol_type {
    TERMINAL,
    NONTERMINAL,
    MULTITERMINAL
};
enum class e_assoc {
    LEFT,
    RIGHT,
    NONE,
    UNK
};
struct symbol {
    const char* name;        /* Name of the symbol */
    int index;               /* Index number for this symbol */
    symbol_type type;        /* Symbols are all either TERMINALS or NTs */
    rule* rule;              /* Linked list of rules of this (if an NT) */
    symbol* fallback;        /* fallback token in case this token doesn't parse */
    int prec;                /* Precedence if defined (-1 otherwise) */
    e_assoc assoc;           /* Associativity if precedence is defined */
    char* firstset;          /* First-set for all rules of this symbol */
    Boolean lambda;          /* True if NT and can generate an empty string */
    int useCnt;              /* Number of times used */
    char* destructor;        /* Code which executes whenever this symbol is
                             ** popped from the stack during error processing */
    int destLineno;          /* Line number for start of destructor.  Set to
                             ** -1 for duplicate destructors. */
    char* datatype;          /* The data type of information held by this
                             ** object. Only used if type==NONTERMINAL */
    int dtnum;               /* The data type number.  In the parser, the value
                             ** stack is a union.  The .yy%d element of this
                             ** union is the correct data type for this object */
    int bContent;            /* True if this symbol ever carries content - if
                             ** it is ever more than just syntax */
                             /* The following fields are used by MULTITERMINALs only */
    int nsubsym;             /* Number of constituent symbols in the MULTI */
    symbol** subsym;         /* Array of constituent symbols */
};
}
using namespace Symbol;

namespace Rule
{
/* Each production rule in the grammar is stored in the following
** structure.  */
struct rule {
    symbol* lhs;             /* Left-hand side of the rule */
    const char* lhsalias;    /* Alias for the LHS (NULL if none) */
    int lhsStart;            /* True if left-hand side is the start symbol */
    int ruleline;            /* Line number for the rule */
    int nrhs;                /* Number of RHS symbols */
    symbol** rhs;            /* The RHS symbols */
    const char** rhsalias;   /* An alias for each RHS symbol (NULL if none) */
    int line;                /* Line number at which code begins */
    const char* code;        /* The code executed when this rule is reduced */
    const char* codePrefix;  /* Setup code before code[] above */
    const char* codeSuffix;  /* Breakdown code after code[] above */
    symbol* precsym;         /* Precedence symbol for this rule */
    int index;               /* An index number for this rule */
    int iRule;               /* Rule number as used in the generated tables */
    Boolean noCode;          /* True if this rule has no associated C code */
    Boolean codeEmitted;     /* True if the code has been emitted already */
    Boolean canReduce;       /* True if this rule is ever reduced */
    Boolean doesReduce;      /* Reduce actions occur after optimization */
    Boolean neverReduce;     /* Reduce is theoretically possible, but prevented
                             ** by actions or other outside implementation */
    rule* nextlhs;           /* Next rule with the same LHS */
    rule* next;              /* Next rule in the global list */
};
}
using namespace Rule;

namespace State
{
struct state;
}
using namespace State;

namespace Config
{
/* A configuration is a production rule of the grammar together with
** a mark (dot) showing how much of that rule has been processed so far.
** Configurations also contain a follow-set which is a list of terminal
** symbols which are allowed to immediately follow the end of the rule.
** Every configuration is recorded as an instance of the following: */
enum class cfgstatus {
    COMPLETE,
    INCOMPLETE
};
struct config {
    rule* rp;         /* The rule upon which the configuration is based */
    int dot;                 /* The parse point */
    char* fws;               /* Follow-set for this configuration only */
    plink* fplp;      /* Follow-set forward propagation links */
    plink* bplp;      /* Follow-set backwards propagation links */
    state* stp;       /* Pointer to state which contains this */
    cfgstatus status;   /* used during followset and shift computations */
    config* next;     /* Next configuration in the state */
    config* bp;       /* The next basis configuration */
};

}
using namespace Config;

namespace Action
{
enum class e_action {
    SHIFT,
    ACCEPT,
    REDUCE,
    ERROR,
    SSCONFLICT,              /* A shift/shift conflict */
    SRCONFLICT,              /* Was a reduce, but part of a conflict */
    RRCONFLICT,              /* Was a reduce, but part of a conflict */
    SH_RESOLVED,             /* Was a shift.  Precedence resolved conflict */
    RD_RESOLVED,             /* Was reduce.  Precedence resolved conflict */
    NOT_USED,                /* Deleted by compression */
    SHIFTREDUCE              /* Shift first, then reduce */
};

/* Every shift or reduce operation is stored as one of the following */
struct action {
    symbol* sp;       /* The look-ahead symbol */
    e_action type;
    union {
        state* stp;     /* The new state, if a shift */
        rule* rp;       /* The rule, if a reduce */
    } x;
    symbol* spOpt;    /* SHIFTREDUCE optimization to this symbol */
    action* next;     /* Next action for this state */
    action* collide;  /* Next action with the same hash */
};

}
using namespace Action;

namespace State
{
/* Each state of the generated parser's finite state machine
** is encoded as an instance of the following structure. */
struct state {
    config* bp;       /* The basis configurations for this state */
    config* cfp;      /* All configurations in this set */
    int statenum;            /* Sequential number for this state */
    action* ap;       /* List of actions for this state */
    int nTknAct, nNtAct;     /* Number of actions on terminals and nonterminals */
    int iTknOfst, iNtOfst;   /* yy_action[] offset for terminals and nonterms */
    int iDfltReduce;         /* Default action is to REDUCE by this rule */
    rule* pDfltReduce;/* The default REDUCE rule. */
    int autoReduce;          /* True if this is an auto-reduce state */
};

state* State_new();
void State_init();
int State_insert(state*, config*);
state* State_find(config*);
state** State_arrayof();
}
using namespace State;

namespace Plink
{
/* A followset propagation link indicates that the contents of one
** configuration followset should be propagated to another whenever
** the first changes. */
struct plink {
    config* cfp;      /* The configuration to which linked */
    plink* next;      /* The next propagate link */
};
}
using namespace Plink;

#include <vector>

/* The state vector for the entire parser generator is recorded as
** follows.  (LEMON uses no global variables and makes little use of
** static variables.  Fields in the following structure can be thought
** of as begin global variables in the program.) */
struct lemon {
    state** sorted                = nullptr;      /* Table of states sorted by state number */
    Rule::rule* rule              = nullptr;      /* List of all rules */
    Rule::rule* startRule         = nullptr;      /* First rule */
    int nstate                    = 0;            /* Number of states */
    int nxstate                   = 0;            /* nstate with tail degenerate states removed */
    int nrule                     = 0;            /* Number of rules */
    int nruleWithAction           = 0;            /* Number of rules with actions */
    int nsymbol                   = 0;            /* Number of terminal and nonterminal symbols */
    int nterminal                 = 0;            /* Number of terminal symbols */
    int minShiftReduce            = 0;            /* Minimum shift-reduce action value */
    int errAction                 = 0;            /* Error action value */
    int accAction                 = 0;            /* Accept action value */
    int noAction                  = 0;            /* No-op action value */
    int minReduce                 = 0;            /* Minimum reduce action */
    int maxAction                 = 0;            /* Maximum action value of any kind */
    std::vector<symbol*> symbols;                 /* Sorted array of pointers to symbols */
    int errorcnt                  = 0;            /* Number of errors */
    symbol* errsym                = 0;            /* The error symbol */
    symbol* wildcard              = nullptr;      /* Token that matches anything */
    char* name                    = nullptr;      /* Name of the generated parser */
    char* arg                     = nullptr;      /* Declaration of the 3rd argument to parser */
    char* ctx                     = nullptr;      /* Declaration of 2nd argument to constructor */
    char* tokentype               = nullptr;      /* Type of terminal symbols in the parser stack */
    char* vartype                 = nullptr;      /* The default type of non-terminal symbols */
    char* start                   = nullptr;      /* Name of the start symbol for the grammar */
    char* stacksize               = nullptr;      /* Size of the parser stack */
    char* include                 = nullptr;      /* Code to put at the start of the C file */
    char* error                   = nullptr;      /* Code to execute when an error is seen */
    char* overflow                = nullptr;      /* Code to execute on a stack overflow */
    char* failure                 = nullptr;      /* Code to execute on parser failure */
    char* accept                  = nullptr;      /* Code to execute when the parser excepts */
    char* extracode               = nullptr;      /* Code appended to the generated file */
    char* tokendest               = nullptr;      /* Code to execute to destroy token data */
    char* vardest                 = nullptr;      /* Code for the default non-terminal destructor */
    char* filename                = nullptr;      /* Name of the input file */
    char* outname                 = nullptr;      /* Name of the current output file */
    char* tokenprefix             = nullptr;      /* A prefix added to token names in the .h file */
    int nconflict                 = 0;            /* Number of parsing conflicts */
    int nactiontab                = 0;            /* Number of entries in the yy_action[] table */
    int nlookaheadtab             = 0;            /* Number of entries in yy_lookahead[] */
    int tablesize                 = 0;            /* Total table size of all tables in bytes */
    int basisflag                 = 0;            /* Print only basis configurations */
    int printPreprocessed         = 0;            /* Show preprocessor output on stdout */
    int has_fallback              = 0;            /* True if any %fallback is seen in the grammar */
    int nolinenosflag             = 0;            /* True if #line statements should not be printed */
    char* argv0                   = nullptr;      /* Name of the program */
};

#include <string_view>

namespace Symbol
{
/* Routines for handling symbols of the grammar */

symbol* Symbol_new(const char*);
bool Symbolcmpp(const symbol*, const symbol*);
void Symbol_init(void);
symbol* Symbol_find(std::string_view);
int Symbol_count(void);
std::vector<symbol*> Symbol_arrayof();
}
using namespace Symbol;

/* Routines to manage the state table */
namespace Config
{
int Configcmp(const char*, const char*);
}
using namespace Config;

namespace Configtable
{
/* Routines used for efficiency in Configlist_add */
void Configtable_init(void);
int Configtable_insert(config*);
config* Configtable_find(const config*);
void Configtable_clear(int(*)(config*));
}
using namespace Configtable;