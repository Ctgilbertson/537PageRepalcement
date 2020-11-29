int main(int argc, char **argv) {
    int opt;
    int page_size;
    int real_mem_size;

    page_size = 0;
    real_mem_size = 100;

    // get simulator params
    while ((opt = getopt(argc, argv, ":p::m::")) != -1) {
        switch (opt) {
            // user indicated a page size
            case 'p':
                page_size = (int)atol(optarg);
                break;
            // user indicated a mem size
            case 'm':
                real_mem_size = (int)atol(optarg);
                break;
            case ':':
                exit(-1);
            default:
                break;
        }
    }

    printf("Page size: %d\n", page_size);
    printf("Real meme size: %d\n", real_mem_size);
}
