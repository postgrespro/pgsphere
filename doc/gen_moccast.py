# A script to create the automatic casts for overlaps and intersects
# between MOCs and spolys/scircles.
#
# This has originally been used to create pg_sphere--1.2.0--1.2.1.sql.
# Before 1.2.1 is release, this can be fixed to improve that SQL.
# After the 1.2.1 release, this is just documentation on how MOC
# casts were generated that is perhaps just a bit more readable than
# that bunch of SQL.

import datetime
import re
import sys


OVERLAP_DEFS = [
    # func_stem, operator, commutator
    ('subset', '<@', '@>'),
    ('not_subset', '!<@', '!@>'),
    ('superset', '@>', '<@'),
    ('not_superset', '!@>', '!<@'),
]

INTERSECT_DEFS = [
    # func_stem, operator, commutator
    ('intersect', '&&', '&&'),
    ('not_intersect', '!&&', '!&&'),
]


GEO_TYPES = ["scircle", "spoly"]

OP_DEFS = OVERLAP_DEFS


class Accum:
    """an accumulator for our output.
    """
    def __init__(self):
        self.parts = []

    @property
    def content(self):
        return "".join(self.parts)

    def write(self, s):
        self.parts.append(s)

    def writeln(self, *strings):
        self.parts.append("\n".join(strings)+"\n")

    def replace_last(self, subs):
        """replaces the last non-whitespace char with the string subs.
        """
        for index, part in enumerate(reversed(self.parts)):
            if part.strip():
                break
        else:
            # nothing to replace
            return
        
        index = -1-index
        self.parts[index] = re.sub("[^\s](\s*)$", 
            lambda mat: subs+mat.group(1),
            self.parts[index])

    def introduce_section(self, sec_name):
        self.writeln()
        self.writeln("-- #################################")
        self.writeln(f"-- {sec_name}")


def emit_drop_code(accum):
    accum.introduce_section("Cleanup")

    accum.writeln("DROP OPERATOR IF EXISTS")
    for _, op, _ in OP_DEFS:
        for geo_type in GEO_TYPES:
            accum.writeln(f"    {op} (smoc, {geo_type}),")
            accum.writeln(f"    {op} ({geo_type}, smoc),")
    accum.replace_last(";")


def make_negator(op):
    if op.startswith("!"):
        return op[1:]
    else:
        return "!"+op


def emit_op_def(accum, operator, leftarg, rightarg, procedure, commutator):
    accum.writeln(
        f"CREATE OPERATOR {operator} (",
        f"  LEFTARG    = {leftarg},",
        f"  RIGHTARG   = {rightarg},",
        f"  PROCEDURE  = {procedure},",
        f"  COMMUTATOR = '{commutator}',",
        f"  NEGATOR    = '{make_negator(operator)}',",
        f"  RESTRICT   = contsel,",
        f"  JOIN       = contjoinsel",
        f");")


def emit_op_and_func(accum, op_def):
    func_stem, operator, commutator = op_def
    for geo_type in GEO_TYPES:
        func_name = f"{geo_type}_{func_stem}_smoc"
        accum.writeln(
            f"CREATE OR REPLACE FUNCTION {func_name}(",
            f"  geo_arg {geo_type}, a_moc smoc) RETURNS BOOL AS $body$",
            f"    SELECT smoc(max_order(a_moc), geo_arg) {operator} a_moc",
            f"  $body$ LANGUAGE SQL STABLE;")
        emit_op_def(accum, operator,
            geo_type, "smoc",
            func_name,
            commutator)
 
        accum.writeln()

        func_name = f"smoc_{func_stem}_{geo_type}"
        accum.writeln(
            f"CREATE OR REPLACE FUNCTION {func_name}(",
            f"  a_moc smoc, geo_arg {geo_type}) RETURNS BOOL AS $body$",
            f"    SELECT a_moc {operator} smoc(max_order(a_moc), geo_arg)",
            f"  $body$ LANGUAGE SQL STABLE;")
        emit_op_def(accum, operator,
            "smoc", geo_type,
            func_name,
            commutator)

        accum.writeln()


def main():
    accum = Accum()

    accum.writeln("-- MOC/geometry automatic casts.")
    accum.writeln(f"-- Generated {datetime.date.today()} by {sys.argv[0]}.")
    accum.writeln(f"-- Re-generation needs to be triggered manually.")
    accum.writeln()
    emit_drop_code(accum)
  
    accum.introduce_section(" smoc/geo OVERLAPS")
    for op_def in OVERLAP_DEFS:
        emit_op_and_func(accum, op_def)
        accum.writeln()

    accum.introduce_section(" smoc/geo INTERSECTS")
    for op_def in INTERSECT_DEFS:
        emit_op_and_func(accum, op_def)
        accum.writeln()

    print(accum.content)

    
if __name__=="__main__":
    main()
