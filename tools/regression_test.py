import sys
import difflib
import filecmp

def regression():

    file1 = open("output/unit_test/run0/stats.yml",'r')
    file2 = open("output/unit_test/run1/stats.yml",'r')

    lines1 = file1.read().splitlines()
    lines2 = file2.read().splitlines()

    diff = difflib.HtmlDiff().make_file(lines1, lines2, "Before", "After")

    fileDiff = open("diff.html", 'w')
    fileDiff.write(diff)
    fileDiff.close()

    # filecmp.cmp('output/unit_test/run0/stats.yml','output/unit_test/run1/stats.yml')

if __name__ == "__main__":
    regression()
