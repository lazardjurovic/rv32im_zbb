parsed_program = []

with open("text.dump", "r") as file:
    for line in file:
        parsed_program.append(line)

parsed_program = [s for s in parsed_program if s != '\n']
parsed_program = [s.replace('\n','') for s in parsed_program]
parsed_program = [s.replace('\t','') for s in parsed_program]
parsed_program = parsed_program[3:]
parsed_program = [s.split() for s in parsed_program]
parsed_program = [s[0] for s in parsed_program]
parsed_program = [s.split(':')[1] for s in parsed_program]
parsed_program = [bin(int(s, 16))[2:].zfill(32) for s in parsed_program]

with open("text_binary.txt", "w") as file:
    for line in parsed_program:
        file.write(line)
        file.write('\n')

