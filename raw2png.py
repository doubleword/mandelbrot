# Converts a raw bitmap into a png image
# pip install Pillow


import argparse
import sys
from PIL import Image


parser = argparse.ArgumentParser()

parser.add_argument('width',type=int,help='width of a bitmap')
parser.add_argument('height',type=int,help='height of a bitmap')
parser.add_argument('bitmap',type=argparse.FileType('rb'),help='bitmap file',metavar='file')

args=parser.parse_args()


data=list(args.bitmap.read())

if len(data)!=args.width*args.height*4:
    print('Bitmap resolution is invalid!')
    sys.exit(1)


rgbdata=[]
for i in range(0,len(data),4):
    rgbdata.append(tuple(data[i:i+3][::-1]))


im=Image.new('RGB',(args.width,args.height))
im.putdata(rgbdata)
im.save(args.bitmap.name+'.png')

