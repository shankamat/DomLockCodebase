import numpy as np



from sys import argv

for arg in argv[1:]:
	data = np.genfromtxt(arg, delimiter=',')
	#print(data[:,10])
	for a in [1,2,4,8]:
		cnt = 0
		sum1 = 0
		for itr in range(np.size(data,0)):
			if data[itr,7] == a and data[itr,10]!='inf' and data[itr,10]!=0:			
					sum1 = sum1+ data[itr,10]
					cnt = cnt + 1
		print(a," ",sum1/cnt)


