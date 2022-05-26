import sys

def main():
	inFile = sys.argv[1]
	argCount = 4
	stackCount = 2
	valuesArray = []
	argSum = argCount + stackCount

	for index in range(0, argSum):
		valuesArray.append([])

	with open(inFile) as f:
		content = f.read()
		byteTotal = int(len(content)/2)
		temp = byteTotal % ((argSum)*4)

		zeroCount = 0
		if temp != 0:
			zeroCount = ((argSum) * 4 - byteTotal % ((argSum) * 4)) * 2

		content = content + ('0' * zeroCount)
		content = content[::-1]
		strLength = len(content)
		stackTotal = 0

		for i in range(0, strLength, (argSum)*8):
			for j in range(0, argCount+stackCount):
				strIndex = i + j * 8
				valuesArray[j].append("0x" + numChange(content[strIndex:strIndex+8]))
			stackTotal = stackTotal + 1
		#print(valuesArray)

		f = open("stack_payload.txt", "w")

		f.write("int stackTotal = {0};\n".format(stackTotal))

		for i in range(0, argSum):
			f.write("DWORD arg{0}[] = {{".format(i))
			str = ', '
			f.write(str.join(valuesArray[i]))
			f.write("};\n")
		f.close()

def numChange(num):
	return num[1]+num[0]+num[3]+num[2]+num[5]+num[4]+num[7]+num[6]

if __name__ == "__main__":
	main()