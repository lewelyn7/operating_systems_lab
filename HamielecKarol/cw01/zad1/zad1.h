#ifndef zad1_h
#define zad1_h

struct main_block_arr{
    int size;
    int idx;
    struct edit_block_arr *arr;
};

struct edit_block_arr{
    char * file1;
    char * file2;        
    int size;
    char ** edit_ops;
};

    struct main_block_arr create_main_arr(int size);
    struct main_block_arr define_files_seq(char** files_seq, struct main_block_arr main_arr);
    struct main_block_arr compare(struct main_block_arr starr);
    int get_edit_operations(struct main_block_arr starr, int id);
    struct main_block_arr fill_with_data(struct main_block_arr starr);
    void remove_edit_block(struct   main_block_arr starr, int id);
    void remove_edit_ops(struct edit_block_arr barr, int id);

#endif
//0x0000555555555616
//run create_table 1 compare_pairs a.txt:b.txt