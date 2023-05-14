import matplotlib.pyplot as plt
import csv

def draw(csv_name):
  with open("./" +csv_name, 'r') as file:
    csvreader = csv.reader(file)
    
    x = list(csvreader)
    a =len(x)
    #print("# cases:")
    #print(a)


  for xi in x:
    len_xi =len(xi)
    
    #print("# data :")
    #print(len_xi)
    nums = []
    for j in range(len_xi-1):
      nums.append( float(xi[j]) )
    plt.plot(nums)
    plt.yscale('symlog')
  file.close()

  plt.xlabel('time')
  plt.title(csv_name)
  #plt.show()
  plt.savefig(csv_name+ ".png")
  plt.clf() 
draw("GA_B.csv")
