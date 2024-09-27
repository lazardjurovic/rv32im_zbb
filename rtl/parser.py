print("\n\n")
print("********************************************")
print("*                       *")
print("*    PARSING TIMING ANALYSIS DATA      *")
print("*                       *")
print("********************************************")
print("\n\n")
print("Extracting all Max Delay Paths from report.")
print("\n\n")
print("**********************************************************************")

class Timing:
    def __init__(self, source, dest, delay):
        self.source = source
        self.dest = dest
        self.delay = delay
        
        #print(f"Parsing delay: {delay}")  # Debug print
        
        # extract delays
        delay_split = self.delay.split()
        #print(f"Delay split: {delay_split}")  # Debug print
        
        try:
            self.delay_number = float(delay_split[3][:-2])
            self.delay_logic = float(delay_split[5][:-2])
            self.delay_route = float(delay_split[8][:-2])
        except IndexError as e:
            print(f"Error parsing delay: {e}")
            self.delay_number = 0.0
            self.delay_logic = 0.0
            self.delay_route = 0.0
                
    def print_data(self):
        print("\n")
        print(self.source)
        print(self.dest)
        print(self.delay)
        print("\n")
    
    def get_data(self):
    	data = ""
    	data = self.source + self.dest + self.delay + "\n\n"
    	return data

text_data = []
min_delay_paths = "Min Delay Paths"
max_delay_paths = "Max Delay Paths"
block_begin = "Slack (MET) :"
index = 0
flag = 0
timing_list = []

with open("timing_report.txt", "r") as file:
    for line in file:
        text_data.append(line)
        
# Take just paths that are in max delay section
for line in text_data:
    index += 1
    if max_delay_paths in line:
        flag = 1
    if min_delay_paths in line:
        flag = 0
        
    if flag == 1:
        if block_begin in line:
            print(line)
            for i in range(0, 19):
                print(text_data[index + i])
            print("**********************************************************************")
            
            timing = Timing(text_data[index] + text_data[index + 1], text_data[index + 2] + text_data[index + 3], text_data[index + 7])
            timing_list.append(timing)

print("\n")
print("Sorting gathered data according to its Data Path Delay.")
print("\n")    

timing_list_sorted = sorted(timing_list, key=lambda x: x.delay_number, reverse=True)

for timing in timing_list_sorted:
    timing.print_data()
    
print("\n\nResults are stored in analisys_result.txt file. \n\n")
    
with open("analisys_result.txt", "w") as out_file:
	for timing in timing_list_sorted:
		out_file.write(timing.get_data())
	   

