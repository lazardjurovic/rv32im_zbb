parsed_program = []
parsed_data = []

# function to check if string represents hex value
# used in parsing .elf files
def is_hex(string):
    try:
        int(string, 16)
        return True
    except ValueError:
        return False

def reverse_endian(s):
    substrings = [s[i:i+4] for i in range(0, len(s), 4)]
    substrings = substrings[::-1]

    for i in range(0, len(substrings)-1, 2):
        substrings[i], substrings[i+1] = substrings[i+1], substrings[i]
    
    merged_string = ''.join(substrings)
    return merged_string

# opening .txt section
with open("mul_text.dump", "r") as file:
    for line in file:
        parsed_program.append(line)

# processign .txt section
parsed_program = [s for s in parsed_program if s != '\n']
parsed_program = [s for s in parsed_program if ">:" not in s] # remove lines containing labelss
parsed_program = [s.replace('\n','') for s in parsed_program]
parsed_program = [s.replace('\t','') for s in parsed_program]
parsed_program = parsed_program[2:]
parsed_program = [s.split() for s in parsed_program]
parsed_program = [s[0] for s in parsed_program]
parsed_program = [s.split(':')[1] for s in parsed_program]
parsed_program = [bin(int(s, 16))[2:].zfill(32) for s in parsed_program] # convert to 32 bit binary string
parsed_program = parsed_program[:-2] # remove last two elements (sustitute li and ret)
parsed_program.append("00000000000000001010000000100011") # insert SW
parsed_program.append("00001000000000010010001000100011") # insert SW
parsed_program.append("00000000000000000000000001110011") # insert ECALL

# opening .data section
with open("mul_data.dump", 'r') as file:
    for line in file:
        parsed_data.append(line)

#processing .data section
parsed_data = [s for s in parsed_data if s != '\n']
parsed_data = parsed_data[2:]
parsed_data = [s.split()[1:] for s in parsed_data]
parsed_data = [item for sublist in parsed_data for item in sublist] # flatten into one big list
parsed_data = [item for i, item in enumerate(parsed_data) if i % 5 != 4] # take 4 elements and skip one, repeat unitll end of list
parsed_data = [s for s in parsed_data if is_hex(s)]
parsed_data = [bin(int(s, 16))[2:].zfill(32) for s in parsed_data] # convert to 32 bit binary string
parsed_data = [reverse_endian(s) for s in parsed_data]

# writing to data memory file
with open("data_mem.txt", 'w') as file:

    # write zeros before start of .data specifide by linker
    for i in range(8192):
        file.write('00000000000000000000000000000000')
        file.write('\n')

    for line in parsed_data:
        file.write(line)
        file.write('\n')

# writing to instruction memory file
with open("instr_mem.txt", "w") as file:
    for line in parsed_program:
        file.write(line)
        file.write('\n')

