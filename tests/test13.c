int add(int a, int b)
{
  return a + b;
}

int mul(int a, int b)
{
  return a * b;
}

int main()
{
  int x = 2;
  int y = 3;

  int z = add(x, y);
  int w = mul(z, 5);

  return w * 4;
}
