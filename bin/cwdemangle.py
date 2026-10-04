#!/usr/bin/env python3
import re
import sys

BASIC_TYPES = {
    "v": "void", "b": "bool", "c": "char", "s": "short", "i": "int", "l": "long", "x": "long long",
    "f": "float", "d": "double", "r": "long double", "w": "wchar_t", "e": "...",
}
OPERATORS = {
    "__nw": "operator new", "__nwa": "operator new[]", "__dl": "operator delete", "__dla": "operator delete[]",
    "__pl": "operator+", "__mi": "operator-", "__ml": "operator*", "__dv": "operator/", "__md": "operator%",
    "__er": "operator^", "__ad": "operator&", "__or": "operator|", "__co": "operator~", "__nt": "operator!",
    "__as": "operator=", "__lt": "operator<", "__gt": "operator>", "__apl": "operator+=", "__ami": "operator-=",
    "__amu": "operator*=", "__adv": "operator/=", "__amd": "operator%=", "__aer": "operator^=",
    "__aad": "operator&=", "__aor": "operator|=", "__ls": "operator<<", "__rs": "operator>>",
    "__ars": "operator>>=", "__als": "operator<<=", "__eq": "operator==", "__ne": "operator!=",
    "__le": "operator<=", "__ge": "operator>=", "__aa": "operator&&", "__oo": "operator||",
    "__pp": "operator++", "__mm": "operator--", "__cm": "operator,", "__rm": "operator->*",
    "__rf": "operator->", "__cl": "operator()", "__vc": "operator[]",
}


class DemangleError(Exception):
    pass


class Function:
    def __init__(self, args, ret):
        self.args = args
        self.ret = ret


class Array:
    def __init__(self, size, element):
        self.size = size
        self.element = element


class Pointer:
    def __init__(self, marks, target):
        self.marks = marks
        self.target = target


class Parser:
    def __init__(self, text):
        self.text = text
        self.pos = 0

    def peek(self):
        return self.text[self.pos] if self.pos < len(self.text) else ""

    def take(self):
        c = self.peek()
        if not c:
            raise DemangleError()
        self.pos += 1
        return c

    def done(self):
        return self.pos >= len(self.text)

    def number(self):
        start = self.pos
        while self.peek().isdigit():
            self.pos += 1
        if start == self.pos:
            raise DemangleError()
        return int(self.text[start:self.pos])

    def name(self):
        length = self.number()
        if self.pos + length > len(self.text):
            raise DemangleError()
        name = self.text[self.pos:self.pos + length]
        self.pos += length
        return template_name(name)

    def qualified(self):
        if self.peek() == "Q":
            self.take()
            count = int(self.take())
            return [self.name() for _ in range(count)]
        return [self.name()]

    def args(self):
        args = []
        while not self.done() and self.peek() != "_":
            args.append(self.type())
        return args

    def type(self):
        c = self.peek()
        if c in ("C", "V"):
            self.take()
            inner = self.type()
            word = "const" if c == "C" else "volatile"
            if isinstance(inner, Function):
                return inner
            if isinstance(inner, Array):
                return Array(inner.size, f"{word} {format_type(inner.element)}")
            if isinstance(inner, Pointer):
                return Pointer(f"{inner.marks} {word}", inner.target)
            return f"{inner} {word}" if inner.endswith(("*", "&")) else f"{word} {inner}"
        if c in ("U", "S"):
            self.take()
            return ("unsigned " if c == "U" else "signed ") + self.type()
        if c in ("P", "R"):
            self.take()
            inner = self.type()
            mark = "*" if c == "P" else "&"
            if isinstance(inner, (Function, Array)):
                return Pointer(mark, inner)
            if isinstance(inner, Pointer):
                return Pointer(inner.marks + mark, inner.target)
            return inner + mark
        if c == "F":
            self.take()
            args = self.args()
            ret = "void"
            if self.peek() == "_":
                self.take()
                ret = self.type()
            return Function(args, ret)
        if c == "A":
            self.take()
            size = self.number()
            if self.take() != "_":
                raise DemangleError()
            return Array(size, self.type())
        if c == "M":
            self.take()
            owner = "::".join(self.qualified())
            inner = self.type()
            if isinstance(inner, Function):
                hidden = inner.args[:2]
                args = inner.args[2:] if hidden in (["const void*", "void*"], ["const void*", "const void*"]) else inner.args
                const = " const" if hidden == ["const void*", "const void*"] else ""
                return f"{format_type(inner.ret)} ({owner}::*)({format_args(args, '')}){const}"
            return f"{inner} {owner}::*"
        if c.isdigit() or c == "Q":
            return "::".join(self.qualified())
        if c in BASIC_TYPES:
            self.take()
            return BASIC_TYPES[c]
        raise DemangleError()


def array_parts(value):
    dims = ""
    while isinstance(value, Array):
        dims += f"[{value.size}]"
        value = value.element
    return format_type(value), dims


def format_type(value):
    if isinstance(value, Pointer) and isinstance(value.target, Function):
        return f"{format_type(value.target.ret)} ({value.marks})({format_args(value.target.args)})"
    if isinstance(value, Pointer):
        element, dims = array_parts(value.target)
        return f"{element}({value.marks}){dims}"
    if isinstance(value, Array):
        return "".join(array_parts(value))
    if isinstance(value, Function):
        return f"{format_type(value.ret)} ()({format_args(value.args)})"
    return value


def format_args(args, empty="void"):
    return ", ".join(format_type(a) for a in args) if args else empty


def split_template_args(text):
    args, depth, start = [], 0, 0
    for i, c in enumerate(text):
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "," and depth == 0:
            args.append(text[start:i])
            start = i + 1
    args.append(text[start:])
    return args


def template_name(name):
    start = name.find("<")
    if start < 0 or not name.endswith(">"):
        return name
    args = []
    for arg in split_template_args(name[start + 1:-1]):
        if arg.lstrip("-").isdigit():
            args.append(arg)
            continue
        try:
            parser = Parser(arg)
            value = format_type(parser.type())
            args.append(value if parser.done() else arg)
        except DemangleError:
            args.append(arg)
    return f"{name[:start]}<{', '.join(args)}>"


def base_name(base, scopes):
    core, template = (base[:base.index("<")], base[base.index("<"):]) if "<" in base else (base, "")
    if core in ("__ct", "__dt") and scopes:
        name = scopes[-1].split("<")[0] + template_name("x" + template)[1:]
        return name if core == "__ct" else "~" + name
    if base.startswith("__op"):
        return "operator " + format_type(Parser(base[4:]).type())
    if base == "__vt":
        return "__vtable"
    return OPERATORS.get(base, template_name(base))


def demangle_at(symbol, split):
    base, parser = symbol[:split], Parser(symbol[split + 2:])
    local = None
    if "$localstatic" in base:
        variable, tag, base = base.split("$", 2)
        local = f"{tag} guard" if variable == "init" else variable
    scopes = []
    if parser.peek().isdigit() or parser.peek() == "Q":
        scopes = parser.qualified()
    const = False
    if parser.peek() == "C":
        parser.take()
        const = True
    name = "::".join(scopes + [base_name(base, scopes)])
    if parser.done():
        if const:
            raise DemangleError()
        return name
    if parser.take() != "F":
        raise DemangleError()
    args = parser.args()
    ret = ""
    if parser.peek() == "_":
        parser.take()
        ret = format_type(parser.type()) + " "
    if not parser.done():
        raise DemangleError()
    result = f"{ret}{name}({format_args(args)}){' const' if const else ''}"
    return f"{result}::{local}" if local else result


def demangle(symbol):
    thunk = re.match(r"^@(\d+)@(.+)$", symbol)
    if thunk:
        return f"{demangle(thunk[2])} [thunk +{thunk[1]}]"
    split = symbol.find("__", 1)
    while split > 0:
        try:
            return demangle_at(symbol, split)
        except Exception:
            split = symbol.find("__", split + 1)
    return symbol


if __name__ == "__main__":
    for arg in sys.argv[1:]:
        print(demangle(arg))
