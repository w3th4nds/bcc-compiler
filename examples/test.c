/*
  This is a
  multi
  line
  comment
*/

// this is a one line comment

int main()
{
  char buf[] = "this is a string";

  for (int i = 0; i < 10; ++i) {
    printf("i = %d\n", i);
  }

  int a = 5 + 9;
  a += 10;
  int b = a--;

  int x = 27;
  int y = x * b;
  x *= 59;

  int x = a % 100;
  x %= y;

  int value = x;
  value ^= 100;

  value = value << 8;
  value = value >> 9;

  return 0;
}
