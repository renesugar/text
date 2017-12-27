#!/usr/bin/env python
# -*- coding: utf-8 -*-

from generate_unicode_normalization_data import cccs
from generate_unicode_normalization_data import expand_decomp_canonical
from generate_unicode_normalization_data import get_decompositions

weights_header_form = '''\
// Warning! This file is autogenerated.
#ifndef BOOST_TEXT_COLLATION_WEIGHTS_HPP
#define BOOST_TEXT_COLLATION_WEIGHTS_HPP


namespace boost {{ namespace text {{

/** TODO
    Min values exclude 0. */
enum class collation_weights : int {{
    min_l1 = {0:>6},        /// The minumum nonzero L1 collation weight.
    max_l1 = {1:>6},        /// The maxumum nonzero L1 collation weight.

    min_l2 = {2:>6},        /// The minumum nonzero L2 collation weight.
    max_l2 = {3:>6},        /// The maxumum nonzero L2 collation weight.

    min_l3 = {4:>6},        /// The minumum nonzero L3 collation weight.
    max_l3 = {5:>6},        /// The maxumum nonzero L3 collation weight.

    min_variable = {6:>6},  /// The minumum variable L1 collation weight.
    max_variable = {7:>6},  /// The maxumum variable L1 collation weight.
}};

}}}}

#endif
'''

collation_elements_file_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/collation_data.hpp>

#include <unordered_map>


namespace boost {{ namespace text {{ namespace detail {{

const std::array<collation_element, {1}> g_collation_elements = {{{{
{0}
}}}};

collation_element const * g_collation_elements_first =
    g_collation_elements.data();

}}}}}}
'''

multiple_cp_key_file_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/collation_data.hpp>
#include <boost/text/collation_weights.hpp>

#include <unordered_map>


namespace boost {{ namespace text {{

// For the compression schemes used elsewhere in sort key generation to work,
// these must be well-formed.  See:
// http://www.unicode.org/reports/tr10/#Eliminating_level_separators
// http://www.unicode.org/reports/tr10/#L2/L3_in_8_bits

static_assert(
    static_cast<int>(collation_weights::max_l2) < static_cast<int>(collation_weights::min_l1),
    "Oops!  The max L2 collation weight must be less than the min L1 weight.");
static_assert(
    static_cast<int>(collation_weights::max_l3) < static_cast<int>(collation_weights::min_l2),
    "Oops!  The max L3 collation weight must be less than the min L2 weight.");
static_assert(
    static_cast<int>(collation_weights::max_l2) - static_cast<int>(collation_weights::min_l2) < 256,
    "Oops!  The range of L2 collation weights must be < 256.");

namespace detail {{
    std::unordered_set<detail::collation_trie_node> const g_collation_initial_nodes = {{
{0}
    }};
}}

static const std::array<detail::collation_trie_node, {2}>
g_collation_noninitial_trie_nodes_array = {{{{
{1}
}}}};

namespace detail {{
    collation_trie_node const * g_collation_trie_nodes =
        g_collation_noninitial_trie_nodes_array.data(); 
}}

}}}}
'''


def get_ducet(filename):
    ducet = {}
    ducet_lines = {}

    min_var = 10000
    max_var = 0
    min_non_var = 10000
    max_non_var = 0
    min_l2 = 10000
    max_l2 = 0
    min_l3 = 10000
    max_l3 = 0
    lines = open(filename, 'r').readlines()
    for line in lines:
        line = line[:-1]
        if not line.startswith('#') and not line.startswith('@') and len(line) != 0:
            comment_start = line.find('#')
            comment = ''
            if comment_start != -1:
                comment = line[comment_start + 1:].strip()
                line = line[:comment_start]
            fields = map(lambda x: x.strip(), line.split(';'))
            cps = tuple(map(lambda x: int(x, 16), fields[0].split(' ')))
            collation_elements = fields[1][1:-1].split('][')
            collation_elements = map(
                lambda x: (
                    map(lambda y: int(y, 16), x[1:].split('.')),
                    x[0] == '*'
                ),
                collation_elements
            )
            for e in collation_elements:
                if e[1]:
                    min_var = min(min_var, e[0][0])
                    max_var = max(max_var, e[0][0])
                else:
                    if e[0][0] != 0:
                        min_non_var = min(min_non_var, e[0][0])
                    max_non_var = max(max_non_var, e[0][0])
                if e[0][1] != 0:
                    min_l2 = min(min_l2, e[0][1])
                max_l2 = max(max_l2, e[0][1])
                if e[0][2] != 0:
                    min_l3 = min(min_l3, e[0][2])
                max_l3 = max(max_l3, e[0][2])
            collation_elements = map(lambda x: x[0], collation_elements)
            ducet[cps] = collation_elements
            ducet_lines[cps] = (line, comment)

    # If you're seeing an error because of this return, it's because we could
    # not determine a range of variable weights.  Hopefully, this will never
    # happen.
    if min_non_var <= max_var:
        return None

    min_l1 = min(min_var, min_non_var)
    max_l1 = max(max_var, max_non_var)

    return (ducet, ducet_lines, min_var, max_var, min_l1, max_l1, min_l2, max_l2, min_l3, max_l3)

def ccc(cccs_dict, cp):
    if cp in cccs_dict:
        return cccs_dict[cp]
    return 0

# http://www.unicode.org/reports/tr10/#Implicit_Weights
def implicit_ce(cp):
    # Tangut and Tangut Components
    if 0x17000 <= cp and cp <= 0x18AFF:
        return [
            [0xFB00, 0x0020, 0x0020],
            [(cp - 0x17000) | 0x8000, 0x0000, 0x0000]
        ]

    # Nushu
    if 0x1B170 <= cp and cp <= 0x1B2FF:
        return [
            [0xFB00, 0x0020, 0x0020],
            [(cp - 0x1B170) | 0x8000, 0x0000, 0x0000]
        ]

    BBBB = [(cp & 0x7FFF) | 0x8000, 0x0000, 0x0000]

    # Core Han Unified Ideographs
    CJK_Compatibility_Ideographs = [
        0xFA0E, 0xFA0F, 0xFA11, 0xFA13, 0xFA14, 0xFA1F, 0xFA21, 0xFA23,
        0xFA24, 0xFA27, 0xFA28, 0xFA29
    ]
    if 0x4E00 <= cp and cp <= 0x9FEA or cp in CJK_Compatibility_Ideographs:
        return [
            [0xFB40 + (cp >> 15), 0x0020, 0x0020],
            BBBB
        ]

    CJK_Unified_Ideographs_Extension_D = [
        0x2B740, 0x2B741, 0x2B742, 0x2B743, 0x2B744, 0x2B745, 0x2B746, 0x2B747,
        0x2B748, 0x2B749, 0x2B74A, 0x2B74B, 0x2B74C, 0x2B74D, 0x2B74E, 0x2B74F,
        0x2B750, 0x2B751, 0x2B752, 0x2B753, 0x2B754, 0x2B755, 0x2B756, 0x2B757,
        0x2B758, 0x2B759, 0x2B75A, 0x2B75B, 0x2B75C, 0x2B75D, 0x2B75E, 0x2B75F,
        0x2B760, 0x2B761, 0x2B762, 0x2B763, 0x2B764, 0x2B765, 0x2B766, 0x2B767,
        0x2B768, 0x2B769, 0x2B76A, 0x2B76B, 0x2B76C, 0x2B76D, 0x2B76E, 0x2B76F,
        0x2B770, 0x2B771, 0x2B772, 0x2B773, 0x2B774, 0x2B775, 0x2B776, 0x2B777,
        0x2B778, 0x2B779, 0x2B77A, 0x2B77B, 0x2B77C, 0x2B77D, 0x2B77E, 0x2B77F,
        0x2B780, 0x2B781, 0x2B782, 0x2B783, 0x2B784, 0x2B785, 0x2B786, 0x2B787,
        0x2B788, 0x2B789, 0x2B78A, 0x2B78B, 0x2B78C, 0x2B78D, 0x2B78E, 0x2B78F,
        0x2B790, 0x2B791, 0x2B792, 0x2B793, 0x2B794, 0x2B795, 0x2B796, 0x2B797,
        0x2B798, 0x2B799, 0x2B79A, 0x2B79B, 0x2B79C, 0x2B79D, 0x2B79E, 0x2B79F,
        0x2B7A0, 0x2B7A1, 0x2B7A2, 0x2B7A3, 0x2B7A4, 0x2B7A5, 0x2B7A6, 0x2B7A7,
        0x2B7A8, 0x2B7A9, 0x2B7AA, 0x2B7AB, 0x2B7AC, 0x2B7AD, 0x2B7AE, 0x2B7AF,
        0x2B7B0, 0x2B7B1, 0x2B7B2, 0x2B7B3, 0x2B7B4, 0x2B7B5, 0x2B7B6, 0x2B7B7,
        0x2B7B8, 0x2B7B9, 0x2B7BA, 0x2B7BB, 0x2B7BC, 0x2B7BD, 0x2B7BE, 0x2B7BF,
        0x2B7C0, 0x2B7C1, 0x2B7C2, 0x2B7C3, 0x2B7C4, 0x2B7C5, 0x2B7C6, 0x2B7C7,
        0x2B7C8, 0x2B7C9, 0x2B7CA, 0x2B7CB, 0x2B7CC, 0x2B7CD, 0x2B7CE, 0x2B7CF,
        0x2B7D0, 0x2B7D1, 0x2B7D2, 0x2B7D3, 0x2B7D4, 0x2B7D5, 0x2B7D6, 0x2B7D7,
        0x2B7D8, 0x2B7D9, 0x2B7DA, 0x2B7DB, 0x2B7DC, 0x2B7DD, 0x2B7DE, 0x2B7DF,
        0x2B7E0, 0x2B7E1, 0x2B7E2, 0x2B7E3, 0x2B7E4, 0x2B7E5, 0x2B7E6, 0x2B7E7,
        0x2B7E8, 0x2B7E9, 0x2B7EA, 0x2B7EB, 0x2B7EC, 0x2B7ED, 0x2B7EE, 0x2B7EF,
        0x2B7F0, 0x2B7F1, 0x2B7F2, 0x2B7F3, 0x2B7F4, 0x2B7F5, 0x2B7F6, 0x2B7F7,
        0x2B7F8, 0x2B7F9, 0x2B7FA, 0x2B7FB, 0x2B7FC, 0x2B7FD, 0x2B7FE, 0x2B7FF,
        0x2B800, 0x2B801, 0x2B802, 0x2B803, 0x2B804, 0x2B805, 0x2B806, 0x2B807,
        0x2B808, 0x2B809, 0x2B80A, 0x2B80B, 0x2B80C, 0x2B80D, 0x2B80E, 0x2B80F,
        0x2B810, 0x2B811, 0x2B812, 0x2B813, 0x2B814, 0x2B815, 0x2B816, 0x2B817,
        0x2B818, 0x2B819, 0x2B81A, 0x2B81B, 0x2B81C, 0x2B81D
    ]
    # All other Han Unified Ideographs
    if 0x3400 <= cp and cp <= 0x4DB5 or \
      0x20000 <= cp and cp <= 0x2A6D6 or \
      0x2A700 <= cp and cp <= 0x2B734 or \
      cp in CJK_Unified_Ideographs_Extension_D or \
      0x2B820 <= cp and cp <= 0x2CEA1 or \
      0x2CEB0 <= cp and cp <= 0x2EBE0:
       return [
            [0xFB80 + (cp >> 15), 0x0020, 0x0020],
            BBBB
        ]

    # Everything else (except Hangul; sigh).
    return [
        [0xFBC0 + (cp >> 15), 0x0020, 0x0020],
        BBBB
    ]

def ce(ducet, cps):
    if cps in ducet:
        return ducet[cps]
    if len(cps) != 1:
        return None
    return implicit_ce(cps[0])

# Add the ten contractions needed for well-formedness.
# See http://www.unicode.org/reports/tr10/#Well_Formed_DUCET
def add_10_contractions(ducet):
    # 0FB2 0F71 ; CE(0FB2) CE(0F71)
    ducet[(0xFB2, 0xF71)] = ce(ducet, (0x0FB2,)) + ce(ducet, (0x0F71,))
    # 0FB3 0F71 ; CE(0FB3) CE(0F71)
    ducet[(0xFB3, 0xF71)] = ce(ducet, (0x0FB3,)) + ce(ducet, (0x0F71,))

    # 0FB2 0F71 0F72 ; CE(0FB2) CE(0F71 0F72)
    ducet[(0xFB2, 0xF71, 0xF72)] = ce(ducet, (0x0FB2,)) + ce(ducet, (0x0F71, 0xF72))
    # 0FB2 0F73      ; CE(0FB2) CE(0F71 0F72)
    ducet[(0xFB2, 0xF73)] = ce(ducet, (0x0FB2,)) + ce(ducet, (0x0F71, 0xF72))
    # 0FB2 0F71 0F74 ; CE(0FB2) CE(0F71 0F74)
    ducet[(0xFB2, 0xF71, 0xF74)] = ce(ducet, (0x0FB2,)) + ce(ducet, (0x0F71, 0xF74))
    # 0xFB2 0F75     ; CE(0FB2) CE(0F71 0F74)
    ducet[(0xFB2, 0xF75)] = ce(ducet, (0x0FB2,)) + ce(ducet, (0x0F71, 0xF74))

    # 0FB3 0F71 0F72 ; CE(0FB3) CE(0F71 0F72)
    ducet[(0xFB3, 0xF71, 0xF72)] = ce(ducet, (0x0FB3,)) + ce(ducet, (0x0F71, 0xF72))
    # 0FB3 0F73      ; CE(0FB3) CE(0F71 0F72)
    ducet[(0xFB3, 0xF73)] = ce(ducet, (0x0FB3,)) + ce(ducet, (0x0F71, 0xF72))
    # 0FB3 0F71 0F74 ; CE(0FB3) CE(0F71 0F74)
    ducet[(0xFB3, 0xF71, 0xF74)] = ce(ducet, (0x0FB3,)) + ce(ducet, (0x0F71, 0xF74))
    # 0FB3 0F75      ; CE(0FB3) CE(0F71 0F74)
    ducet[(0xFB3, 0xF75)] = ce(ducet, (0x0FB3,)) + ce(ducet, (0x0F71, 0xF74))

    return ducet

def collation_elements_for_decomposition(cccs_dict, ducet, cps):
    longest_prefix = (cps[0],)
    for i in reversed(range(1, len(cps))):
        t = tuple(cps[0:i + 1])
        if t in ducet:
            longest_prefix = t
            break

    i = len(longest_prefix)
    while i < len(cps):
        blocked = True
        if i == len(longest_prefix) or ccc(cccs_dict, cps[i - 1]) < ccc(cccs_dict, cps[i]):
            blocked = False
        if not blocked:
            new_longest = longest_prefix + (cps[i],)
            if new_longest in ducet:
                longest_prefix = new_longest
                cps = cps.pop(i)
            else:
                i += 1
        else:
            i += 1

    cps = cps[len(longest_prefix):]

    return (ce(ducet, longest_prefix), cps)

# http://www.unicode.org/reports/tr10/#Avoiding_Normalization
def ucet_from_ducet_and_decompositions(cccs_dict, ducet, decomposition_mapping):
    ucet = {}

    handled = set()

    for k,v in decomposition_mapping.items():
        v_initial = v
        collation_elements = []
        while len(v):
            (ces, v) = collation_elements_for_decomposition(
                cccs_dict, ducet, v
            )
            if ces == None:
                print hex(k),map(lambda x : hex(x), v0)
            collation_elements += ces 
        ucet[(k,)] = collation_elements
        if 1 < len(v_initial):
            handled.add(tuple(v_initial))

    for k,v in ducet.items():
        if k not in handled:
            ucet[k] = v

    return ucet

import ctypes
try:
    icu_caniter = ctypes.cdll.LoadLibrary('libicu_caniter.dylib')
except OSError:
    print 'Could not load libicu_caniter.dylib.  Did you remember to build it, ' + \
          'and copy it, libicuuc.60.dylib, and libicudata.60.dylib into the ' + \
          'working directory?'
    exit(1)
icu_caniter.canonical_closure.restype = ctypes.POINTER(ctypes.c_int * 1024)

def canonical_closure(tuple_):
    array_param = (ctypes.c_int * len(tuple_))()
    for i in range(len(tuple_)):
        array_param[i] = tuple_[i]
    call_result = icu_caniter.canonical_closure(array_param, ctypes.c_int(len(array_param)))
    retval = []
    i = 0
    current = []
    for i in range(len(call_result.contents)):
        x = call_result.contents[i]
        if x == 0:
            if current == []:
                break
            if current != list(tuple_):
                retval.append(current)
            current = []
        else:
            current.append(x)
    return map(lambda x: tuple(x), retval)

#print canonical_closure((493,)) # 493 == ǭ (example from TN #5)

# http://www.unicode.org/notes/tn5/#Enumerating_Equivalent_Strings
def add_canonical_closure(fcc_ucet):
    new_ucet = {}

    for k,v in fcc_ucet.items():
        new_ucet[k] = v
        for k2 in canonical_closure(k):
            new_ucet[k2] = v

    return new_ucet

def trie_data(fcc_ucet):
    trie_initial_cps = set()
    fanouts = {}
    trie_cp_num_parents = {}
    for k,v in fcc_ucet.items():
        for i in range(0, len(k) - 1):
            if i == 0:
                trie_initial_cps.add(k[i])
            if k[i] not in fanouts:
                fanouts[k[i]] = set()
            end_of_sequence = None
            if i + 1 == len(k) - 1:
                end_of_sequence = k
            fanouts[k[i]].add((k[i + 1], end_of_sequence))
            if k[i + 1] not in trie_cp_num_parents:
                trie_cp_num_parents[k[i + 1]] = set()
            trie_cp_num_parents[k[i + 1]].add(k[i])

    trie_cp_num_parents = map(lambda x: (x[0], len(x[1])), trie_cp_num_parents.items())
    single_parent_cps = \
      sorted(map(lambda x: x[0], filter(lambda x: x[1] == 1, trie_cp_num_parents)))

    return (trie_initial_cps, fanouts)

def make_unique_collation_element_sequence(ucet):
    new_ucet = {}
    collation_elements = []

    already_processed = {}
    for k,v in ucet.items():
        tv = tuple(map(lambda x: tuple(x), v))
        if tv not in already_processed:
            first = len(collation_elements)
            collation_elements += v
            last = len(collation_elements)
            already_processed[tv] = (first, last)
        new_ucet[k] = already_processed[tv]

    return (new_ucet, collation_elements)

class trie_node:
    def __init__(self, cp):
        self.cp_ = cp
        self.children_ = []
        self.collation_elements_ = (0, 0)
    def __lt__(self, rhs):
        return self.cp_ < rhs.cp_
    def __str__(self):
        return print_node(self)
    def child(self, cp):
        for c in self.children_:
            if c.cp_ == cp:
                return c
        return None
    def insert(self, node):
        self.children_.append(node)
        return node

def trie_insert(trie, k, v):
    i = 0
    end = len(k)

    while i < end:
        c = trie.child(k[i])
        if c != None:
            trie = c
            i += 1
        else:
            break

    while i < end:
        trie = trie.insert(trie_node(k[i]))
        i += 1

    if trie.collation_elements_ != (0, 0) and trie.collation_elements_ != v:
        raise Exception('Error in trie construction -- data lost due to node collision.')

    trie.collation_elements_ = v

def make_trie(ucet):
    trie = trie_node(-1)

    for k,v in ucet.items():
        trie_insert(trie, k, v)

    return trie

def print_node(trie, indent = -1):
    if trie.cp_ == -1:
        retval = ''
    else:
        retval = '{0}cp={1} coll={2}\n'.format(
            '  ' * indent, hex(trie.cp_), trie.collation_elements_
        )
    for c in sorted(trie.children_):
        retval += print_node(c, indent + 1)
    return retval

def num_descendents(trie):
    if trie.children_ == []:
        return 0
    return len(trie.children_) + reduce(lambda x,y: x + num_descendents(y), trie.children_, 0)

def visit_trie(trie, all_nodes):
    if len(trie.children_) == 0:
        return
    sorted_children = sorted(trie.children_)
    to_append = len(sorted_children)
    N = len(all_nodes) + to_append
    for c in sorted_children:
        num_children = len(c.children_)
        num_descendents_ = num_descendents(c)
        node = trie_node(c.cp_)
        node.children_ = (N, N + num_children)
        node.collation_elements_ = c.collation_elements_
        all_nodes.append(node)
        N += num_descendents_
    for c in sorted_children:
        visit_trie(c, all_nodes)

def flatten_trie(trie):
    hash_lines = ''
    trie_lines = ''

    all_nodes = []
    visit_trie(trie, all_nodes)
    num_root_children = len(trie.children_)
    i = 0
    for n in all_nodes:
        line = '        {{ {}, {}, {}, {{ {}, {}, }} }},\n'.format(
            hex(n.cp_),
            n.children_[0] - num_root_children, n.children_[1] - num_root_children,
            n.collation_elements_[0], n.collation_elements_[1]
        )
        if i < num_root_children:
            hash_lines += line
        else:
            trie_lines += line
        i += 1

    return (hash_lines, trie_lines, len(all_nodes))

def find_singleton_keys(ucet, trie):
    non_singleton_initial_cps = set(map(lambda x: x.cp_, trie.children_))
    singleton_keys = set()
    for k,v in ucet.items():
        if len(k) == 1 and k[0] not in non_singleton_initial_cps:
            singleton_keys.add(k)
    return singleton_keys

def ce_to_cpp(ce, min_l2):
    biased_l2 = ce[1] - min_l2
    if biased_l2 < 0:
        biased_l2 += 256
    return '{{uint16_t({}), uint8_t({}), uint8_t({})}}'.format(
        hex(ce[0]), hex(biased_l2), hex(ce[2])
    )

if __name__ == "__main__":
    cccs_dict = cccs('DerivedCombiningClass.txt')
    decomposition_mapping = \
      get_decompositions('UnicodeData.txt', cccs_dict, expand_decomp_canonical)
    decomposition_mapping = filter(lambda x: x[1][1], decomposition_mapping)
    decomposition_mapping = map(lambda x: (x[0], x[1][0]), decomposition_mapping)
    decomposition_mapping = dict(decomposition_mapping)

    # TODO: Consider using allkeys_CLDR.txt.
    (ducet, ducet_lines, min_var, max_var, min_l1, max_l1, min_l2, max_l2, min_l3, max_l3) = \
      get_ducet('allkeys.txt')

    # TODO: Remove if/when allkeys_CLDR.txt is used.
    ducet = add_10_contractions(ducet)

    fcc_ucet = ucet_from_ducet_and_decompositions(cccs_dict, ducet, decomposition_mapping)
    fcc_ucet = add_canonical_closure(fcc_ucet)
    (fcc_ucet, collation_elements) = make_unique_collation_element_sequence(fcc_ucet)

    trie = make_trie(fcc_ucet)
    (hash_lines, trie_lines, num_trie_lines) = flatten_trie(trie)

    singleton_keys = find_singleton_keys(fcc_ucet, trie)

    hpp_file = open('collation_weights.hpp', 'w')
    hpp_file.write(weights_header_form.format(
        hex(min_l1), hex(max_l1), hex(min_l2), hex(max_l2),
        hex(min_l3), hex(max_l3), hex(min_var), hex(max_var)
    ))

    item_strings = map(lambda x: ce_to_cpp(x, min_l2), collation_elements)
    collation_elements_list = '    ' + ',\n    '.join(item_strings) + ',\n'
    cpp_file = open('collation_data_0.cpp', 'w')
    cpp_file.write(collation_elements_file_form.format(collation_elements_list, len(collation_elements)))

    cpp_file = open('collation_data_1.cpp', 'w')
    cpp_file.write(multiple_cp_key_file_form.format(hash_lines, trie_lines, num_trie_lines, len(trie.children_)))
