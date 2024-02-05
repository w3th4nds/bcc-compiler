long dummy(void)
{
  int a = 0x20;
  return a - 0x10;
}

// should return 200
int main(void)
{
  long x = dummy();
  int y = 5 + x * ((4 + 3) * 10) - 1040;
  int a = x + y * 2;
  return a + 7 * 2;
}
