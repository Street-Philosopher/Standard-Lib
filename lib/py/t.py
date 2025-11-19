
def xmacro(fname):
	f = open(fname)
	out = open(f"src/{fname.split(".")[0]}.lst", "w")

	symbols = []

	maxlen = 0

	for line in f:
		line = line
		if line.startswith("#define"):
			line = line.split()
			if len(line[1]) > maxlen:
				maxlen = len(line[1])
		symbols.append(line)


	for real in symbols:
		if isinstance(real, str):
			out.write(real)
		else:																		# +1 bc of the "forced characters" we're writing (len("X(, )") // 4 == +1)
			out.write("X(" + real[1] + ", " + real[2] + ")" + ("\t" * (1 + (maxlen - (1 + len(real[1]) + len(str(real[2])))) // 4)) + "\t\t\t" + " ".join(real[3:]) + "\n")

	f.close(); out.close()

def enumify(fname):
	f = open(fname)

	symbols = []

	maxlen = 0

	for line in f:
		line = line
		if line.startswith("#define"):
			line = line.split()
			symbols.append(line)
			if len(line[1]) > maxlen:
				maxlen = len(line[1])

	print(maxlen)

	f.close()
	f = open(fname, "a")

	f.write("\n\nenum errornums {\n")

	for s in symbols:
		f.write("\t" + s[1] + " = " + s[2] + "," + ("\t" * (1 + (maxlen - (len(s[1]) + len(str(s[2])))) // 4)) + "\t\t\t" + " ".join(s[3:]) + "\n")

	f.write("};")
	f.close()

xmacro("error.h")