# import argparse

def write_bytes(ifile, ofile, bs=512, seek=0):
	print("[#] Reading %s" %(ifile))
	ifile = open(ifile, 'rb')
	data = ifile.read()
	ifile.close()
	
	print("[#] Writing %d bytes to %s" %(len(data), ofile))
	ofile = open(ofile, 'r+b')
	ofile.seek(seek * bs)
	ofile.write(data)
	ofile.close()
	print("[ + ] Write succesful\n")

# parser = argparse.ArgumentParser()
# parser.add_argument("--input", dest="ifile", action="store", required=True)
# parser.add_argument("--output", dest="ofile", action="store", required=True)
# parser.add_argument("--seek", dest="seek", action="store", type=int)
# parser.add_argument("--bs", dest="bs", action="store", type=int)
# args = parser.parse_args()

# ifile = open(args.ifile, 'rb')
# data = ifile.read()
# ifile.close()

# ofile = open(args.ofile, 'r+b')
# ofile.seek(args.seek * args.bs, 0)
# ofile.write(data)
# ofile.close()