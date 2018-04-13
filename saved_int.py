import sys, getopt, re, copy, operator, codecs
import xml.etree.ElementTree as ET 

inst_dict = {
    ('CREATEFRAME', 'PUSHFRAME', 'RETURN', 'BREAK', 'POPFRAME'): [0],
    ('CALL', 'LABEL', 'JUMP'): [1, 'label'],
    ('PUSHS', 'DPRINT', 'WRITE'): [1, 'symb'],
    ('POPS', 'DEFVAR'): [1, 'var'],
    ('ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND', 'OR', 'STRI2INT', 'CONCAT', 'GETCHAR', 'SETCHAR'): [3, 'var', 'symb', 'symb'],
    ('NOT', 'MOVE', 'INT2CHAR', 'STRLEN', 'TYPE'): [2, 'var', 'symb'],
    ('READ'): [2, 'var', 'type'],
    ('JUMPIFEQ', 'JUMPIFNEQ'): [3, 'label', 'symb', 'symb'],
}

frame_stack = list()
call_stack = list()
data_stack = list()
GF = dict()
labels_dict = dict()
TF = None
LF = None
DEBUG = False
insts = 0
vars = 0

def print_help():
    print('python3.6 interpret.py --source <xml_file>')

def print_err(err_code):
    if err_code == 31:
        print('Invalid XML input file format', file=sys.stderr)
    elif err_code == 32:
        print('Error of lexical or syntactic analysis of text elements and attributes in input XMLFile', file=sys.stderr)
    elif err_code == 52:
        print('Error in semantic input code at checks in IPPcode18.', file=sys.stderr)
    elif err_code == 53:
        print('Wrong type of operands.', file=sys.stderr)
    elif err_code == 54:
        print('Access to a non-existent variable', file=sys.stderr)
    elif err_code == 55:
        print("Frame does not exists.", file=sys.stderr)
    elif err_code == 56:
        print('Missing value (in variable, on stack, or in the call stack)', file=sys.stderr)
    elif err_code == 57:
        print('Zero division', file=sys.stderr)
    elif err_code == 58:
        print('Wrong work with string', file=sys.stderr)
    sys.exit(err_code)

def check_attrib(attrib, key, flag=None):
    if flag is None:
        try:
            _ = attrib[key]
        except KeyError:
            print_err(32)
    else:
        try:
            _ = attrib[key]
            return True
        except KeyError:
            return False
    
def check_instr(instruction, opcode):
    #print(opcode, instruction.tag, instruction.attrib, instruction.text)

    if instruction.tag[:len(instruction.tag)-1].lower() != 'arg':
        print_err(32)
    if not(0 < int(instruction.tag[-1]) <= len(inst_dict[opcode])):
        print_err(32)
    arg_num = int(instruction.tag[-1])
    check_attrib(instruction.attrib, 'type')
    if inst_dict[opcode][arg_num] == 'symb' and (instruction.attrib['type'] in ('bool', 'int', 'string') or instruction.attrib['type'] in ('var')):
        if instruction.text is None and instruction.attrib['type'] in ('bool', 'int'):
            print_err(32)
        if instruction.text is None and instruction.attrib['type'] in ('string'):
            instruction.text = ''
        if  re.match(r'^(-|\+)?[0-9]+$', instruction.text) is None\
        and re.match(r'^(false|true)$', instruction.text)  is None\
        and re.match(r'^(([^\x00-\x1F\x7F\xA0\#])+|(\x5C[0-9]{3})+|($))$', instruction.text) is None\
        and re.match(r'^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
                print('HM')
                print_err(32)
        if instruction.attrib['type'] in ('int')    and re.match(r'^(-|\+)?[0-9]+$', instruction.text) is None\
        or instruction.attrib['type'] in ('bool')   and re.match(r'^(false|true)$', instruction.text) is None\
        or instruction.attrib['type'] in ('string') and re.match(r'^(([^\x00-\x1F\x7F\xA0\#])+|(\x5C[0-9]{3})+|($))$', instruction.text) is None\
        or instruction.attrib['type'] in ('var')    and re.match(r'^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
                print_err(52)
    elif inst_dict[opcode][arg_num] == 'var' and instruction.attrib['type'] in ('var'):
        if re.match(r'^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(32)
    elif inst_dict[opcode][arg_num] == 'label' and instruction.attrib['type'] in ('label'):
        if re.match(r'^([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(32)
    elif inst_dict[opcode][arg_num] == 'type' and instruction.attrib['type'] in ('type'):
        if re.match(r'^\s*(int|bool|string)(\s*)$', instruction.text) is None:
            print_err(32)
    else:
        print_err(32)
    
def transform_string(string):
    i = 0
    while i < len(string):
        if string[i] in ('\\'):
            c = int(string[i+1:i+4])
            up = i + 4
            if up >= len(string):
                up = len(string) 
            string = string[0:i] + chr(c) + string[up:]
            i += 1
        else:
            i += 1
    return string

def back_transform(string):
    i = 0
    while i < len(string):
        c = ord(string[i])
        if 0 <= c <= 32 or c == 35 or c == 92:
            string = string[0:i] + '\\0' + str(c) + string[i+1:]
        i+=1
    return string
    
def check_root(xml_root):
    if xml_root.tag.lower() != 'program':
        print_err(32)
    check_attrib(xml_root.attrib, 'language')
    if xml_root.attrib['language'].lower() != 'ippcode18':
        print_err(32)
    if (len(xml_root.attrib) == 3):
        flag = check_attrib(xml_root.attrib, 'name', True)
        flag = check_attrib(xml_root.attrib, 'description', True)
        if flag == False:
            print_err(32)
    elif (len(xml_root.attrib) == 2):
        flag = check_attrib(xml_root.attrib, 'name', True)
        if flag == False:
            flag = check_attrib(xml_root.attrib, 'description', True)
            if flag == False:
                print_err(32)
    elif (len(xml_root.attrib) > 3):
        print_err(31)


def parse_xml_file(xml_namefile):
    global GF, TF, LF, frame_stack, labels_dict, insts
    try:
        xml_tree = ET.parse(xml_namefile)
    except ET.ParseError:
        print_err(31)
    xml_root = xml_tree.getroot()
    
    check_root(xml_root)

    order_array = list()
    for instruction in xml_root.findall('instruction'):
        check_attrib(instruction.attrib, 'opcode')
        check_attrib(instruction.attrib, 'order')
        order_array.append(int(instruction.attrib['order']))
        if instruction.attrib['opcode'] in ('LABEL'):   
            if labels_dict.get(instruction[0].text) is None:
                labels_dict[instruction[0].text] = int(instruction.attrib['order'])
            else:
                print_err(52)
    order_array.sort()

    inst_reader = 0
    while inst_reader != len(xml_root):
        root_index = 0 
        while order_array[inst_reader] != int(xml_root[root_index].attrib['order']):
            root_index += 1
        inst_reader += 1

        elem = xml_root[root_index]
        if elem.tag.lower() != 'instruction': 
            print_err(32)
        #check_attrib(elem.attrib, 'opcode')
        #check_attrib(elem.attrib, 'order')

        if len(elem) > 3:
            print_err(31)
        success = False
        cnt_arg = -1
        arg_order = list()
        for key in inst_dict.keys():
            if elem.attrib['opcode'] in key and len(elem) == inst_dict[key][0]:
                success = True
                cnt_arg = inst_dict[key][0]
                for subelem in elem:
                    arg_order.append(int(subelem.tag[-1]))
                    check_instr(subelem, key)
        if not success:
            print_err(32)

        inst_name = elem.attrib['opcode'].lower()
        if cnt_arg == 0:
            if inst_name in ('return'):
                inst_reader = return_()
            else:
                eval(inst_name)()
        elif cnt_arg == 1:
            if inst_name in ('call'):
                inst_reader = eval(inst_name)(elem[0], inst_reader, order_array)
            elif inst_name in ('jump'):
                inst_reader = call(elem[0], inst_reader, order_array, True)
            else:   
                eval(inst_name)(elem[0])
        elif cnt_arg == 2:
            first_arg = arg_order.index(1)
            second_arg = arg_order.index(2)
            if inst_name in ('not'):
                why_not(elem[first_arg], elem[second_arg])
            elif inst_name in ('type'):
                type_(elem[first_arg], elem[second_arg])
            else:
                eval(inst_name)(elem[first_arg], elem[second_arg])
        elif cnt_arg == 3:
            first_arg = arg_order.index(1)
            second_arg = arg_order.index(2)
            third_arg = arg_order.index(3)
            if inst_name in ('add', 'sub', 'mul', 'idiv'):
                arithmetic(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('lt', 'gt', 'eq'):
                compare(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('and', 'or'):
                logic(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('jumpifeq', 'jumpifneq'):
                inst_reader = jumps(inst_name, elem[first_arg], elem[second_arg], elem[third_arg], inst_reader, order_array) 
            else:
                eval(inst_name)(elem[first_arg], elem[second_arg], elem[third_arg])
        check_vars()
        insts += 1

def check_vars():
    global LF, TF, GF, vars
    cnt = 0
    dict_list = {0: 'LF', 1: 'GF', 2: 'TF'}
    for _, dict_name in dict_list.items():
        if eval(dict_name) is not None:
            for _, value in eval(dict_name).items():
                if value is not None:
                    cnt += 1
    vars = max(cnt, vars)

def check_exist_frame(name):
    global GF, TF, LF
    if eval(name) is None:
        print_err(55)

def check_exist_var(frame, var):
    global GF, TF, LF
    if eval(frame).get(var) is None:
        print_err(54)

def check_undef_var(frame, var):
    global GF, TF, LF
    if eval(frame)[var][0] is None:
        print_err(56)

def move(arg1, arg2):
    if DEBUG:
        print('MOVE')
    global TF, LF, GF
    if arg2.attrib['type'] in ('int', 'string', 'bool'):
        if arg1.text.split('@')[0] in ('LF', 'GF', 'TF'):
            check_exist_frame(arg1.text.split('@')[0])
            check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = arg2.text
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = arg2.attrib['type']
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg1.text.split('@')[0])
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if arg1.text.split('@')[0] in ('LF', 'GF', 'TF'):
            check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
            check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]

def createframe():
    if DEBUG:
        print('CREATEFRAME')
    global TF
    TF = dict()

def pushframe():
    if DEBUG:
        print('PUSHFRAME')
    global TF, LF, frame_stack
    check_exist_frame('TF')
    LF = copy.deepcopy(TF)
    frame_stack.append(LF)
    TF = None

def popframe():
    if DEBUG:
        print('POPFRAME')
    global TF, LF, frame_stack
    if not frame_stack:
        print(55)
    TF = frame_stack.pop()
    if not frame_stack:
        LF = None
    else:
        LF = frame_stack[-1]
    
def defvar(arg):
    if DEBUG:
        print('DEFVAR')
    global TF, LF, GF
    if arg.text.split('@')[0] in ('LF', 'TF'):
        check_exist_frame(arg.text.split('@')[0])
        eval(arg.text.split('@')[0])[arg.text.split('@')[1]] = {0: None, 1: None}
    elif arg.text.split('@')[0] in ('GF'):
        eval(arg.text.split('@')[0])[arg.text.split('@')[1]] = {0: None, 1: None}

def call(label, inst_reader, order_array, push=False):
    if DEBUG:
        print('CALL')
    global call_stack, labels_dict
    if push == False:
        call_stack.append(inst_reader)
    if labels_dict.get(label.text) is None:
        print_err(52)
    else:
        num_order = labels_dict.get(label.text)
        return order_array.index(num_order)

def return_():
    if DEBUG:
        print('RETURN')
    global call_stack
    if not call_stack:
        print_err(56)
    return call_stack.pop()

def pushs(arg):
    if DEBUG:
        print('PUSHS')
    global data_stack
    if arg.attrib['type'] in ('int', 'string', 'bool'):
        data_stack.append([arg.text, arg.attrib['type']])
    elif arg.attrib['type'] in ('var'):
        check_exist_frame(arg.text.split('@')[0])
        check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
        check_undef_var(arg.text.split('@')[0], arg.text.split('@')[1])
        data_stack.append([eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0], eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1]])
    #print(data_stack)

def pops(arg):
    if DEBUG:
        print('POPS')
    global data_stack, GF, LF, TF
    if not data_stack:
        print_err(56)
    check_exist_frame(arg.text.split('@')[0])
    check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
    stack_elem = data_stack.pop()
    eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = stack_elem[0]
    eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] = stack_elem[1]

def check_3_arg(arg2, arg3, arg1=None):
    global LF, TF, GF
    if arg1 is not None:
        check_exist_frame(arg1.text.split('@')[0])
        check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
    if arg3.attrib['type'] in ('var'):
        check_exist_frame(arg3.text.split('@')[0]) 
        check_exist_var(arg3.text.split('@')[0], arg3.text.split('@')[1])
        check_undef_var(arg3.text.split('@')[0], arg3.text.split('@')[1])
    if arg1 is not None:
        return arg1.text.split('@')

def check_type(arg, type_):
    global LF, TF, GF
    operand = None
    if arg.attrib['type'] in ('string', 'bool', 'int'):
        if not(arg.attrib['type'] in (type_)):
            print_err(53)
        return arg.text
    if arg.attrib['type'] in ('var'):
        if not(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in (type_)):
            print_err(53)
        return eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0]

def check_same_type(arg2, arg3, arg1=None):
    global LF, TF, GF
    first_operand = dict()
    second_operand = dict()
    if arg2.attrib['type'] in ('var'):
        first_operand[0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
        first_operand[1] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]
    elif arg2.attrib['type'] in ('string', 'bool', 'int'):
        first_operand = {0: arg2.text, 1: arg2.attrib['type']}
    if arg3.attrib['type'] in ('var'):
        second_operand[0] = eval(arg3.text.split('@')[0])[arg3.text.split('@')[1]][0]
        second_operand[1] = eval(arg3.text.split('@')[0])[arg3.text.split('@')[1]][1]
    elif arg3.attrib['type'] in ('string', 'bool', 'int'):
        second_operand = {0: arg3.text, 1: arg3.attrib['type']}
    if first_operand[1] != second_operand[1]:
        print_err(53)
    if arg1 is not None:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'
    return first_operand, second_operand 
     

def arithmetic(arg1, arg2, arg3, inst_name):
    if DEBUG:
        print(inst_name)
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'int')
    second_operand = check_type(arg3, 'int')
    if inst_name in ('add'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) + int(second_operand)
    elif inst_name in ('sub'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) - int(second_operand)
    elif inst_name in ('mul'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) * int(second_operand)
    elif inst_name in ('div'):
        if int(second_operand) == 0:
            print_err(57)
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) // int(second_operand)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'
    #print(eval(arg1_text[0])[arg1_text[1]])

def recode(operand, mode):
    if mode == 0:
        if operand.lower() in ('false'):
            return 0
        elif operand.lower() in ('true'):
            return 1
    elif mode == 1:
        if operand.lower() in ('false'):
            return False
        elif operand.lower() in ('true'):
            return True

def compare(arg1, arg2, arg3, inst_name):
    if DEBUG:
        print(inst_name)
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand, second_operand = check_same_type(arg2, arg3, arg1)
    if first_operand[1] in ('int'):
        first_operand = int(first_operand[0])
        second_operand = int(second_operand[0])
    elif first_operand[1] in ('bool'):
        first_operand = recode(first_operand[0], 0)
        second_operand = recode(second_operand[0], 0)
    elif first_operand[1] in ('string'):
        first_operand = first_operand[0]
        second_operand = second_operand[0]
    if inst_name in ('lt'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand < second_operand).lower()
    elif inst_name in ('gt'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand > second_operand).lower()
    elif inst_name in ('eq'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand == second_operand).lower()
    #print(eval(arg1_text[0])[arg1_text[1]])


def logic(arg1, arg2, arg3, inst_name):
    if DEBUG:
        print(inst_name)
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'bool')
    second_operand = check_type(arg3, 'bool')
    first_operand = recode(first_operand[0], 1)
    second_operand = recode(second_operand[0], 1)
    if inst_name in ('and'):
        eval(arg1_text[0])[arg1_text[1]][0] = str(first_operand and second_operand).lower()
    elif inst_name in ('or'):
        eval(arg1_text[0])[arg1_text[1]][0] = str(first_operand or second_operand).lower()
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'
    #print(eval(arg1_text[0])[arg1_text[1]])

def check_2_arg(arg1, arg2, type_):
    global LF, TF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in (type_):
        return arg2.text
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1] in (type_):
            return eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
        else:
            print_err(53)
    else:
        print_err(53)

def why_not(arg1, arg2):
    if DEBUG:
        print('NOT')
    global LF, TF, GF
    arg2.text = check_2_arg(arg1, arg2, 'bool')
    arg2.text = recode(arg2.text, 1)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(not(arg2.text)).lower()
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def int2char(arg1, arg2):
    if DEBUG:
        print('INT2CHAR')
    global LF, TF, GF
    arg2.text = check_2_arg(arg1, arg2, 'int')
    try:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(chr(int(arg2.text)))
    except ValueError:
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def stri2int(arg1, arg2, arg3):
    if DEBUG:
        print('STRI2INIT')
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    first_operand = transform_string(first_operand) #
    second_operand =  int(check_type(arg3, 'int'))
    if second_operand >= len(first_operand):
        print_err(58)
    try:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = ord(first_operand[second_operand])
    except TypeError:
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def read(arg1, arg2):
    if DEBUG:
        print('READ')
    global LF, TF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = arg2.text
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = input()
    if arg2.text in ('int'):
        if re.match(r'^(-|\+)?[0-9]+$', eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0]) is None:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 0
        else:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = int(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0])
    elif arg2.text in ('bool'):
        if eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0].lower() in ('true'):
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 'true'
        else:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 'false'
    elif arg2.text in ('string'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0])
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0])

def write(arg):
    if DEBUG:
        print('WRITE')
    global LF, TF, GF
    tmp_str = 'ERROR'
    if arg.attrib['type'] in ('bool'):
        tmp_str = recode(arg.text, 1)
    elif arg.attrib['type'] in ('string'):
        tmp_str = transform_string(arg.text)
    elif arg.attrib['type'] in ('var'):
        check_exist_frame(arg.text.split('@')[0])
        check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
        check_undef_var(arg.text.split('@')[0], arg.text.split('@')[1])
        if eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in ('bool'):
            eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = recode(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0], 1)
        if eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in ('string'):
            eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = transform_string(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0])
        tmp_str = eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0]
    else:
        tmp_str = arg.text
    print(tmp_str)


def concat(arg1, arg2, arg3):
    if DEBUG:
        print('CONCAT')
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    second_operand = check_type(arg3, 'string')
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = first_operand + second_operand
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def strlen(arg1, arg2):
    if DEBUG:
        print('STRLEN')
    global LF, TF, GF
    tmp_str = check_2_arg(arg1, arg2, 'string')
    tmp_str = transform_string(tmp_str)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = int(len(tmp_str))
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def getchar(arg1, arg2, arg3):
    if DEBUG:
        print('GETCHAR')
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    first_operand =  transform_string(first_operand)
    second_operand =  int(check_type(arg3, 'int'))
    if second_operand >= len(first_operand):
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(str(first_operand[second_operand]))
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def setchar(arg1, arg2, arg3):
    if DEBUG:
        print('SETCHAR')
    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    target = check_type(arg1, 'string')
    target = transform_string(target)
    first_operand = int(check_type(arg2, 'int'))
    second_operand = check_type(arg3, 'string')
    second_operand = transform_string(second_operand)
    if first_operand >= len(target) or second_operand in (''):
        print_err(58)
    if first_operand < 0:
        first_operand = len(target) + first_operand
        if first_operand < 0:
            print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(target[:first_operand] + second_operand[0] + target[first_operand+1:])
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'
    print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def type_(arg1, arg2):
    if DEBUG:
        print('TYPE')
    global TF, LF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in ('int', 'bool', 'string'):
         eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = arg2.attrib['type']
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0] is None:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = ''
        else:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'
    #print(eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]])

def label(arg):
    if DEBUG:
        print('LABEL', arg.text)
    pass

def jumps(inst_name, arg1, arg2, arg3, inst_reader, order_array):
    if DEBUG:
        print(inst_name)
    global TF, LF, GF
    check_3_arg(arg2, arg3)
    first_operand, second_operand = check_same_type(arg2, arg3)
    if first_operand[1] in ('int'):
        first_operand = int(first_operand[0])
        second_operand = int(second_operand[0])
    elif first_operand[1] in ('bool'):
        first_operand = recode(first_operand[0], 0)
        second_operand = recode(second_operand[0], 0)
    elif first_operand[1] in ('string'):
        first_operand = first_operand[0]
        second_operand = second_operand[0]
    if inst_name in ('jumpifeq') and first_operand == second_operand:
        return call(arg1, inst_reader, order_array, True)
    elif inst_name in ('jumpifneq') and first_operand != second_operand:
        return call(arg1, inst_reader, order_array, True)
    else:
        return inst_reader
        
def main(argv):
    global insts, vars
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
    print('INSTS =', insts)
    print('VARS  =', vars)

if __name__ == "__main__":
    main(sys.argv[1:])
