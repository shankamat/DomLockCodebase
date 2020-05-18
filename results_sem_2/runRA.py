import os
import subprocess

NAryRange = [4]
LevelRange =[7]
requestFractionRange = [0.2,0.5,0.8]
RadiusRange = [2,4,8,16]
ShuffleProbRange = [2,0]
SkewnessRange = [1,2,4,8]
noOfThreads = 8
typeOfHierarchy = 'd'
caseParameter = 11


for NAry in NAryRange:
	for level in LevelRange:
		for requestFraction in requestFractionRange:
			sizeOfHierarchy = (NAry**level-1)/(NAry-1)
			noOfRequestedNodes = int(sizeOfHierarchy*requestFraction)
			for radius in RadiusRange:
				for shuffleProb in ShuffleProbRange:
					for skewness in SkewnessRange:
						with open("output_RA_4_7.txt","a") as out:
							proc = subprocess.Popen(["./bin/a.out", str(NAry), str(level), str(noOfRequestedNodes), str(radius), str(shuffleProb), str(noOfThreads), str(caseParameter), str(skewness), 's', '0', str(typeOfHierarchy)], stdout = out)
						proc.wait()
