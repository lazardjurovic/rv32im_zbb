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
with open("text.dump", "r") as file:
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

# Storing whole register file into data memory
parsed_program.append("00000000000100000010000000100011") # insert SW
parsed_program.append("00000000001000000010100000100011") # insert SW
parsed_program.append("00000010001100000010000000100011") # insert SW
parsed_program.append("00000010010000000010100000100011") # insert SW
parsed_program.append("00000100010100000010000000100011") # insert SW
parsed_program.append("00000100011000000010100000100011") # insert SW
parsed_program.append("00000110011100000010000000100011") # insert SW
parsed_program.append("00000110100000000010100000100011") # insert SW
parsed_program.append("00001000100100000010000000100011") # insert SW
parsed_program.append("00001000101000000010100000100011") # insert SW
parsed_program.append("00001010101100000010000000100011") # insert SW
parsed_program.append("00001010110000000010100000100011") # insert SW
parsed_program.append("00001100110100000010000000100011") # insert SW
parsed_program.append("00001100111000000010100000100011") # insert SW
parsed_program.append("00001110111100000010000000100011") # insert SW
parsed_program.append("00001111000000000010100000100011") # insert SW
parsed_program.append("00010001000100000010000000100011") # insert SW
parsed_program.append("00010001001000000010100000100011") # insert SW
parsed_program.append("00010011001100000010000000100011") # insert SW
parsed_program.append("00010011010000000010100000100011") # insert SW
parsed_program.append("00010101010100000010000000100011") # insert SW
parsed_program.append("00010101011000000010100000100011") # insert SW
parsed_program.append("00010111011100000010000000100011") # insert SW
parsed_program.append("00010111100000000010100000100011") # insert SW
parsed_program.append("00011001100100000010000000100011") # insert SW
parsed_program.append("00011001101000000010100000100011") # insert SW
parsed_program.append("00011011101100000010000000100011") # insert SW
parsed_program.append("00011011110000000010100000100011") # insert SW
parsed_program.append("00011101110100000010000000100011") # insert SW
parsed_program.append("00011101111000000010100000100011") # insert SW
parsed_program.append("00011111111100000010000000100011") # insert SW

# Adding ECALL to signal the end of the program
parsed_program.append("00000000000000000000000001110011") # insert ECALL

# Insert '0' at the beginning and shift the rest
parsed_program = ['00000000000000000000000000000000'] + parsed_program


# opening .data section
with open("data.dump", 'r') as file:
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
    # for i in range(8192):
    #     file.write('00000000000000000000000000000000')
    #     file.write('\n')

    for line in parsed_data:
        file.write(line)
        file.write('\n')

# writing to instruction memory file
with open("instr_mem.txt", "w") as file:
    for line in parsed_program:
        file.write(line)
        file.write('\n')

