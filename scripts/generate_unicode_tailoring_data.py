#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Point this at the icu4c/source/data/coll directory, and it will create
# several headers containing the collation tailorings found there.

# To get this dumb script to work, I have to spell out some imports in the ICU
# files:

# fi.txt: [import fi-u-co-trad] -> [import fi-u-co-traditional]
# de.txt: [import de-u-co-phonebk] - > [import de-u-co-phonebook]


import argparse
import os
import re


parser = argparse.ArgumentParser(description='Generates headers with tailoring scripts for use with Boost.Text\'s Unicode collation.')
parser.add_argument('icu_dir', type=str, help='the path to icu4c/source/data/coll containing ICU\'s tailoring data')
parser.add_argument('--tests', action='store_true', help='generate lexer and parser tests instead of the data themselves')
args = parser.parse_args()

all_tailorings = {}
tailorings_by_file = {}

for filename in os.listdir(args.icu_dir):
    lines = open(os.path.join(args.icu_dir, filename)).readlines()
    root,ext = os.path.splitext(filename)
    if ext != '.txt':
        continue
    if root == 'root':
        root = 'und'
    name_prefix = root + '-u-co-'
    name = ''
    line_idx = 0
    max_line_idx = len(lines)
    while line_idx < max_line_idx:
        line = lines[line_idx]
        if 'Sequence{' in line:
            seq_end_line_idx = line_idx
            while not line.strip().endswith('}'):
                seq_end_line_idx += 1
                line = lines[seq_end_line_idx]
            sequence = ''.join(lines[line_idx:seq_end_line_idx + 1]).strip()
            if not sequence.startswith('Sequence{'):
                raise Exception('Oops!')
            sequence = sequence[len('Sequence{'):-1]
            sequence = map(lambda x: x.strip()[1:-1], sequence.splitlines())
            sequence = map(lambda x: (x.startswith('&') or x.startswith('[')) and x or '  ' + x, sequence)
            sequence = '\n'.join(sequence)
            if name == '':
                raise Exception('Somehow we got unnamed tailoring rules!')
            all_tailorings[name_prefix + name] = sequence
            if sequence.strip() != '':
                if root not in tailorings_by_file:
                    tailorings_by_file[root] = {}
                tailorings_by_file[root][name] = sequence
            name = ''
            line_idx = seq_end_line_idx + 1
            continue
        if not 'collations' in line and '{' in line and '"' not in line and '}' not in line:
            name = line.strip()[:-1]
            line_idx += 1
            continue
        line_idx += 1

lexer_tests_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/data/all.hpp>
#include <boost/text/detail/lexer.hpp>

#include <gtest/gtest.h>


TEST(lexer, data)
{{

{0}
}}
'''

parser_tests_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/data/all.hpp>
#include <boost/text/detail/parser.hpp>

#include <gtest/gtest.h>


TEST(parser, data)
{{

    boost::text::detail::collation_tailoring_interface callbacks = {{
        [](boost::text::detail::cp_seq_t const & reset_, bool before_) {{}},
        [](boost::text::detail::relation_t const & rel) {{}},
        [](boost::text::collation_strength strength) {{}},
        [](boost::text::variable_weighting weighting) {{}},
        [](boost::text::l2_weight_order order) {{}},
        [](boost::text::detail::cp_seq_t const & suppressions) {{}},
        [](std::vector<boost::text::detail::reorder_group> const & reorder_groups) {{}},
        [](boost::text::string const & s) {{
            std::cout << s << std::endl;
            throw std::runtime_error("Parser produced an error!");
        }},
        [](boost::text::string const & s) {{ /*std::cout << s << std::endl;*/ }}
    }};


{0}
}}
'''

any_option_regex = re.compile(r'\[[^ \]]+\] *')
reorder_regex = re.compile(r'\[ *reorder([^\]]+) *\]')
suppress_regex = re.compile(r'\[ *suppressContractions(?: +([^ \]]+))+ *\]')

group_cps = {
    'Mlym': [0x0D05, 0x0D06, 0x0D3B, 0x0D3C],
    'Armn': [0x0561, 0x0531, 0x0556, 0x0559],
    'Hani': [0x2F00, 0x3280, 0x2F88F, 0x2FA1D],
    'Laoo': [0x0EDE, 0x0E81, 0x0EC3, 0x0EC4],
    'Cher': [0xAB70, 0x13A0, 0x13FD, 0x13F5],
    'Ethi': [0x1200, 0x1201, 0x2DDD, 0x2DDE], # .., .., .., [66 9B 7D, 05, 05]
    'Mong': [0x1880, 0x1881, 0x18AA, 0x18A9],
    'Hebr': [0x05D0, 0x2135, 0xFB28, 0xFB4A],
    'Bopo': [0x3105, 0x31A0, 0x3129, 0x312D],
    'Geor': [0x10D0, 0x2D00, 0x10FE, 0x10FF],
    'Deva': [0x0950, 0xA8FD, 0x094C, 0x094D], # [67 06, 05, 05]
    'Cyrl': [0x0430, 0x2DF6, 0x04CF, 0x04C0], # [60 06, 05, 05], ..
    'Arab': [0x0621, 0x077A, 0x077B],
    'Mymr': [0x1000, 0x1075, 0xAA75, 0xAA76],
    'Khmr': [0x1780, 0x1781, 0x17C5, 0x17D2],
    'Sinh': [0x0D85, 0x0D86, 0x0DDE, 0x0DCA],
    'Thai': [0x0E01, 0x0E02, 0x0E43, 0x0E44],
    'Grek': [0x03B1, 0x1D6C2, 0x03F8, 0x03F7], # .., .., [5F 5E, 05, 05] [5F 5E, 05, A0]
    'Beng': [0x0980, 0x0985, 0x09D7, 0x09FC],
    'Guru': [0x0A74, 0x0A73, 0x0A4B, 0x0A4C],
    'Gujr': [0x0AD0, 0x0A85, 0x0ACC, 0x0ACD],
    'Orya': [0x0B05, 0x0B06, 0x0B56, 0x0B57],
    'Taml': [0x0BD0, 0x0B85, 0x0BCD, 0x0BD7],
    'Telu': [0x0C05, 0x0C06, 0x0C55, 0x0C56],
    'Knda': [0x0C85, 0x0C86, 0x0CD5, 0x0CD6],
    'Latn': [0x0061, 0xFF41, 0x02AC, 0x02AD],
    'Kana': [0x3041, 0x3042, 0x1B11D, 0x1B11E],
    'Hang': [0x1100, 0x3131, 0xD7FA, 0xD7FB],
    'implicit': [0x2a700, 0x2b740, 0x2b820, 0x2ebe0]
}

groups_in_ce_order = [
    'Latn',
    'Grek',
    'Cyrl',
    'Geor',
    'Armn',
    'Hebr',
    'Arab',
    'Ethi',
    'Deva',
    'Beng',
    'Guru',
    'Gujr',
    'Orya',
    'Taml',
    'Telu',
    'Knda',
    'Mlym',
    'Sinh',
    'Thai',
    'Laoo',
    'Mymr',
    'Khmr',
    'Mong',
    'Cher',
    'Hang',
    'Kana',
    'Bopo',
    'Hani'
]

if args.tests:
    lex_tests = ''
    parse_tests = ''
    for k0,v0 in sorted(tailorings_by_file.items()):
        file_comment = '    // From {}.txt:\n\n'.format(k0)
        lex_tests += file_comment
        parse_tests += file_comment
        for k,v in sorted(v0.items()):
            if k.startswith('private'):
                continue
            strname = 'boost::text::data::{}::{}_collation_tailoring()'.format(
                k0 == 'or' and 'or_' or k0, k
            )
            lex_tests += '''\
    EXPECT_NO_THROW(
        boost::text::detail::lex(
            {0}.begin(),
            {0}.end(),
            [](boost::text::string const & s) {{
                std::cout << s << std::endl;
                throw std::runtime_error("Lexer produced an error!");
            }},
            "{1}.hpp")
    );
'''.format(strname, k0)
            parse_tests += '''\
    EXPECT_NO_THROW(
        boost::text::detail::parse(
            {0}.begin(),
            {0}.end(),
            callbacks,
            "{1}.hpp")
    );
'''.format(strname, k0)
        lex_tests += '\n\n'
        parse_tests += '\n\n'

    f = open('lexer_data.cpp', 'w')
    f.write(lexer_tests_form.format(lex_tests))
    f = open('parser_data.cpp', 'w')
    f.write(parser_tests_form.format(parse_tests))

import_regex = re.compile(r'\[ *import +([^ \]]+) *\] *')

single_tailoring_form = '''\
inline string_view {0}_collation_tailoring()
{{
    return string_view(
{1});
}}

'''

header_form = '''\
// Warning! This file is autogenerated.
#ifndef BOOST_TEXT_DATA_{0}_HPP
#define BOOST_TEXT_DATA_{0}_HPP

#include <boost/text/string_view.hpp>


namespace boost {{ namespace text {{ namespace data {{ namespace {1} {{

{2}
}}}}}}}}

#endif
'''

tailored_reordering_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/collation_tailoring.hpp>
#include <boost/text/collate.hpp>
#include <boost/text/data/all.hpp>

#include <gtest/gtest.h>


using namespace boost;

auto const error = [](text::string const & s) {{ std::cout << s; }};
auto const warning = [](text::string const & s) {{}};

{0}
'''

tailored_reordering_test_form = '''\
TEST(tailoring, {0}_{1}_reorders)
{{
    constexpr std::array<uint32_t, {3}> cps = {{{{
        {2}
    }}}};
    text::tailored_collation_element_table const table =
        text::make_tailored_collation_element_table(
            text::data::{0}::{1}_collation_tailoring(),
            "{0}::{1}_collation_tailoring()",
            error, warning);

    for (int i = 0, end = (int)cps.size() - 1; i != end; ++i) {{
        EXPECT_LE(
            text::collate(
                cps.begin() + i,
                cps.begin() + i + 1,
                cps.begin() + i + 1,
                cps.begin() + i + 2,
                table,
                text::collation_strength::primary,
                text::variable_weighting::non_ignorable),
            0)
            << "CASE " << i << " {0}::{1}_collation_tailoring() "
            << std::hex << "0x" << cps[i] << " 0x" << cps[i + 1] << std::dec;
    }}
}}

'''

tailoring_string_form = '''\
    g_curr_file = "{0}";
    g_curr_tailoring = "{1}";
    g_tailoring = data::{0}::{1}_collation_tailoring();
    make_test();
'''

tailoring_rules_tests_form = '''\
// Warning! This main() is autogenerated.
int main()
{{
{0}
}}
'''


reordering_tailoring_tests = ''
tailoring_string_strings = ''

# Flatten tailorings_by_file by processing the [import]s.
for k0,v0 in sorted(tailorings_by_file.items()):
    tailorings = ''
    for k,v in sorted(v0.items()):
        match = True
        while match:
            match = import_regex.search(v)
            if match:
                import_ = match.group(1)
                if not import_ in all_tailorings:
                    import_ += '-u-co-standard'
                if not import_ in all_tailorings:
                    print '{}.txt: error: Unable to satisfy import "{}".'.format(k0, match.group(1))
                    done = True
                    break
                replacement = all_tailorings[import_]
                v = v.replace(v[match.start(0):match.end(0)], replacement)
        if not k.startswith('private'):
            lines = v.replace('\\', '\\\\').splitlines()
            lines = map(lambda x: '"' + x + '\\n"', lines)
            chunked_lines = []
            for i in range(len(lines)):
                chunk_size = 4096
                if len(lines[i]) < chunk_size:
                    chunked_lines.append(lines[i])
                else:
                    chunks = len(lines[i]) / chunk_size
                    remainder = len(lines[i]) % chunk_size
                    prev_offset = 0
                    offset = 0
                    for j in range(chunks):
                        line_ok = False
                        while not line_ok:
                            try:
                                line = lines[i][j * chunk_size + prev_offset:(j + 1) * chunk_size + offset]
                                line.decode('UTF-8', 'strict')
                                line_ok = True
                            except UnicodeDecodeError:
                                offset -= 1
                        prev_offset = offset
                        if not line.startswith('"'):
                            line = '"' + line
                        if not line.endswith('"'):
                            line += '"'
                        chunked_lines.append(line)
                    if remainder != 0 or offset != 0:
                        chunked_lines.append('"' + lines[i][chunks * chunk_size + offset:])
            lines = '\n'.join(chunked_lines)
            tailorings += single_tailoring_form.format(k.replace('-', '_'), lines)

        groups = []
        for match in reorder_regex.finditer(v):
            for g in match.group(1).strip().split(' '):
                groups.append(g)
        if 'private' not in k and 'zh' not in k0 and len(groups) and 'others' not in groups:
            groups += filter(lambda x: x not in groups, groups_in_ce_order)
            group_ordering_cps = 0
            cps_string = ''
            for group in groups:
                group_ordering_cps += len(group_cps[group])
                cps_string += '/* {} */ '.format(group) + ', '.join(map(lambda x: hex(x), group_cps[group])) + ', '
                if group == 'Hani':
                    group_ordering_cps += len(group_cps['implicit'])
                    cps_string += '/* implicit */ ' + ', '.join(map(lambda x: hex(x), group_cps['implicit'])) + ', '
            reordering_tailoring_tests += tailored_reordering_test_form.format(
                k0 == 'or' and 'or_' or k0, k, cps_string, group_ordering_cps
            )

        # TODO
        suppressions = []
        for match in suppress_regex.finditer(v):
            suppressions.append(match.group(1))

        if 'private' not in k:
            tailoring_string_strings += tailoring_string_form.format(
                k0 == 'or' and 'or_' or k0, k
            )

    if args.tests:
        f = open('tailoring_reorderings.cpp', 'w')
        f.write(tailored_reordering_form.format(reordering_tailoring_tests))
        f = open('tailoring_rules_tests_main.cpp', 'w')
        f.write(tailoring_rules_tests_form.format(tailoring_string_strings))
    else:
        f = open(k0 + '.hpp', 'w')
        f.write(header_form.format(k0.upper(), k0 == 'or' and 'or_' or k0, tailorings))
        f.close()