# generate random integer values
from random import seed
from random import randint
import sys
# seed random number generator
seed(1)
# generate some integers
for _ in range(100):
	value = randint(1, 10)
	print(value, end=' ')
	