import sys
import csv

with open(sys.argv[1], newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='"')
    it = iter(spamreader)
    next(it)
    for row in it:
        print(row[2], row[7], row[8])
