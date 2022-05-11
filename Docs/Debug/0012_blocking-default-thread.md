commitid d3d02691235c419173085049b4b51af212a1b974

It seems that the default thread is blocking on a `KernelMutex`.

### Notes

-   When debugging, I saw that `m_holding_thread` was null which makes no sense.
    I was messing around with the debugger quite a bit, so this could be a coincidence.

### Ideas

### Theories

### Actions
