commitid 6a760d0be77e24aa187303c96624302150efc398

It seems that my `RefPtr` implementation is broken somehow.

### Notes

-   I do not understand which constructor was used by `RefCounted::construct` it makes no sense to me.

-   After some testing, it seems that the `RefPtr` implementation itself is fine.

-   It seems that `String` or `StringView` is broken.

    In particular, it seems that `builder.string().view()` is broken, even if we cache the result elsewhere.

-   It turns out that `ImmutableString` includes the null terminator in it's size, thus the resulting view is broken.

### Ideas

### Theories

-   I suspect, that the null terminator is included in the `size()` result.

### Actions

-   The solution was to subtract one when computing the size of a string, I suspect, that this was already correct before I made
    strings immutable.
