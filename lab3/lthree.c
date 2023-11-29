#include <stdio.h>
#include <stdlib.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define NUM_FRAMES 256
#define ADDRESS_SPACE_SIZE 65536

char physical_memory[ADDRESS_SPACE_SIZE];
char *page_table[PAGE_TABLE_SIZE];
int free_frames[NUM_FRAMES];

char *handlePageFault(int page_number)
{

    int i;
    for (i = 0; i < NUM_FRAMES; i++)
    {
        if (free_frames[i] == 0)
        {
            free_frames[i] = 1;
            break;
        }
    }

    FILE *file = fopen("lab3_data/BACKING_STORE.bin", "rb");
    fseek(file, page_number * FRAME_SIZE, SEEK_SET);
    fread(&physical_memory[i * FRAME_SIZE], sizeof(char), FRAME_SIZE, file);
    fclose(file);

    return &physical_memory[i * FRAME_SIZE];
}

char *getPage(int page_number)
{
    char *page = page_table[page_number];
    if (page == NULL)
    {
        page = handlePageFault(page_number);
        page_table[page_number] = page;
        return page_table[page_number];
    }
    return page_table[page_number];
}

int main(int argc, char const *argv[])
{

    FILE *file = fopen("lab3_data/addresses.txt", "r");

    int page_num;
    int offset;
    int num;
    int value;
    char *frame;
    int phys_addr;

    while (fscanf(file, "%d", &num) == 1)
    {
        offset = num & 0xFF;
        page_num = (num >> 8) & 0xFF;
        frame = getPage(page_num);
        phys_addr = frame - physical_memory + offset;
        value = physical_memory[phys_addr];
        printf("Virtual Address: %d, Physical Address: %d, Value: %d\n", num, phys_addr, value);
    }

    fclose(file);

    return 0;
}
