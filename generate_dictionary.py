import sys
import random
from faker import Factory

if len(sys.argv) < 2:
    print('no N given', file=sys.stderr)
    sys.exit(1)
try:
    n = int(sys.argv[1])
    if n < 0:
        raise ValueError
except ValueError:
    print('N should be a positive integer', file=sys.stderr)
    sys.exit(1)

def fake_age():
    return random.randint(1, 100)

fake = Factory.create()
for _ in range(n):
    print('{};{};{}'.format(fake.name(), fake_age(), fake.email()))
