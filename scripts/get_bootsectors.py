if __name__ == "__main__":
	with open("empty-diskette.img", 'rb') as f:
		boot = f.read(512)
	with open('boot.txt', 'w') as f:
		for i in boot:
			f.write("\\x%02x" %(i))