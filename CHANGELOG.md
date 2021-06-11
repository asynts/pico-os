### Features

  - Accept UART via interrupt

  - Execute system calls outside of interrupt handlers and block threads from scheduling

### Bugs

  - Lookup devices via `VirtualFile::m_device_id` instead of runtime objects.
