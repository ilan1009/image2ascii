#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int checkFile(FILE *fptr)
{
  if (fptr == NULL)
  {
    printf("Couldn't open file :(\n");
    return 1;
  }
  char f1, f2;
  if (fscanf(fptr, "%c%c", &f1, &f2) != 2)
  {
    printf("Error finding header, likely not a PGM file.\n");
    return 1;
  }
  printf("%c%c", f1, f2);
  if (!(f1 == 'P' && f2 == '2'))
  {
    printf("File doesn't appear to be a PGM file, exiting.\n");
    return 1;
  }
  printf("File checked...\n");
  return 0;
}

unsigned int index2d(unsigned int x, unsigned int y, unsigned int width)
{
  return x + y * width;
}

unsigned char calculateCharacter(unsigned char greyValue, unsigned char maxValue, unsigned char threshold)
{
  const unsigned char charSet[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
  const unsigned char charSetLen = 70;
  if (greyValue == 0)
    return ' ';

  unsigned char index = ((int)greyValue * (charSetLen - 1) + maxValue - 1) / maxValue;
  if (index < threshold)
    return ' ';
  return charSet[index];
}

int main(int argc, char *argv[])
{
  unsigned char threshold = 0;
  if (argc < 2)
  {
    printf("filepath required\n");
    return 1;
  }
  if (argc > 2)
  {
    threshold = atoi(argv[2]);
  }

  FILE *out_fptr = NULL;
  int use_terminal = 1;
  if (argc > 3)
  {
    out_fptr = fopen(argv[3], "w");
    if (out_fptr == NULL)
    {
      perror("Error opening out file");
      return 1;
    }
    use_terminal = 0;
  }

  FILE *fptr = fopen(argv[1], "r");
  if (checkFile(fptr) != 0)
  {
    if (out_fptr != NULL) fclose(out_fptr);
    return 1;
  }

  char ch;
  ch = getc(fptr);
  while (ch == '#')
  {
    while (getc(fptr) != '\n');
    ch = getc(fptr);
  }
  ungetc(ch, fptr);

  unsigned int width, height;
  if (fscanf(fptr, "%u %u", &width, &height) != 2)
  {
    printf("Invalid image size\n");
    if (out_fptr != NULL) fclose(out_fptr);
    fclose(fptr);
    return 1;
  }

  unsigned char max_value;
  if (fscanf(fptr, "%hhu", &max_value) != 1)
  {
    printf("Couldn't read max value\n");
    if (out_fptr != NULL) fclose(out_fptr);
    fclose(fptr);
    return 1;
  }

  printf("w: %d\nh: %d\nm: %d\n", width, height, max_value);

  unsigned char greyValue;
  unsigned int x = 0, y = 0;

  while ((ch = fgetc(fptr)) != EOF)
  {
    while (isspace(ch))
    {
      ch = fgetc(fptr);
    }
    if (isdigit(ch))
    {
      greyValue = 0;
      do
      {
        greyValue = greyValue * 10 + (ch - '0');
        ch = fgetc(fptr);
      } while (isdigit(ch));
      ungetc(ch, fptr);
      
      if (use_terminal) printf("%c ", calculateCharacter(greyValue, max_value, threshold));
      else fprintf(out_fptr, "%c ", calculateCharacter(greyValue, max_value, threshold));

      if (++x >= width)
      {
        x = 0;
        y++;
        if (use_terminal) printf("\n");
        else fprintf(out_fptr, "\n");
      }
    }
  }

  fclose(fptr);
  if (out_fptr != NULL) fclose(out_fptr);
  return 0;
}
