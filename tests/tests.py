import sys, os, filecmp, os.path

counter = 1
error = False
while counter <= 50:
    ic17int = 'tests/ic17int'
    test_path = 'tests/test'+str(counter)+'.code'
    test_output = 'tests/test'+str(counter)+'.output'
    test_input = 'tests/test'+str(counter)+'.input'
    input_ic17int = 'tests/test.code' 
    output = 'tests/output.output'

    os.system('./ifj17 <%(test_path)s >%(input_ic17int)s 2>%(output)s' % locals())

    with open(output, 'r') as f:
        first_line = f.readline()
    with open(output, 'r+') as f:
        lines = f.readlines()
        f.seek(0)
        f.truncate()
        for line in lines:
            if 'lexical' in line[37:45]:
                line = line.replace(line, str(1))
                error = True
            elif 'syntax' in line[37:44]:
                line = line.replace(line, str(2))
                error = True
            elif 'error' in line[9:15]:
                line = line.replace(line, str(3))
                error = True
            elif 'type' in line[9:14]:
                line = line.replace(line, str(4))
                error = True
            elif 'Others' in line [0:7]:
                line = line.replace(line, str(6))
                error = True
            elif 'Internal' in line [0:9]:
                line = line.replace(line, str(99))
                error = True
            f.write(line)

    if os.path.exists(test_input) and error == False:
        os.system('%(ic17int)s %(input_ic17int)s <%(test_input)s >%(output)s' % locals())
    elif error == False:
        os.system('%(ic17int)s %(input_ic17int)s >%(output)s' % locals())

    if filecmp.cmp(output, test_output):
        print ('Tests no.%(counter)d is succesful!' % locals())
    else:
        print ('Tests no.%(counter)d is unsuccesful!' % locals())
        
    os.remove(output)
    os.remove(input_ic17int)

    error = False
    counter = counter + 1
