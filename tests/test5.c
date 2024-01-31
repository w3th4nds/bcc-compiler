int a = 1;

int func1()
{
  return 1;
}

int func2(int b, long c)
{
  return 2;
}

int main(int d, int e)
{
  // assign
  long f = 3;
  // expr
  int g = d + e * f + 5;
  long h;
  h = g * 10;
  // calls
  func1();
  // with args
  func2(e, 5);
  // return
  return e;
}
