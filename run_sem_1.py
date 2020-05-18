import os
import subprocess

#hirarchySizeRange = [10,50,100,500,1000]
hirarchySizeRange = [10000]
requestFractionRange = [0.2,0.4]
#skewnessRange = [1,2,4]
skewnessRange = [1,2,4,8,16,32]
thresholdRange = [1,2,3,4,5,6,7,8,9,10]
noOfThreads = 8
typeOfHierarchy = 't'
caseParameter = 5
alpha = 9

for sizeOfHierarchy in hirarchySizeRange:
	for requestFraction in requestFractionRange:
		noOfRequestedNodes = sizeOfHierarchy*requestFraction
		for skewness in skewnessRange:
				for threshold in thresholdRange:
					with open("output_MD_10000_t.txt","a") as out:
						proc = subprocess.Popen(["./bin/a.out", str(sizeOfHierarchy), str(noOfThreads), str(caseParameter), str(noOfRequestedNodes), str(skewness), 's', '0', str(typeOfHierarchy), str(threshold), str(alpha)], stdout = out)
						proc.wait()
