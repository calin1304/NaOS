#!/usr/bin/env python3

import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Add files to be loaded to image")
    parser.add_argument('-e', type=str, nargs=1, required=True, dest="img_entry")
    parser.add_argument('-f', type=str, nargs='*', dest="files")
    parser.add_argument('-o', type=str, nargs=1, required=True, dest="output_filename")
    args = parser.parse_args()

    with open(args.img_entry[0], 'rb') as f:
        img_data = f.read()
    curr_offset = len(img_data)
    file_offset = {}
    file_size = {}
    for i in args.files:
        file_offset[i] = curr_offset
        with open(i, 'rb') as f:
            data = f.read()
            file_size[i] = len(data)
            curr_offset += len(data)
        img_data += data

    for (k, v, sz) in zip(file_offset.keys(), file_offset.values(), file_size.values()):
        img_data += (bytes(k, encoding='ascii') + b'=' + v.to_bytes(4, byteorder="little") + sz.to_bytes(4, byteorder="little") + int().to_bytes(1, byteorder='little'))
    img_data += int(curr_offset).to_bytes(4, byteorder='little')
    with  open(args.output_filename[0], 'wb') as f:
        f.write(img_data)
