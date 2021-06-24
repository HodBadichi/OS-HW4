#include <assert.h>
#include <iostream>

//change main.cpp to your malloc_3.cpp
#include "malloc_3.cpp"
//compile with g++ -g malloc3_srealloc_tests.cpp malloc_3.cpp -o sreallocTest
//then run
//RUN ONLY ONE TEST AT A TIME!!!!!!!! ¯\_(ツ)_/¯
//goodluck ^_^

static size_t meta_data_size = _size_meta_data();

void malloc3_realloc_note_a_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note a
    char *first = (char *) smalloc(150);
    char *second = (char *) srealloc(first, 149);

    assert(first == second);

    assert(_num_allocated_bytes() == 150);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 1);

}

void malloc3_realloc_note_b_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    bool alloc_size = (300 + meta_data_size) <= 340 ? false : true;
    //realloc note b
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    sfree(first);
    char *third = (char *) srealloc(second, 340);
    if(alloc_size){
        assert(_num_allocated_bytes() == 340 + (meta_data_size - 40));
    } else {
        assert(_num_allocated_bytes() == 340);
    }
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 1);

}

void malloc3_realloc_note_b2_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note b
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    sfree(first);
    char *third = (char *) srealloc(second, 400);
    assert(_num_allocated_bytes() == 550);
    assert(third == second);
    assert(_num_free_blocks() == 1);
    assert(_num_allocated_blocks() == 2);

}

void malloc3_realloc_note_c_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);
    bool alloc_size = (300 + meta_data_size) <= 340 ? false : true;
    //realloc note c
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    sfree(second);
    char *third = (char *) srealloc(first, 340);

    if(alloc_size){
        assert(_num_allocated_bytes() == 340 + (meta_data_size - 40));
    } else {
        assert(_num_allocated_bytes() == 340);
    }
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 1);

}

void malloc3_realloc_note_d_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    bool alloc_size = (450 + 2*meta_data_size) <= 514 ? false : true;
    //realloc note d
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    char *third = (char *) smalloc(150);
    sfree(first);
    sfree(third);
    srealloc(second, 514);

    if(alloc_size){
        assert(_num_allocated_bytes() == 514 + (2*meta_data_size - 64));
    } else {
        assert(_num_allocated_bytes() == 514);
    }
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 1);

}

void malloc3_realloc_note_e_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note e
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    char *third = (char *) smalloc(150);
    char *fourth = (char *) smalloc(600);

    sfree(first);
    sfree(third);
    sfree(fourth);
    srealloc(second, 600);

    assert(_num_allocated_bytes() == (1050 + meta_data_size));
    assert(_num_free_blocks() == 2);
    assert(_num_allocated_blocks() == 3);

}

void malloc3_realloc_note_d2_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note d
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    char *third = (char *) smalloc(150);
    char *fourth = (char *) smalloc(600);

    sfree(first);
    sfree(third);
    sfree(fourth);
    srealloc(second, 1146);
    size_t left_bytes = meta_data_size + 150 + (750 + meta_data_size) + meta_data_size + 150;
    assert(_num_allocated_bytes() == left_bytes);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 1);

}

void malloc3_realloc_note_f_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note d
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);

    srealloc(first, 151);

    assert(_num_allocated_bytes() == 451);
    assert(_num_free_blocks() == 1);
    assert(_num_allocated_blocks() == 3);

}

void malloc3_realloc_note_g_test() {
    assert(_num_allocated_bytes() == 0);
    assert(_num_free_blocks() == 0);
    assert(_num_allocated_blocks() == 0);

    //realloc note g
    char *first = (char *) smalloc(150);
    char *second = (char *) smalloc(150);
    char *third = (char *) smalloc(150);
    char *fourth = (char *) smalloc(600);

    sfree(first);
    sfree(third);
    sfree(fourth);
    srealloc(second, 1300);
    size_t left_bytes = 1300 +300 + meta_data_size;
    assert(_num_allocated_bytes() == left_bytes);
    assert(_num_free_blocks() == 1);
    assert(_num_allocated_blocks() == 2);

}

int main() {

    //RUN ONLY ONE TEST AT A TIME!!!!!!!! ^_^
    //malloc3_realloc_note_a_test();
    //malloc3_realloc_note_b_test();
    //malloc3_realloc_note_b2_test();
    //malloc3_realloc_note_c_test();
   //malloc3_realloc_note_d_test();
   //malloc3_realloc_note_d2_test();
   // malloc3_realloc_note_e_test();
    //malloc3_realloc_note_f_test();
    malloc3_realloc_note_g_test();

    return 0;
}