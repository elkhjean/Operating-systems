#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *file;
    int num;
    file = fopen("lab3_data/addresses.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    int pageNum;
    int offset;

    while (fscanf(file, "%d", &num) == 1)
    {
        offset = num & 0xFF;
        pageNum = (num >> 8) & 0xFF;

        printf("address: %d, Page number: %d, Page offset: %d\n", num, pageNum, offset);
    }

    fclose(file);

    return 0;
}
