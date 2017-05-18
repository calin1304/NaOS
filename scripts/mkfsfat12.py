# import argparse

# parser = argparse.ArgumentParser()
# parser.add_argument("-o", dest="ofile", action="store", required=True)
# args = parser.parse_args()

def make_empty_img(ofile):
	print("[#] Formating file %s" %(ofile))
	f = open(ofile, "wb")
	sector_size = 512
	boot = b'\x00' * (sector_size * 1)
	fat = (b'\xf8\xff\xff' + b'\x00' * (sector_size * 9 - 3))
	root = b'\x00' * (sector_size * 14)
	data = b'\x00' * (sector_size * 2847)
	f.write(boot + fat + fat + root + data)
	f.close()
	print("[ + ] Operation complete\n")
