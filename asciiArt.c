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
     char fileHeader[3];
     fgets(fileHeader, 3, fptr); // put the first line into the header variable;
     fileHeader[2] = '\0';       // ensure null-terminated string

     if (strcmp(fileHeader, "P2") != 0)
     { // checking if it's a PGM file based on the magic header.
          printf("File doesn't appear to be a PGM file in nature, exiting.\n");
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
     unsigned const char charSet[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
     const unsigned char charSetLen = 70;
     if (greyValue == 0)
          return ' ';

     unsigned char index = ((int)greyValue * (charSetLen - 1) + maxValue - 1) / maxValue; // recalculate to a range of 0-69 to be indexed in the charset,
                                                                                          // each corresponding to a different level of "brightness". Rounds up.
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

     FILE *fptr = fopen(argv[1], "r");
     if (checkFile(fptr) != 0)
          return 1;

     char ch;
     ch = getc(fptr);
     while (ch == '#')
     {
          while (getc(fptr) != '\n')
               ;
          ch = getc(fptr);
     }
     ungetc(ch, fptr);

     unsigned int width;
     unsigned int height;

     if (fscanf(fptr, "%u %u", &width, &height) != 2)
     {
          printf("Invalid image size\n");
          return 1;
     }

     unsigned char max_value;
     if (fscanf(fptr, "%hhu", &max_value) != 1)
     {
          printf("Couldn't read max value\n");
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

               printf("%c ", calculateCharacter(greyValue, max_value, threshold));

               if (++x >= width)
               {
                    x = 0;
                    y++;
                    printf("\n");
               }
          }
     }

     fclose(fptr);
     return 0;
}
