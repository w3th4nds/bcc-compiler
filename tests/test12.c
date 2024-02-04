int func(void)
{
  int a = 5;
  int b = 2;
  return a * b + 10;
}

int main(void)
{
  int x = 0x10;
  int y = func() * 4;
  int a = (y + 20) * x;
  return a;
}
