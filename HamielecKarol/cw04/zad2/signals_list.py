print("hello")

infile = open('ttt.txt', 'r')
outfile = open('signal_list.txt', 'w+')
for line in infile:
    row = line.split(sep=None)
    string = "\"" + row[1] + "\"" + ", "
    print(string)
    outfile.write(string)