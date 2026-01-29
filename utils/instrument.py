#!/usr/bin/env python3
"""
In-place C instrumentation: inserts TRACK_ASSIGN() on the line after
simple assignments. Intended to be idempotent-ish (skips if TRACK_ASSIGN
is already present on the following line).
"""

import re
import sys
from pathlib import Path


class CInstrumenter:
    def __init__(self, source_dir):
        self.source_dir = Path(source_dir)
        self.instrumented_files = []
        
    def is_valid_identifier(self, name):
        """Check if name is a valid C identifier"""
        return re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*$', name) is not None
    
    def remove_strings_and_comments(self, line):
        """Replace strings and comments with placeholders to avoid false positives"""
        # Remove C++ style comments
        if '//' in line:
            line = line[:line.index('//')]
        
        # Mark string content (but keep the quotes)
        line = re.sub(r'"[^"]*"', '"__STR__"', line)
        line = re.sub(r"'[^']*'", "'__STR__'", line)
        
        return line
    
    def is_compound_assignment(self, text):
        """Check if this is a compound assignment (+=, -=, etc.)"""
        # Look for compound assignment operators
        compound_ops = ['+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=']
        return any(op in text for op in compound_ops)
    
    def is_comparison(self, text):
        """Check if = is part of a comparison"""
        return '==' in text or '!=' in text or '<=' in text or '>=' in text
    
    def instrument_assignment(self, line):
        """If line is a simple assignment, return an instrumentation line (without newline)."""
        original_line = line

        if line.strip().startswith('//') or line.strip().startswith('/*'):
            return None

        safe_line = self.remove_strings_and_comments(line)

        if self.is_compound_assignment(safe_line) or self.is_comparison(safe_line):
            return None

        pattern = r'^\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([^;=]+);'
        match = re.search(pattern, safe_line)
        if not match:
            return None

        var_name = match.group(1)
        rest_of_line = match.group(2)

        type_keywords = ['int', 'char', 'float', 'double', 'long', 'short',
                         'unsigned', 'signed', 'void', 'struct', 'union', 'enum']

        before_var = original_line[:match.start(1)].strip()
        if any(kw in before_var for kw in type_keywords):
            return None

        indent_match = re.match(r'^(\s*)', original_line)
        indent = indent_match.group(1) if indent_match else ''

        return f"{indent}TRACK_ASSIGN({var_name}, {rest_of_line});"
    
    def instrument_file(self, filepath):
        """Instrument a single C file in place by inserting TRACK_ASSIGN after assignments."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()

            instrumented_lines = []
            in_block_comment = False
            total_inserts = 0

            for line in lines:
                if '/*' in line:
                    in_block_comment = True
                if '*/' in line:
                    in_block_comment = False
                    instrumented_lines.append(line)
                    continue

                if in_block_comment or 'TRACK_ASSIGN' in line:
                    instrumented_lines.append(line)
                    continue

                instr_line = self.instrument_assignment(line)
                if instr_line:
                    instrumented_lines.append(instr_line + '\n')
                    total_inserts += 1
                else:
                    instrumented_lines.append(line)

            with open(filepath, 'w', encoding='utf-8') as f:
                f.writelines(instrumented_lines)

            if total_inserts > 0:
                self.instrumented_files.append(filepath)
                print(f"Instrumented {filepath} (+{total_inserts} TRACK_ASSIGN)")
            else:
                print(f"No insertions in {filepath}")

            return True
        except Exception as e:
            print(f"Error instrumenting {filepath}: {e}", file=sys.stderr)
            return False
    
    def instrument_directory(self):
        """Instrument all C files in the source directory."""
        c_files = list(self.source_dir.glob('**/*.c'))
        h_files = list(self.source_dir.glob('**/*.h'))

        print(f"Found {len(c_files)} C files and {len(h_files)} header files (headers left unchanged)")

        for c_file in c_files:
            self.instrument_file(c_file)

        return len(self.instrumented_files)


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 instrument.py <source_directory>")
        sys.exit(1)
    
    source_dir = Path(sys.argv[1])
    
    if not source_dir.is_dir():
        print(f"Error: {source_dir} is not a directory", file=sys.stderr)
        sys.exit(1)

    instrumenter = CInstrumenter(source_dir)

    inserts = instrumenter.instrument_directory()
    if inserts == 0:
        print("No new TRACK_ASSIGN insertions (already instrumented)")
    return 0


if __name__ == '__main__':
    sys.exit(main())
