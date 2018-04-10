import sys, getopt, re
import xml.etree.ElementTree as ET 

inst_dict = {
    ('createframe', 'pushframe', 'return', 'break', 'popframe'): [0],
    ('call', 'label', 'jump'): [1, 'label'],
    ('pushs', 'dprint', 'write'): [1, 'symb'],
    ('pops', 'defvar'): [1, 'var'],
    ('add', 'sub', 'mul', 'idiv', 'lt', 'gt', 'eq', 'and', 'or', 'str2int', 'concat', 'getchar', 'setchar'): [3, 'var', 'symb', 'symb'],
    ('not', 'move', 'int2char', 'strlen', 'type'): [2, 'var', 'symb'],
    ('read'): [2, 'var', 'type'],
    ('jumpifeq', 'jumpifneq'): [3, 'label', 'symb', 'symb'],
}

def print_help():
    print('python3.6 interpret.py --source <xml_file>')

def print_err(err_code):
    if err_code == 31:
        print('Error of lexical or syntactic analysis of text elements and attributes in input XMLFile', file=sys.stderr)
        sys.exit(err_code)

def check_attrib(attrib, key):
    try:
        _ = attrib[key]
    except KeyError:
        print_err(31)
    
def check_instr(instruction, opcode):
    print(opcode, instruction.tag, instruction.attrib, instruction.text)

    if instruction.tag[:len(instruction.tag)-1].lower() != 'arg':
        print_err(31)
    if not(0 < int(instruction.tag[-1]) <= len(inst_dict[opcode])):
        print_err(31)
    arg_num = int(instruction.tag[-1])
    check_attrib(instruction.attrib, 'type')
    if inst_dict[opcode][arg_num] == 'symb' and (instruction.attrib['type'] in ('bool', 'int', 'string') or instruction.attrib['type'] in ('var')):
        if instruction.attrib['type'] in ('int') and re.match('^(-|\+)?[0-9]+$', instruction.text) is None:
            print_err(31)
        elif instruction.attrib['type'] in ('bool') and re.match('^(false|true)$', instruction.text) is None:
            print_err(31)
        elif instruction.attrib['type'] in ('string') and re.match('^(([^\x00-\x1F\x7F\xA0])+|(\x5C[0-9]{3})+|($))$', instruction.text) is None:
            print_err(31)
        elif instruction.attrib['type'] in ('var') and re.match('^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(31)
    elif inst_dict[opcode][arg_num] == 'var' and instruction.attrib['type'] in ('var'):
        if re.match('^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(31)
    elif inst_dict[opcode][arg_num] == 'label' and instruction.attrib['type'] in ('label'):
        if re.match('^([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(31)
    elif inst_dict[opcode][arg_num] == 'type' and instruction.attrib['type'] in ('type'):
        if re.match('^\s*(int|bool|string)(\s*#.*|$)$', instruction.text) is None:
            print_err(31)    
    

def parse_xml_file(xml_namefile):
    try:
        xml_tree = ET.parse(xml_namefile)
    except ET.ParseError:
        print_err(31)
    xml_root = xml_tree.getroot()
    
    if xml_root.tag.lower() != 'program':
        print_err(31)
    check_attrib(xml_root.attrib, 'language')
    if xml_root.attrib['language'].lower() != 'ippcode18':
        print_err(31)

    for elem in xml_root:
        if elem.tag.lower() != 'instruction': 
            print_err(31)
        check_attrib(elem.attrib, 'opcode')
        check_attrib(elem.attrib, 'order')

        for key in inst_dict.keys():
            if elem.attrib['opcode'].lower() in key and len(elem) == inst_dict[key][0]:
                for subelem in elem:
                    check_instr(subelem, key)

def main(argv):
    try:
        opts, _ = getopt.getopt(
            argv, "hs:", ['help', 'source='])
    except getopt.GetoptError as err:
        print(str(err))
        print_help()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ('-h','--help'):
            print_help()
            sys.exit()
        elif opt in ("-s", "--source"):
            xml_namefile = arg
            parse_xml_file(xml_namefile)
    
if __name__ == "__main__":
    main(sys.argv[1:])
