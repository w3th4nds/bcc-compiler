int func(void)
{
  int a = 5;
  int b = 2;
  return a * b + 10;
}

// should return 23
int main(void)
{
  int x = 0x10;
  int y = func() +  4 * 20;
  int a = 3 + x * (((5 + 3) * y) - (30 + func())) - 12 * 1000;
  return a + 20;
}
