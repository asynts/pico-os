export module std;

export extern "C" void _exit(int status) {
    __asm("BKPT #0");
}

export extern "C" void _kill(int pid, int sig) {
    return;
}

export extern "C" int _getpid(void) {
    return -1;
}

unsigned char heap[8 * 1024];
unsigned char *heap_ptr = heap + sizeof(heap);

export extern "C" void* _sbrk(int increment) {
    unsigned char *prev_heap_ptr = heap_ptr;
    heap_ptr += increment;
    return prev_heap_ptr;
}
