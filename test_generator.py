#test_generator.py
#takes args for percent insertion, percent search, and total ops
import sys, getopt
import random

TEST_DIR = "tests/"

def generateTest(insertionPercent, totalOps, output):
    with open(TEST_DIR + output, "wb") as f:
        outputText = ""
        for _ in xrange(totalOps):
            if (random.random() < insertionPercent):
                outputText += ("i %d\n" % random.randint(1, totalOps))
            else:
                outputText += ("s %d\n" % random.randint(1, totalOps))
        f.write(outputText)

def usage():
    print 'Usage:'
    print 'python test_generator.py --insertion-percent=<percent of ops that will insert (0 to 100)> --total=<total number of ops> --output=<output file>'
    sys.exit(2)

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "i:t:o:", ["insertion-percent=", "total=", "output="])
    except getopt.GetoptError:
        usage()
    if (len(opts)!= 3):
        usage()
    for opt, arg in opts:
        if opt in ("-i", "--insertion-percent"):
            insertionPercent = 1.0*int(arg)/100
        elif opt in ("-t", "--total"):
            totalOps = int(arg)
        elif opt in ("-o", "--output"):
            output = arg
    generateTest(insertionPercent, totalOps, output)


if __name__ == "__main__":
    main(sys.argv[1:])
