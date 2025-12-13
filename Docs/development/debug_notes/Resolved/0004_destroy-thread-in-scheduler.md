commitid 1e6e5ae5e674aa4032153bd07037ea1226b1e6ba

### Notes

-   Somehow, we end up calling `~Thread` in `schedule_next` which seems to cause an infinite loop.

-   When destorying a thread, we seem to release a mutex that causes the thread to be destoryed again.
    Essentially, we are increasing the reference count while the destruction logic runs.

-   The symptom was, that we crashed when running `a` in `Editor.elf`.
    That is gone now, but we crash before `Editor.elf` can come up.

### Conclusions

-   The problem was solved by adding an `m_in_cleanup` member to `RefCounted`.
    That way, we ensure that the destructor is only called once.
