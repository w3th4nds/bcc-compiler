int dummy(int x)
{
  return x + 2 * x;
}

int main(void)
{
  int x = 50;
  if (x <= 40) {
    return dummy(x);
  }
  else {
    x = 1;
    return x;
  }
}
