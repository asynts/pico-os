export module boot:flash_second_stage;

import boot:high_level;

// Implemented in Assembly.
export extern "C"
void boot_2_flash_second_stage();
