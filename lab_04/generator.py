import random
data = [str(i) for i in range(0,10000)]
random.shuffle(data)
print(' '.join(data))

data1 = [str(i) for i in range(0,10000000)]
random.shuffle(data1)
print(' '.join(data1))
