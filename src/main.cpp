#include "App.h"

int main()
{
    App app;
    app.setup();
    app.loop();

    return 0;
}

// extern "C" {

// using init_fptr = void (*)();

// extern init_fptr __preinit_array_start[];
// extern init_fptr __preinit_array_end[];
// extern init_fptr __init_array_start[];
// extern init_fptr __init_array_end[];

// void __libc_init_array(void)
// {
//     for (init_fptr* p = __preinit_array_start; p < __preinit_array_end; ++p) {
//         (*p)();
//     }
//     for (init_fptr* p = __init_array_start; p < __init_array_end; ++p) {
//         (*p)();
//     }
// }

// void Reset_Handler(void)
// {
//     // 1) copy .data
//     // 2) zero .bss
//     __libc_init_array(); // <-- ДО main()
//     main();
//     while (1) {
//     }
// }

// } // extern "C"
