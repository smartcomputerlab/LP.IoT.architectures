import matplotlib.pyplot as plt
import numpy as np

ihpmil1=float(input("Input average current value for high power stage in mA (case1): "))
ihpmil2=float(input("Input average current value for high power stage in mA (case2): "))
ihpmil3=float(input("Input average current value for high power stage in mA (case3): "))
thp1=float(input("Input time of high power stage in sec (case1): "))
thp2=float(input("Input time of high power stage in sec (case2): "))
thp3=float(input("Input time of high power stage in sec (case3): "))
ilp1=float(input("Input average current value for low power stage in mA (case1): "))
ilp2=float(input("Input average current value for low power stage in mA (case2): "))
ilp3=float(input("Input average current value for low power stage in mA (case3): "))

avi1 = np.arange(ilp1+1.0,20.0, 0.1)  # average current in mA
avi2 = np.arange(ilp2+1.0,20.0, 0.1)  # average current in mA
avi3 = np.arange(ilp3+1.0,20.0, 0.1)  # average current in mA
tlp1=thp1*(ihpmil1-avi1)/(avi1-ilp1)
tlp2=thp2*(ihpmil2-avi2)/(avi2-ilp2)
tlp3=thp3*(ihpmil3-avi3)/(avi3-ilp3)
# create data
x2 = [0.01,0.1,1,10,100,1000]
y2 = [10,10,10,10,10,10]

fig, ax = plt.subplots()
plt.yscale("log")
ax.grid()
ax.set(ylabel='time in seconds', xlabel='average current in mA',title='low_power time')
ax.plot(avi1, tlp1, label="case 1")
ax.plot(avi2, tlp2, label="case 2")
ax.plot(avi3, tlp3, label="case 3")
ax.plot(y2,x2,label="10mA")
plt.legend()
fname=str(input("Give the name of the graph file: "))
fig.savefig(fname)
plt.show()

